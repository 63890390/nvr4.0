/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Net.cpp
 * Author: tenevoi
 * 
 * Created on 20 февраля 2020 г., 14:45
 */

#include "../Net/Net.hpp"
#include "../StatusPrint/StatusPrint.hpp"
#include "../Settings/Settings.hpp"
#include <netinet/ip.h>
#include <string.h>
#include <thread>
#include <unistd.h>

Net::Net(Settings &set) {
    this->set = &set;

}

Net::Net(const Net& orig) {
}

Net::~Net() {
    /*Закрываем подчинённый сокет*/
    PrintStatus("Закрываем подчинённый сокет", NULL);
    if (connectedSocket != -1)
        close(connectedSocket);
    if (mainSocket != -1)
        close(mainSocket);
}

bool Net::NetInit() {

    struct sockaddr_in serv_addr;
    int enable = 1;
    int count = 0;
    mainSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mainSocket < 0)
        goto end;
    memset((char *) &serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(set->incomingTcpPort);
    if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
        goto end;
    for (; bind(mainSocket, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0; count++)
    {
        if (set->stopProgramm) goto end; //если получен сигнал о выходе то идем на выход без сообщения об ошибке.
        if (count == 0)
            PrintError("Warning: Bind fail - ", strerror(errno), "Retrying...", NULL);
        else putchar('.');
        if (count > set->countReconect * 10)
            goto end;
        std::this_thread::sleep_for(std::chrono::milliseconds(set->timePeriodReconect * 100));
    }

    if (listen(mainSocket, SOMAXCONN) == -1)
        goto end;

    PrintStatus("Net initialization OK", NULL);
    return false;
end:
    if (mainSocket != -1)
        close(mainSocket);
    PrintError("Net initialization ERROR", NULL);
    return true;
}

bool Net::WaitConnetion() {
    connectedSocket = accept(mainSocket, NULL, NULL);
    if (connectedSocket < 0)
        return true;
    return false;
}

bool Net::ResiveFromFF(DataFromFFMpeg &resivedData) {
    char buf[bufferSize];
    size_t resivedByte = read(connectedSocket, buf, bufferSize);

    /*Закрываем сокет клиента*/
    PrintStatus("Закрываем сокет клиента", NULL);
    close(connectedSocket);
    connectedSocket = -1;

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