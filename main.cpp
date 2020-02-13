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
#include "ms.hpp"
#include "printer.hpp"
#include "Channel.hpp"
#include "VideoFileFragment.hpp"
#include "metadataWriter.hpp"
const int displaytime = 0; //000000;



using namespace std;
namespace fs = std::experimental::filesystem;


//глобальная переменная отвечающая за выход из программы
bool stopProgramm = false;

/*действия которые будут происходить при получении сигнала*/
void term_handler(int i) {

    lasterror;
    cerr << "Экстренный выход из программы. по коду " << i << "\r\n";
    stopProgramm = true;
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

/*Структура которую передаёт FF*/
struct FromFF {
    int index;
    char filename[128];
    long double start;
    float duration;
    long double end;
    char source_dir[512];
    char camname[32];
};

bool NetInit(int& mainSocket, const int& incomingPort, int countReconect, int timePeriodReconect) {

    struct sockaddr_in serv_addr;
    int enable = 1;
    int count = 0;
    mainSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mainSocket < 0)
        goto end;
    memset((char *) &serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(incomingPort);
    if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
        goto end;
    for (; bind(mainSocket, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0; count++)
    {
        if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
        if (count == 0)
        {
            lasterror;
            cerr << "Warning: Bind fail - " << strerror(errno) << "Retrying...";
        } else putchar('.');
        if (count > countReconect * 10)
            goto end;
        std::this_thread::sleep_for(std::chrono::milliseconds(timePeriodReconect * 100));
    }
    if (listen(mainSocket, SOMAXCONN) == -1)
        goto end;
    if (count > 0)
    {
        currentaction();
        cout << "Net initialization OK\r\n";
        usleep(displaytime);
    }
    return false;
end:
    if (count > 0)
    {
        currentaction();
        cout << "Net initialization OK\r\n";
        usleep(displaytime);
    }
    if (mainSocket != -1)
        close(mainSocket);
    return true;
}

bool WaitConnetion(const int &mainSocket, int &connectedSocket) {
    connectedSocket = accept(mainSocket, NULL, NULL);
    if (connectedSocket < 0)
        return true;
    return false;
}

bool ResiveFromFF(const int &connectedSocket, FromFF &resivedData, size_t bufferSize) {
    char buf[bufferSize];
    size_t resivedByte;
    resivedByte = read(connectedSocket, buf, bufferSize);
    if (resivedByte == bufferSize || resivedByte == -1)
        return true;
    buf[resivedByte] = 0; //Добавим 0 для символа конца строки.
    if (sscanf(buf,
        "c:%s\ti:%d\tf:%s\ts:%Lf\td:%f\te:%Lf\tdir:%s",
        resivedData.camname, &resivedData.index, resivedData.filename,
        &resivedData.start, &resivedData.duration, &resivedData.end, resivedData.source_dir) == EOF)
        return true;
    return false;
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

bool AddChannel(const FromFF &resived, map<int, Channel*>& channels, int& channelId) {
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
            currentaction();
            cout << "removing " << f.path().native() << "\r\n";
            usleep(displaytime);
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

bool CloseExistingDstFile(FILE* &dstFile, map<int, Channel*> &channels) {
    if (dstFile == NULL)
        return true;
    metadataWriter writer;
    writer.version = 1;
    writer.channelCount = channels.size();

    writer.channels = (metadataWriter::channel*)malloc(sizeof (metadataWriter::channel) * writer.channelCount);
    int i = 0;
    for (const auto &ch : channels)
    {
        writer.channels[i].firstStart = ch.second->firstStart;
        writer.channels[i].fragmentCount = ch.second->Count();
        writer.channels[i].fragments = (metadataWriter::channel::fragment*)malloc(sizeof(metadataWriter::channel::fragment)*writer.channels[i].fragmentCount);
        for(const auto &fr : *(ch.second->videoFragments))
        {
            printf("%Lf\t",fr->begintTime);                        
        }
        cout << "\r\n";
        i++;
    }


    return false;
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
    dstFile = fopen(currentDstFile.c_str(), "a+");
    if (dstFile == NULL)
    {
        //cerr << "Ошибка создания файла видеоданных: " << strerror(errno) << "\r\n";
        return true;
    }
    return false;

}

int main(int argc, char** argv) {



    int color = 30;
    /*Включаем отлавливание прерывание программы*/
    setExitFunction();

    /*Выключаем буффер потоков вывода*/
    SetBuffer();
    invisible_cursor();
    clrscr();
    //system("setterm -cursor off >/dev/null");
    int mainErrorCode = 0; //Ерор код при выходе из приложения
    const int incomingTcpPort = 6000; //Входящий порт для подключений FF
    const int countReconect = 10; //Количество попыток переподключений
    const int timePeriodReconect = 10; //Время очерез которое повторяются попытки переподключений
    int mainSocket = -1; //Создаём главный сокет для ожидания подключения
    FILE * fChannelList = NULL; //Файл в котором находиться информация о списке каналов(имя канала, номер, время первой записи)    
    map<int, Channel*> channels; //список каналов
    int connectedSocket = -1; //подчинёный сокет для соеденившегося FF    

    /*Минимальное свободное которое всегда должно быть.*/
    std::uintmax_t minFreeSpace = 1024 * 1024 * 1024 * 23LL;

    /*Максиммальный размер файла видеоданных*/
    std::uintmax_t maxFileSize = 1024 * 1024 * 50LL;

    /*структура  - Ерор код для работы с файловыми опирациями*/
    std::error_code ec;

    /*Текущее свободное для использования место*/
    std::uintmax_t currentAvailableSpace = 0;

    /*Путь к директрии с архивом*/
    fs::path recordDir = "/var/www/video/archive/record";
    gotoxy(column1, 1);
    COLOR(43, 30);
    cout << "DIR: ";
    COLOR(40, 33);
    cout << recordDir.string() << "\r\n";
    set_display_atrib(0);
    /*текущая приставка к имени файлов индекса и видеоданных равнвна времени создания пары файлов*/
    string curPrefix;

    /*переменная содержащая в себе путь к текущему исходному обрабатываемому файлу с видеоданными*/
    fs::path currentSourseFile;

    /*переменная содержащая в себе путь к текущему большому файлу с видеоданными*/
    fs::path currentDstFile;

    /*переменная содержащая размер в байтах исходного файла с видеоданными*/
    std::uintmax_t currentSrcSize = 0;

    /*переменная содержащая текущий размер в байтах большого файла с видеоданными*/
    std::uintmax_t currentDstSize = 0;

    /*переменная типа "файл" текущего исходного файла с видеоданными*/
    FILE * srcFile = NULL;

    /*переменная типа "файл" текущего большого файла с видеоданными*/
    FILE * dstFile = NULL;

    /*Структура с данными полученными от FFMPEG-a*/
    FromFF recivedDataFF;

    /*текущий номер канала с которым работаем*/
    int currentChannelId = -1;

    /*Данные о текущем фидеофрагменте который должен быть в большом файле*/
    VideoFileFragment * currentVideoFragment = NULL;

    uintmax_t currentStartPosIBigFile = 0;

    currentaction();
    cout << "Updating prefix";
    usleep(displaytime);
    if (UpdatePrefix(curPrefix))
    {
        lasterror;
        cerr << "Error update prefix\r\n";
        goto end;
    }


    //инициализируем сеть для подключения ffmpeg-ов
    currentaction();
    cout << "Инициализируем сеть для подключения ffmpeg-ов";
    usleep(displaytime);
    if (NetInit(mainSocket, incomingTcpPort, countReconect, timePeriodReconect))
    {
        if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.        
        lasterror;
        cerr << "Ошибка инициализации сети.\r\n";
        mainErrorCode = 1;
        goto end;
    }


    gotoxy(column3, 3);
    cout << "Max size:" << ms(maxFileSize) << "\r\n";

    /* основной цикл:*/
    while (!stopProgramm)
    {
        /*Проверим минимальное доступное место, если размер превышен то пробуем освободить место удаляя самые старые данные*/
        currentaction();
        cout << "Проверяемдоступное место";
        usleep(displaytime);
        currentAvailableSpace = fs::space(recordDir, ec).available;
        if (ec.value())
        {
            lasterror;
            cerr << "Ошибка проверки доступного места: " << ec.message() << "\r\n";
            mainErrorCode = 2;
            goto end;
        }
        gotoxy(column1, 2);
        COLOR(43, 30);
        cout << "Доступно: ";
        COLOR(40, 33);
        cout << ms(currentAvailableSpace) << "\r\n";
        gotoxy(column2, 2);
        cout << "Минимум: " << ms(minFreeSpace) << "\r\n";
        resetcolor();

        if (currentAvailableSpace < minFreeSpace)
            if (DeleteOldFiles(recordDir, curPrefix))
            {
                lasterror;
                cerr << "Ошибка при удалении самых старых файлов\r\n";
            }




        /* ждём входящие соеденения*/
        currentaction();
        cout << "Ожидаем входящее соеденение";
        usleep(displaytime);
        if (WaitConnetion(mainSocket, connectedSocket))
        {
            if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
            lasterror;
            cerr << "Ошибка во время ожидания соеденения.\r\n";
            mainErrorCode = 3;
            goto end;
        }


        /*получаем данные от FF*/
        currentaction();
        cout << "Получаем данные от FFMpeg";
        usleep(displaytime);
        if (ResiveFromFF(connectedSocket, recivedDataFF, 1000))
        {
            if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
            lasterror;
            cerr << "Ошибка принятия данных от FF.\r\n";
            mainErrorCode = 4;
            goto end;
        }

        /*Закрываем сокет клиента*/
        currentaction();
        cout << "Закрываем сокет клиента";
        usleep(displaytime);
        close(connectedSocket);
        connectedSocket = -1;

        /*нужно проверить есть ли такой канал уже в списке наших каналов*/
        currentaction();
        cout << "Проверяем есть ли такой канал в списке";
        usleep(displaytime);
        if (FindChannelId(recivedDataFF.camname, channels, currentChannelId))
        {
            lasterror;
            cerr << "Ошибка поиска канала в списке каналов. " << recivedDataFF.camname << "\r\n";
            mainErrorCode = 5;
            goto end;
        }

        /*проверяем если номер канала -1 то добавляем его в список*/
        if (currentChannelId == -1)
        {
            currentaction();
            cout << "Добавляем " << recivedDataFF.camname << " в список каналов.";
            usleep(displaytime);
            if (AddChannel(recivedDataFF, channels, currentChannelId))
            {
                lasterror;
                cerr << "Ошибка добавления канала в список каналов. " << recivedDataFF.camname << "\r\n";
                mainErrorCode = 6;
                goto end;
            }
        }

        /*Текущий канал*/
        Channel * currentChannel = channels.at(currentChannelId);
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
        currentaction();
        cout << "Проверяем существует ли файл " << currentSourseFile;
        usleep(displaytime);
        if (!fs::exists(currentSourseFile))
        {
            lasterror;
            cerr << "Ошибка: файл " << currentSourseFile << " не существует.\r\n";
            continue;
        }

        /*Смотрим размер в байтах исходного файла фрагмента*/
        currentaction();
        cout << "Смотрим размер в байтах файла сегмента видео данных";
        usleep(displaytime);
        currentSrcSize = fs::file_size(currentSourseFile);
        gotoxy(column5, 4 + currentChannelId);
        cout << "Size: " << ms(currentSrcSize) << "\r\n";

        /*Выделяем место в оперативной памяти для загрузки всего файла*/
        char buffer[currentSrcSize];


        /*Открываем файл для чтения*/
        currentaction();
        cout << "Открываем файл для чтения";
        usleep(displaytime);
        FILE * srcFile = fopen(currentSourseFile.c_str(), "r");

        /*Читаем содержимое в память*/
        currentaction();
        cout << "Читаем содержимое в память";
        usleep(displaytime);
        if (fread(buffer, currentSrcSize, 1, srcFile) != 1)
        {
            lasterror;
            cerr << "Ошибка при чтении файла " << currentSourseFile << " ";
            cerr << strerror(errno) << "\r\n";
            fclose(srcFile);
            srcFile = NULL;
            continue;
        }

        /*Закрываем файл фрагмента*/
        currentaction();
        cout << "Закрываем файл фрагмента";
        usleep(displaytime);
        fclose(srcFile);





        /*Проверяем открыт ли текущий большой файл видеоданных*/
        currentaction();
        cout << "Проверяем открыт ли текущий большой файл видеоданных, и не превышает ли он заданный размер";
        usleep(displaytime);
        if (dstFile == NULL)
        {
            currentaction();
            cout << "Создаём новый большой файл видеоданных";
            usleep(displaytime);
            if (CreateNewDstFile(dstFile, curPrefix, currentDstFile, recordDir))
            {
                lasterror;
                cerr << "Ошибка создания файла\r\n";
            }
        }

        /*Выясняем текущий размер большого файла, он же старт текущего сегмента в нем*/
        currentDstSize = currentStartPosIBigFile = fs::file_size(currentDstFile);


        if (currentDstSize + currentSrcSize >= maxFileSize)
        {
            CloseExistingDstFile(dstFile, channels);

            currentaction();
            cout << "Создаём новый большой файл видеоданных";
            usleep(displaytime);
            if (CreateNewDstFile(dstFile, curPrefix, currentDstFile, recordDir))
            {
                lasterror;
                cerr << "Ошибка создания файла\r\n";
            }
        }


        /*Пишем в файл данные которые мы считали ранее*/
        currentaction();
        cout << "Пишем в файл данные которые мы считали ранее";
        usleep(displaytime);
        if (fwrite(buffer, currentSrcSize, 1, dstFile) != 1)
        {
            lasterror;
            cerr << "Ошибка записи данных в файл видеоданных: " << strerror(errno) << "\r\n";
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
        currentaction();
        cout << "Заставляем систему сбросить все данные из кэша в файл";
        usleep(displaytime);
        fflush(dstFile);



        /*Удаляем файл фрагмента, так как всё содержимое уже в памяти*/
        currentaction();
        cout << "Удаляем файл фрагмента, так как всё содержимое уже в памяти";
        usleep(displaytime);
        fs::remove(currentSourseFile);



        /*Создаём структуру с данными о видеоврагменте*/
        currentVideoFragment = new VideoFileFragment(recivedDataFF.start, recivedDataFF.end, currentStartPosIBigFile, currentSrcSize);



        currentChannel->AddVideoFileFragment(currentVideoFragment);
        gotoxy(column6, 4 + currentChannelId);
        cout << "Фрагментов:" << currentChannel->Count() << "   \r\n";

    }


    /* Выход */
end:
    /*Закрываем подчинённый сокет*/
    currentaction();
    cout << "Закрываем подчинённый сокет";
    usleep(displaytime);
    if (connectedSocket != -1)
        close(connectedSocket);
    if (mainSocket != -1)
        close(mainSocket);
    /*Закрываем файл видеоданных если он открыт*/
    currentaction();
    cout << "Закрываем файл видеоданных если он открыт";
    usleep(displaytime);
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

