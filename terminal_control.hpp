/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   terminal_control.hpp
 * Author: tenevoi
 *
 * Created on 6 февраля 2020 г., 17:55
 */

#ifndef TERMINAL_CONTROL_HPP
#define TERMINAL_CONTROL_HPP

#include <cstdio>


#define ESC "\033"

//Format text
#define RESET 		0
#define BRIGHT 		1
#define DIM			2
#define UNDERSCORE	3
#define BLINK		4
#define REVERSE		5
#define HIDDEN		6

//Foreground Colours (text)

#define F_BLACK 	30
#define F_RED		31
#define F_GREEN		32
#define F_YELLOW	33
#define F_BLUE		34
#define F_MAGENTA 	35
#define F_CYAN		36
#define F_WHITE		37

//Background Colours
#define B_BLACK 	40
#define B_RED		41
#define B_GREEN		42
#define B_YELLOW	44
#define B_BLUE		44
#define B_MAGENTA 	45
#define B_CYAN		46
#define B_WHITE		47


#define columnsize 20
#define column1 1
#define column2 column1+columnsize
#define column3 column2+columnsize
#define column4 column3+columnsize
#define column5 column4+columnsize
#define column6 column5+columnsize
#define column7 column6+columnsize
#define column8 column7+columnsize
#define column9 column8+columnsize
#define column10 column9+columnsize
#define column11 column10+columnsize
#define column12 column11+columnsize
#define column13 column12+columnsize
#define column14 column13+columnsize
#define column15 column14+columnsize
#define column16 column15+columnsize
#define column17 column16+columnsize
#define column18 column17+columnsize
#define column19 column18+columnsize
#define column20 column19+columnsize


#define savecursor()            printf(ESC "[s");
#define restorcurcor()          printf(ESC "[u");
#define obr()                   printf(ESC "[51m");
#define home() 			printf(ESC "[H") //Move cursor to the indicated row, column (origin at 1,1)
#define clrscr()		printf(ESC "[2J") //lear the screen, move to (1,1)
#define gotoxy(x,y)		printf(ESC "[%lu;%luH", (long unsigned int)y, (long unsigned int)x); for(int i=0;i<columnsize;i++)putchar(' '); printf(ESC "[%lu;%luH", (long unsigned int)y, (long unsigned int)x);
#define visible_cursor()        printf(ESC "[?25h");
#define invisible_cursor()      printf(ESC "[?25l");
//Set Display Attribute Mode	<ESC>[{attr1};...;{attrn}m
#define resetcolor() printf(ESC "[0m")
#define set_display_atrib(color) 	printf(ESC "[%dm",color)
#define COLOR(f,b)		printf(ESC "[%d;%dm",f,b);




#define currentaction() gotoxy(1,19); set_display_atrib(0);cout << "Last action: "; printf(ESC "[0K");  
#define lasterror gotoxy(1,20);     set_display_atrib(31); cerr << "Last  error: "; printf(ESC "[0K"); 

#endif /* TERMINAL_CONTROL_HPP */

