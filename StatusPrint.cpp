/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "StatusPrint.hpp"
#include <stdio.h>
#include <cstddef>


void PrintStatus(const char * message, ...) {
    if (message == NULL)
        return;
    fprintf(stdout,"\033[19;1H"); 
    fprintf(stdout,"\033[0m");
    fprintf(stdout,"Last action: "); 
    fprintf(stdout, "%s", message);
    const char ** p = &message;
    p++;
    while (*p != NULL)
    {
        fprintf(stdout, "%s\r\n", *p);
        p++;
    }
    fprintf(stdout, "\033[0K");
    fprintf(stdout,"\033[25;1H\r\n");
    fflush(stdout);

}

void PrintError(const char * message, ...) {
    if (message == NULL)
        return;
    fprintf(stderr,"\033[20;1H"); 
    fprintf(stderr,"\033[31m");
    fprintf(stderr,"Last  error: "); 
    fprintf(stderr, "%s", message);
    const char ** p = &message;
    p++;
    while (*p != NULL)
    {
        fprintf(stderr, "%s\r\n", *p);
        p++;
    }
    fprintf(stderr, "\033[0K");
    fprintf(stderr,"\033[25;1H\r\n");
    fflush(stderr);
}