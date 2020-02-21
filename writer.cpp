/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: tenevoi
 *
 * Created on 30 января 2020 г., 11:37
 */
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS  64  //Нужно для работы с файлами больше 2GB


#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <map>
#include <vector>

#include <netinet/ip.h>
#include <string.h> //for memset
#include <fstream>  
#include <signal.h>
#include <algorithm>
#include <experimental/filesystem>
#include "terminal_control.hpp"
#include "ms/ms.hpp"
#include "printer/printer.hpp"
#include "Channel/Channel.hpp"
#include "VideoFragment/VideoFileFragment.hpp"
#include "MetaData/MetaData.hpp"
#include "Settings/Settings.hpp"
#include "DataFromFFMpeg/DataFromFFMpeg.hpp"
#include "StatusPrint/StatusPrint.hpp"
#include "Net/Net.hpp"
#include "netMonitor/netMonitor.hpp"

using namespace std;
namespace fs = std::experimental::filesystem;

Settings set;
Net net(set);
netMonitor monitor;

/*действия которые будут происходить при получении сигнала от ядра операционной системы*/
void term_handler(int i) {

    char r[4];
    sprintf(r, "%d", i);
    PrintError("Экстренный выход из программы. по коду ", r, NULL);
    set.stopProgramm = true;
}

/*регистрируем сигналы от системы на которые программа будет реагировать*/
void setExitFunction() {
    struct sigaction sa;
    sigset_t newset;
    sigemptyset(&newset); // чистимся
    sigaddset(&newset, SIGHUP); // добавляем сигнал SIGHUP
    sigprocmask(SIG_BLOCK, &newset, 0); // блокируем его
    sa.sa_handler = term_handler; // указываем обработчик
    sigaction(SIGTERM, &sa, 0); // обрабатываем сигнал SIGTERM
    sigaction(SIGINT, &sa, 0); // обрабатываем сигнал SIGTERM
}

bool FindChannelId(char *channelName, const map<int, Channel*>& channels, int& channelId) {
    auto result = find_if(channels.begin(), channels.end(), [channelName](const pair<int, Channel*> & item) {
        return strcmp(item.second->name, channelName) == 0;
    });
    if (result == channels.end())
        channelId = -1;
    else
        channelId = (*result).first;
    return false;
}

bool AddChannel(const DataFromFFMpeg &resived, map<int, Channel*>& channels, int& channelId) {
    channelId = channels.size() + 1; //прибавим единицу, для того чтобы 0 оставался в резерве.
    Channel *newChannel = new Channel(resived.start, resived.camname);
    pair < map<int, Channel*>::iterator, bool> result = channels.emplace(channelId, newChannel);
    return !result.second;
}

void SetBuffer() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

int UpdatePrefix(string & str) {
    str = to_string(std::chrono::system_clock::now().time_since_epoch().count());
    return 0;
}

int DeleteOldFiles(fs::path & archiveDir, string & currentPrefix) {

    vector<fs::directory_entry> v;

    fs::directory_iterator dir(archiveDir);
    for (fs::directory_entry const &f : dir)
        if (f.path().extension() == ".data")
            v.push_back(f);

    std::sort(v.begin(), v.end());
    if (v.begin() == v.end())
        return -1;

    else
    {
        fs::directory_entry f = *(v.begin());
        if (f.path().stem() != currentPrefix)
        {
            //PrintStatus("removing ", f.path().native(), NULL);
            fs::remove(*v.begin());
        } else
        {
            lasterror;
            cerr << "can't remove file becose file is current\r\n";
            return -2;
        }
    }
    return 0;
}

bool CreateNewDstFile(FILE* &dstFile, string &curPrefix, fs::path &currentDstFile, fs::path &recordDir) {
    if (dstFile != NULL)
    {
        fflush(dstFile);
        fclose(dstFile);
        dstFile = NULL;
    }/*Проверяем открыт ли текущий большой файл видеоданных*/

    /*если файл не открыт то начинаем открывать файл*/

    /*Устанавливаем путь к большому файлу видеоданных*/
    UpdatePrefix(curPrefix); //Обновляем перфикс
    currentDstFile = recordDir;
    currentDstFile += "/";
    currentDstFile += curPrefix;
    currentDstFile += ".data";
    dstFile = fopen(currentDstFile.c_str(), "wb+");
    if (dstFile == NULL)
    {
        //cerr << "Ошибка создания файла видеоданных: " << strerror(errno) << "\r\n";
        return true;
    }
    return false;

}

