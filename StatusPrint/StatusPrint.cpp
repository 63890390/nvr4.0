/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "StatusPrint.hpp"
#include <stdio.h>
#include <cstddef>
#include <sys/socket.h>
#include "../netMonitor/netMonitor.hpp"
#include "string.h"
#include <thread>
#include <iostream>

//void PrintStatus(const char * message, ...) {
//    static long count=0;    
//    static netMonitor monitor = netMonitor();
////    monitor.connected = false;
////    if (message == NULL)
////        return;
////    char buffer[1024]={0,};
////    strcat(buffer,message);
////    const char ** p = &message;
////    (char *)p++;
////    while (*p != NULL)
////    {
////        strcat(buffer,*p);
////        p++;
////    }
////    strcat(buffer,"\r\n");
////    if(monitor.connected)send(monitor.connectedSocket, buffer,strlen(buffer),0);
//    count++;
//    
//}

void PrintStatus(const char * message, ...) {
    if (message == NULL)
        return;
    fprintf(stdout, "\033[19;1H");
    fprintf(stdout, "\033[0m");
    fprintf(stdout, "Last  status: ");
    fprintf(stdout, "%s", message);
    const char ** p = &message;
    p++;
    while (*p != NULL)
    {
        fprintf(stdout, "%s", *p);
        p++;
    }
    fprintf(stdout, "\033[0K");
    fprintf(stdout, "\033[25;1H\r\n");
    fflush(stdout);
}


void PrintError(const char * message, ...) {
    return;
    if (message == NULL)
        return;
    fprintf(stderr, "\033[20;1H");
    fprintf(stderr, "\033[31m");
    fprintf(stderr, "Last  error: ");
    fprintf(stderr, "%s", message);
    const char ** p = &message;
    p++;
    while (*p != NULL)
    {
        fprintf(stderr, "%s\r\n", *p);
        p++;
    }
    fprintf(stderr, "\033[0K");
    fprintf(stderr, "\033[25;1H\r\n");
    fflush(stderr);
}