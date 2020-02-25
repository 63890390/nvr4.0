/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   netMonitor.cpp
 * Author: tenevoi
 * 
 * Created on 20 февраля 2020 г., 17:44
 */

#include "netMonitor.hpp"
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <thread>

void proc(int mainSocket) {
    int stdholder = dup(STDOUT_FILENO);
    char b;
    size_t writed, readed;
    while (true)
    {
        int connectedSocket;
        connectedSocket = accept(mainSocket, NULL, NULL);
        writed = write(connectedSocket, "\033[2JPlease enter the magic word and press ENTER\r\nlocalhost ~ # ",
                strlen("\033[2JPlease enter the magic word and press ENTER\r\nlocalhost ~ # ") + 1);
        writed = write(connectedSocket, "\xff\xfb\x01", 3);
        char pass[128] = {0,};
        int simbolnumber = 0;

        while (read(connectedSocket, &b, 1))
        {
            if (b == '\xff')
            {
                std::cerr << "Command ";
                readed = read(connectedSocket, &b, 1);
                std::cerr << (int) b << "value ";
                readed = read(connectedSocket, &b, 1);
                std::cerr << (int) b << "\r\n";
                continue;
            }

            if (b == '\r' || b == '\n' || b == ' ' || simbolnumber >= 127)
            {
                std::cerr << (int) b << "\r\n";
                break;
            }
            writed = write(connectedSocket, "*", 1);
            pass[simbolnumber] = b;
            simbolnumber++;
        }
        pass[simbolnumber] = 0;
        //write(connectedSocket,"\xff\xfc\x01",3);            

        if (strcmp("abcd", pass) != 0)
        {
            writed = write(connectedSocket, "\r\nWRONG!!!\r\n", 12);
            sleep(1);
            close(connectedSocket);
            continue;
        }
        writed = write(connectedSocket, "\255\253\031", 3);
        dup2(connectedSocket, STDOUT_FILENO);
        printf("\033[2J");
        fprintf(stderr, "\033[2J");
        while (read(connectedSocket, &b, 1) != 0)
        {
            if (b == '\xff')
            {
                std::cerr << "Command ";
                readed = read(connectedSocket, &b, 1);
                std::cerr << (int) b << "value ";
                readed = read(connectedSocket, &b, 1);
                std::cerr << (int) b << "\r\n";
                continue;
            }
            std::cerr << b << "\r\n";
        }

        dup2(stdholder, STDOUT_FILENO);
        printf("\033[2J");
        fprintf(stderr, "\033[2J");
    }

}

netMonitor::netMonitor() {
    //std::cout << "Monitor start\r\n";
    struct sockaddr_in serv_addr;
    int enable = 1;
    int count = 0;
    mainSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mainSocket < 0)
        return;
    memset((char *) &serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(6001);
    if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
        return;
    for (; bind(mainSocket, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0; count++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (listen(mainSocket, SOMAXCONN) == -1)
        return;
    static std::thread t1(proc, mainSocket);
    return;
}

netMonitor::netMonitor(const netMonitor& orig) {
}

netMonitor::~netMonitor() {
}

