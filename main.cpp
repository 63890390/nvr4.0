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

#include <netinet/ip.h>
#include <string.h> //for memset
#include <fstream>  
#include <signal.h>
#include <algorithm>
#include <experimental/filesystem>
#include "terminal_control.hpp"
#include "ms.hpp"
#include "printer.hpp"



using namespace std;
namespace fs = std::experimental::filesystem;



//глобальная переменная отвечающая за выход из программы
bool stopProgramm = false;

/*действия которые будут происходить при получении сигнала*/
void term_handler(int i) {
    cout << "Экстренный выход из программы.\r\n";
    stopProgramm = true;
}

/*регистрируем сигналы от системы на которые программа будет реагировать*/
void setExitFunction() {
    struct sigaction sa;
    sigset_t newset;
    sigemptyset(&newset);
    sigaddset(&newset, SIGHUP);
    sigprocmask(SIG_BLOCK, &newset, 0);
    sa.sa_handler = term_handler;
    sigaction(SIGTERM, &sa, 0);
    sigaction(SIGINT, &sa, 0);
}

/* Структура описания канала */
struct Channel {
    char name[32]; //имя канала
    long double firstStart; //время первой записи
};

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
    for (; bind(mainSocket, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0; count++)
    {
        if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
        if (count == 0) cerr << "Warning: Bind fail - " << strerror(errno) << "\r\n" << "Retrying...";
        else putchar('.');
        if (count > countReconect * 10)
            goto end;
        std::this_thread::sleep_for(std::chrono::milliseconds(timePeriodReconect * 100));
    }
    if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
        goto end;
    if (listen(mainSocket, SOMAXCONN) == -1)
        goto end;
    if (count > 0)
        cout << "OK\r\n";
    return false;
end:
    if (count > 0)
        cout << "OK\r\n";
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

bool FindChannelId(char *channelName, const map<int, Channel>& channels, int& channelId) {
    auto result = find_if(channels.begin(), channels.end(), [channelName](const pair<int, Channel> & item) {
        return strcmp(item.second.name, channelName) == 0;
    });
    if (result == channels.end())
        channelId = -1;
    else
        channelId = (*result).first;
    return false;
}

bool AddChannel(const FromFF &resived, map<int, Channel>& channels, int& channelId) {
    channelId = channels.size() + 1; //прибавим единицу, для того чтобы 0 оставался в резерве.
    Channel newChannel;
    newChannel.firstStart = resived.start;
    strcpy(newChannel.name, resived.camname);
    pair < map<int, Channel>::iterator, bool> result = channels.emplace(channelId, newChannel);
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
            cout << "removing " << f.path().native() << "\r\n";
            fs::remove(*v.begin());
        } else
        {
            cout << "can't remove file becose file is current\r\n";
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
    dstFile = fopen(currentDstFile.c_str(), "a+");
    if (dstFile == NULL)
    {
        cerr << "Ошибка создания файла видеоданных: " << strerror(errno) << "\r\n";
        return true;
    }
    return false;

}

int main(int argc, char** argv) {
    //    printer p;
    //    p.printTable();


    /*Включаем отлавливание прерывание программы*/
    setExitFunction();

    /*Выключаем буффер потоков вывода*/
    SetBuffer();
    system("setterm -cursor off");
    int mainErrorCode = 0; //Ерор код при выходе из приложения
    const int incomingTcpPort = 6000; //Входящий порт для подключений FF
    const int countReconect = 10; //Количество попыток переподключений
    const int timePeriodReconect = 10; //Время очерез которое повторяются попытки переподключений
    int mainSocket = -1; //Создаём главный сокет для ожидания подключения
    FILE * fChannelList = NULL; //Файл в котором находиться информация о списке каналов(имя канала, номер, время первой записи)    
    map<int, Channel> channels; //список каналов
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
    gotoxy(1, 1);
    cout << "DIR: " << recordDir.string() << "\r\n";
    gotoxy(1, 10);
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


    if (UpdatePrefix(curPrefix))
    {
        cerr << "Error update prefix\r\n";
        goto end;
    }


    //инициализируем сеть для подключения ffmpeg-ов
    //cout << "инициализируем сеть для подключения ffmpeg-ов" << "\r\n";
    if (NetInit(mainSocket, incomingTcpPort, countReconect, timePeriodReconect))
    {
        if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.        
        cerr << "Ошибка инициализации сети.\r\n";
        mainErrorCode = 1;
        goto end;
    }


    /* основной цикл:*/
    //cout << "Ждём данные..." << "\r\n";
    while (!stopProgramm)
    {
        //home();
        /*Проверим минимальное доступное место, если размер превышен то пробуем освободить место удаляя самые старые данные*/
        currentAvailableSpace = fs::space(recordDir, ec).available;
        if (ec.value())
        {
            cerr << "Ошибка проверки доступного места: " << ec.message() << "\r\n";
            mainErrorCode = 2;
            goto end;
        }
        gotoxy(1, 2);
        cout << "Доступно: " << ms(currentAvailableSpace) << " ";
        cout << "Минимум: " << ms(minFreeSpace) << "                  \r\n";

        if (currentAvailableSpace < minFreeSpace)
            if (DeleteOldFiles(recordDir, curPrefix))
            {
                cerr << "Ошибка при удалении самых старых файлов\r\n";
            }




        /* ждём входящие соеденения*/
        if (WaitConnetion(mainSocket, connectedSocket))
        {
            if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
            cerr << "Ошибка во время ожидания соеденения.\r\n";
            mainErrorCode = 3;
            goto end;
        }


        /*получаем данные от FF*/
        if (ResiveFromFF(connectedSocket, recivedDataFF, 1000))
        {
            if (stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
            cerr << "Ошибка принятия данных от FF.\r\n";
            mainErrorCode = 4;
            goto end;
        }
        /*Закрываем сокет клиента*/
        close(connectedSocket);
        connectedSocket = -1;

        /*нужно проверить есть ли такой канал уже в списке наших каналов*/
        if (FindChannelId(recivedDataFF.camname, channels, currentChannelId))
        {
            cerr << "Ошибка поиска канала в списке каналов. " << recivedDataFF.camname << "\r\n";
            mainErrorCode = 5;
            goto end;
        }

        /*проверяем если номер канала -1 то добавляем его в список*/
        if (currentChannelId == -1 && AddChannel(recivedDataFF, channels, currentChannelId))
        {
            cerr << "Ошибка добавления канала в список каналов. " << recivedDataFF.camname << "\r\n";
            mainErrorCode = 6;
            goto end;
        }

        /*Текущий канал*/
        Channel & currentChannel = channels.at(currentChannelId);
        gotoxy(1, 3)cout << "Номер канала: " << currentChannelId << "                  ";
        gotoxy(20, 3);
        cout << "Name: " << currentChannel.name << "                   ";
        gotoxy(50, 3);
        cout << "File: " << recivedDataFF.filename << "                   \r\n";

        /*Читаем фрагмент в память*/

        /*Устанавливаем путь к файлу фрагмента*/
        currentSourseFile = recivedDataFF.source_dir;
        currentSourseFile += "/";
        currentSourseFile += recivedDataFF.filename;



        /*Проверяем существует ли такой файл*/
        if (!fs::exists(currentSourseFile))
        {
            cerr << "Ошибка: файл " << currentSourseFile << " не существует.\r\n";
            continue;
        }

        /*Смотрим размер в байтах файла сегмента видео данных*/
        currentSrcSize = fs::file_size(currentSourseFile);
        gotoxy(90, 3);
        cout << "Size: " << ms(currentSrcSize) << "                                \r\n";

        /*Выделяем место в оперативной памяти для загрузки всего файла*/
        char buffer[currentSrcSize];


        /*Открываем файл для чтения*/
        FILE * srcFile = fopen(currentSourseFile.c_str(), "r");

        /*Читаем содержимое в память*/
        if (fread(buffer, currentSrcSize, 1, srcFile) != 1)
        {
            cerr << "Ошибка при чтении файла " << currentSourseFile << "\r\n";
            cerr << strerror(errno) << "\r\n";
            fclose(srcFile);
            srcFile = NULL;
            continue;
        }

        /*Закрываем файл фрагмента*/
        fclose(srcFile);





        /*Проверяем открыт ли текущий большой файл видеоданных*/
        if (dstFile == NULL || (currentDstSize+currentSrcSize) >= maxFileSize)
        {
            if (CreateNewDstFile(dstFile, curPrefix, currentDstFile, recordDir))
            {
                cerr << "Ошибка создания файла\r\n";
            }
        }

        /*Пишем в файл данные которые мы считали ранее*/
        if (fwrite(buffer, currentSrcSize, 1, dstFile) != 1)
        {
            cerr << "Ошибка записи данных в файл видеоданных: " << strerror(errno) << "\r\n";
            fclose(dstFile);
            dstFile = NULL;
            continue;
        }
        currentDstSize = fs::file_size(currentDstFile);
        gotoxy(1, 4);
        cout << "Data-File:" << currentDstFile.filename().string() << "                      \r\n";
        gotoxy(50, 4); 
        cout << "Max size:"<<ms(maxFileSize) << "         \r\n";
        gotoxy(90, 4);
        cout << "Size:" << ms(currentDstSize) << "       \r\n";
        
        /*Заставляем систему сбросить все данные из кэша в файл*/
        fflush(dstFile);
        

        /*Удаляем файл фрагмента, так как всё содержимое уже в памяти*/
        fs::remove(currentSourseFile);

    }


    /* Выход */
end:
    /*Закрываем подчинённый сокет*/
    if (connectedSocket != -1)
        close(connectedSocket);
    if (mainSocket != -1)
        close(mainSocket);
    /*Закрываем файл видеоданных если он открыт*/
    if (dstFile != NULL)
    {
        fflush(dstFile);
        fclose(dstFile);
        dstFile = NULL;
    }
    return mainErrorCode;
}