int main(int argc, char** argv) {
    freopen("/dev/null","a",stdout);
    
    /*Включаем отлавливание прерывание программы*/
    setExitFunction();

    /*Выключаем буффер потоков вывода*/
    SetBuffer();
    invisible_cursor();
    clrscr();
    int mainErrorCode = 0; //Ерор код при выходе из приложения
    FILE * fChannelList = NULL; //Файл в котором находиться информация о списке каналов(имя канала, номер, время первой записи)    
    FILE * srcFile = NULL; /*переменная типа "файл" текущего исходного файла с видеоданными*/
    FILE * dstFile = NULL; /*переменная типа "файл" текущего большого файла с видеоданными*/
    map<int, Channel*> channels; //список каналов 
    error_code ec; /*структура  - Ерор код для работы с файловыми опирациями*/
    uintmax_t currentAvailableSpace = 0; /*Текущее свободное для использования место*/
    string curPrefix; /*текущая приставка к имени файлов индекса и видеоданных равнвна времени создания пары файлов*/
    fs::path currentSourseFile; /*переменная содержащая в себе путь к текущему исходному обрабатываемому файлу с видеоданными*/
    fs::path currentDstFile; /*переменная содержащая в себе путь к текущему большому файлу с видеоданными*/
    uintmax_t currentSrcSize = 0; /*переменная содержащая размер в байтах исходного файла с видеоданными*/
    uintmax_t currentDstSize = 0; /*переменная содержащая текущий размер в байтах большого файла с видеоданными*/
    DataFromFFMpeg recivedDataFF; /*Структура с данными полученными от FFMPEG-a*/
    int currentChannelId = -1; /*текущий номер канала с которым работаем*/
    VideoFileFragment * currentVideoFragment = NULL; /*Данные о текущем фидеофрагменте который должен быть в большом файле*/
    uintmax_t currentStartPosIBigFile = 0;

    gotoxy(column1, 1);
    COLOR(43, 30);
    cout << "DIR: ";
    COLOR(40, 33);
    cout << set.recordDir.string() << "\r\n";
    set_display_atrib(0);

    PrintStatus("Updating prefix", NULL);
    if (UpdatePrefix(curPrefix))
    {
        PrintError("Error update prefix", NULL);
        goto end;
    }

    //инициализируем сеть для подключения ffmpeg-ов
    PrintStatus("Инициализируем сеть для подключения ffmpeg-ов", NULL);
    if (net.NetInit())
    {
        if (set.stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.        
        PrintError("Ошибка инициализации сети.", NULL);
        mainErrorCode = 1;
        goto end;
    }

    gotoxy(column3, 3);
    cout << "Max size:" << ms(set.maxFileSize) << "\r\n";

    /* основной цикл:*/
    while (!set.stopProgramm)
    {
        /*Проверим минимальное доступное место, если размер превышен то пробуем освободить место удаляя самые старые данные*/
        PrintStatus("Проверяем доступное место", NULL);
        currentAvailableSpace = fs::space(set.recordDir, ec).available;
        if (ec.value())
        {
            PrintError("Ошибка проверки доступного места: ", ec.message().c_str(), NULL);
            mainErrorCode = 2;
            goto end;
        }
        gotoxy(column1, 2);
        COLOR(43, 30);
        cout << "Доступно: ";
        COLOR(40, 33);
        cout << ms(currentAvailableSpace) << "\r\n";
        gotoxy(column2, 2);
        cout << "Минимум: " << ms(set.minFreeSpace) << "\r\n";
        resetcolor();

        if (currentAvailableSpace < set.minFreeSpace)
            if (DeleteOldFiles(set.recordDir, curPrefix))
                PrintError("Ошибка при удалении самых старых файлов", NULL);

        /* ждём входящие соеденения*/
        PrintStatus("Ожидаем входящее соеденение", NULL);
        if (net.WaitConnetion())
        {
            if (set.stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
            PrintError("Ошибка во время ожидания соеденения.", NULL);
            mainErrorCode = 3;
            goto end;
        }

        /*получаем данные от FF*/
        PrintStatus("Получаем данные от FFMpeg", NULL);
        if (net.ResiveFromFF(recivedDataFF))
        {
            if (set.stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
            PrintError("Ошибка принятия данных от FF.", NULL);
            mainErrorCode = 4;
            goto end;
        }


        /*нужно проверить есть ли такой канал уже в списке наших каналов*/
        PrintStatus("Проверяем есть ли такой канал в списке", NULL);
        if (FindChannelId(recivedDataFF.camname, channels, currentChannelId))
        {
            PrintError("Ошибка поиска канала в списке каналов. ", recivedDataFF.camname, NULL);
            mainErrorCode = 5;
            goto end;
        }

        /*проверяем если номер канала -1 то добавляем его в список*/
        if (currentChannelId == -1)
        {
            //("Добавляем ", recivedDataFF.camname, " в список каналов.", NULL);
            if (AddChannel(recivedDataFF, channels, currentChannelId))
            {
                PrintError("Ошибка добавления канала в список каналов. ", recivedDataFF.camname, NULL);
                mainErrorCode = 6;
                goto end;
            }
        }

        /*Текущий канал*/
        Channel * currentChannel = channels.at(currentChannelId);

        /*если в канале нет данных о видеофрагментах то значит это будет первый 
         * видеофрагмент и нам нужно записать его время старта которе потом запишем в большой файл*/
        if (currentChannel->videoFragments->empty())
            currentChannel->firstStart = recivedDataFF.start;
        gotoxy(column1, 4 + currentChannelId)cout << "Номер канала: " << currentChannelId << "\r\n";
        gotoxy(column2, 4 + currentChannelId);
        cout << "Name: " << currentChannel->name << "\r\n";
        gotoxy(column3, 4 + currentChannelId);
        cout << "File: " << recivedDataFF.filename << "\r\n";

        /*Читаем фрагмент в память*/
        /*Устанавливаем путь к файлу фрагмента*/
        currentSourseFile = recivedDataFF.source_dir;
        currentSourseFile += "/";
        currentSourseFile += recivedDataFF.filename;

        /*Проверяем существует ли исходный файл фрагмента*/
        //PrintStatus("Проверяем существует ли файл ", currentSourseFile.native().c_str(), NULL);
        if (!fs::exists(currentSourseFile))
        {
            PrintError("Ошибка: файл ", currentSourseFile, " не существует.", NULL);
            continue;
        }

        /*Смотрим размер в байтах исходного файла фрагмента*/
        PrintStatus("Смотрим размер в байтах файла сегмента видео данных", NULL);
        currentSrcSize = fs::file_size(currentSourseFile);
        gotoxy(column5, 4 + currentChannelId);
        cout << "Size: " << ms(currentSrcSize) << "\r\n";

        /*Выделяем место в оперативной памяти для загрузки всего файла*/
        char buffer[currentSrcSize];

        /*Открываем файл для чтения*/
        PrintStatus("Открываем файл для чтения", NULL);
        FILE * srcFile = fopen(currentSourseFile.c_str(), "r");

        /*Читаем содержимое в память*/
        PrintStatus("Читаем содержимое в память", NULL);
        if (fread(buffer, currentSrcSize, 1, srcFile) != 1)
        {
            PrintError("Ошибка при чтении файла ", currentSourseFile, strerror(errno), NULL);
            fclose(srcFile);
            srcFile = NULL;
            continue;
        }

        /*Закрываем файл фрагмента*/
        PrintStatus("Закрываем файл фрагмента", NULL);
        fclose(srcFile);

        /*Проверяем открыт ли текущий большой файл видеоданных*/
        PrintStatus("Проверяем открыт ли текущий большой файл видеоданных, и не превышает ли он заданный размер", NULL);
        if (dstFile == NULL)
        {
            PrintStatus("Создаём новый большой файл видеоданных", NULL);
            if (CreateNewDstFile(dstFile, curPrefix, currentDstFile, set.recordDir))
                PrintError("Ошибка создания файла", NULL);
        }

        /*Выясняем текущий размер большого файла, он же старт текущего сегмента в нем*/
        currentDstSize = currentStartPosIBigFile = fs::file_size(currentDstFile);

        if (currentDstSize + currentSrcSize >= set.maxFileSize)
        {
            MetaData::CloseExistingDstFile(dstFile, channels);

            /*Почистим экран*/
            for (int h = 0; h < channels.size(); h++)
            {
                gotoxy(column3, 5 + h);
                printf(ESC "[0K");
            }

            PrintStatus("Создаём новый большой файл видеоданных", NULL);
            if (CreateNewDstFile(dstFile, curPrefix, currentDstFile, set.recordDir))
                PrintError("Ошибка создания файла", NULL);
        }

        /*Пишем в файл данные которые мы считали ранее*/
        PrintStatus("Пишем в файл данные которые мы считали ранее", NULL);
        if (fwrite(buffer, currentSrcSize, 1, dstFile) != 1)
        {
            PrintError("Ошибка записи данных в файл видеоданных: ", strerror(errno), NULL);
            fclose(dstFile);
            dstFile = NULL;
            continue;
        }
        currentDstSize += currentSrcSize;
        gotoxy(column1, 3);
        cout << "Data-File:" << currentDstFile.filename().string() << "\r\n";
        gotoxy(column4, 3);
        cout << "Size:" << ms(currentDstSize) << "\r\n";

        /*Заставляем систему сбросить все данные из кэша в файл*/
        PrintStatus("Заставляем систему сбросить все данные из кэша в файл", NULL);
        fflush(dstFile);

        /*Удаляем файл фрагмента, так как всё содержимое уже в памяти*/
        PrintStatus("Удаляем файл фрагмента, так как всё содержимое уже в памяти", NULL);
        fs::remove(currentSourseFile);

        /*Создаём структуру с данными о видеоврагменте*/
        currentVideoFragment = new VideoFileFragment(recivedDataFF.start, recivedDataFF.end, currentStartPosIBigFile, currentSrcSize);

        currentChannel->AddVideoFileFragment(currentVideoFragment);
        gotoxy(column6, 4 + currentChannelId);
        cout << "Фрагментов:" << currentChannel->Count() << "   \r\n";
    }

    /* Выход */
end:


    /*Закрываем файл видеоданных если он открыт*/
    PrintStatus("Закрываем файл видеоданных если он открыт", NULL);
    usleep(set.displaytime);
    if (dstFile != NULL)
    {
        fflush(dstFile);
        fclose(dstFile);
        dstFile = NULL;
    }
    visible_cursor();
    set_display_atrib(0);
    gotoxy(1, 30);
    return mainErrorCode;
}

