/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   printer.hpp
 * Author: tenevoi
 *
 * Created on 10 февраля 2020 г., 13:56
 */

#ifndef PRINTER_HPP
#define PRINTER_HPP
#include "../terminal_control.hpp"
#include <stdio.h>
#include <iostream>
#include<string.h>

using namespace std;
class printer {
public:
    printer(){};
    virtual ~printer(){};

    void printTable(){
        header();
    }
private:
    void data(){
        int str = 4;
        gotoxy(1+1,str);cout <<"1581327616383526540.data";
        gotoxy(col1+1,str);cout << "123.213 MB";
        gotoxy(col1+col2+1,str);cout << "23.147 GB";
        gotoxy(1,str);  cout << "|";
        gotoxy(col1,str); cout << "|";
        gotoxy(col1+col2,str); cout << "|";    
        gotoxy(col1+col2+col3,str);putchar('|');
    }
    void header(){
        int str=1;
        gotoxy(col1,str);cout <<"|" ;for(int i=1;i<col2;i++)putchar('-');putchar('|');
        gotoxy(col2,str);cout <<"|" ;for(int i=1;i<col3;i++)putchar('-');putchar('|');
        gotoxy(col3,str); cout <<"|" ;for(int i=1;i<end;i++)putchar('-');putchar('|');
        gotoxy(end,str);putchar('|');
        
        str++;
        gotoxy(1,str);  cout << "|";
        gotoxy(col1,str); cout << "|";
        gotoxy(col1+col2,str); cout << "|";
        gotoxy(col1+col2+col3,str);putchar('|');
        
        gotoxy(1+1+((col1-strlen(header0))/2),str);  cout << header0;
        gotoxy(col1+1+((col2-strlen(header2))/2),str); cout << header2;
        gotoxy(col1+1+col2+1+((col3-strlen(header3))/2),str); cout << header3;
        
        str++;
        gotoxy(1,str);         cout <<"|" ;for(int i=1;i<col1;i++)putchar(' ');putchar('|');
        gotoxy(col1,str);      cout <<"|" ;for(int i=1;i<col2;i++)putchar(' ');putchar('|');
        gotoxy(col1+col2,str); cout <<"|" ;for(int i=1;i<col3;i++)putchar(' ');putchar('|');  
        gotoxy(col1+col2+col3,str);putchar('|');
        
        str++;
        gotoxy(1,str);         cout <<"|" ;for(int i=1;i<col1;i++)putchar('-');putchar('|');
        gotoxy(col1,str);      cout <<"|" ;for(int i=1;i<col2;i++)putchar('-');putchar('|');
        gotoxy(col1+col2,str); cout <<"|" ;for(int i=1;i<col3;i++)putchar('-');putchar('|');
        gotoxy(col1+col2+col3,str);putchar('|');        
        
        str++;
        gotoxy(1,str);  cout << "|";
        gotoxy(col1,str); cout << "|";
        gotoxy(col1+col2,str); cout << "|";
        gotoxy(col1+col2+col3,str);putchar('|');
        
        gotoxy(1+1,str);  cout << "Текущий фрагмент";
        gotoxy(col1+1,str); cout << "Размер";
        gotoxy(col1+col2+1,str); cout << "";
        gotoxy(col1+col2+col3,str);putchar('|');

        str++;
        gotoxy(1,str);         cout <<"|" ;for(int i=1;i<col1;i++)putchar(' ');putchar('|');
        gotoxy(col1,str);      cout <<"|" ;for(int i=1;i<col2;i++)putchar(' ');putchar('|');
        gotoxy(col1+col2,str); cout <<"|" ;for(int i=1;i<col3;i++)putchar(' ');putchar('|');  
        gotoxy(col1+col2+col3,str);putchar('|');                
        
        str++;
        gotoxy(1,str);         cout <<"|" ;for(int i=1;i<col1;i++)putchar('-');putchar('|');
        gotoxy(col1,str);      cout <<"|" ;for(int i=1;i<col2;i++)putchar('-');putchar('|');
        gotoxy(col1+col2,str); cout <<"|" ;for(int i=1;i<col3;i++)putchar('-');putchar('|');  
        gotoxy(col1+col2+col3,str);putchar('|');        
        
    }

    const int col1=1;
    const int col2=61;
    const int col3=81;
    const int end=91;
    const char *header0="Файл видео данных";
    const char *header1="Текущий фрагмент";
    const char *header2="Размер";
    const char *header3="Свободно";
    
};

#endif /* PRINTER_HPP */

