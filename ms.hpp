/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ms.hpp
 * Author: tenevoi
 *
 * Created on 7 февраля 2020 г., 12:48
 */

#ifndef MS_HPP
#define MS_HPP
#include <iostream>

class ms {
public:

    ms(long long bytes) {
        this->bytes = bytes;
    }
    
    ms(const ms& orig){};
    virtual ~ms(){};

    operator const char*() {
        FormatBytes();
        return str;
    }

    std::string operator<<(const char* s) {
        std::cout << s;
    }


private:

    char str[32];
    long long bytes;

    void FormatBytes() {
        const char *sizes[5] = {"B", "KB", "MB", "GB", "TB"};
        int i;
        double dblByte = bytes;
        for (i = 0; i < 5 && bytes >= 1024; i++, bytes /= 1024)
            dblByte = bytes / 1024.0;
        sprintf(str, "%.3f %s", dblByte, sizes[i]);
    }

};

#endif /* MS_HPP */

