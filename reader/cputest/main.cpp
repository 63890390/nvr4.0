/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: tenevoi
 *
 * Created on 20 февраля 2020 г., 10:36
 */

#include <cstdlib>
#include <cstdint>
#include <stdio.h>

using namespace std;

void Myprint(const char* first, ...) {


    printf("%s\r\n", first);
    const char ** p = &first;
    p++;
    while (*p != NULL)
    {
        printf("%s\r\n", *p);
        p++;
    }

}

/*
 * 
 */
int main(int argc, char** argv) {

    Myprint("hello", "world","My","Name","is Dima", NULL);
    return 0;
}

