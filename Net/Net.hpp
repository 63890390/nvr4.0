/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Net.hpp
 * Author: tenevoi
 *
 * Created on 20 февраля 2020 г., 14:45
 */

#ifndef NET_HPP
#define NET_HPP

#include <cstdio>
#include "../DataFromFFMpeg/DataFromFFMpeg.hpp"
#include "../Settings/Settings.hpp"

class Net {
public:
    Net(Settings &set);
    Net(const Net& orig);
    virtual ~Net();
    bool NetInit();
    bool WaitConnetion();
    bool ResiveFromFF(DataFromFFMpeg &resivedData);
    int bufferSize = 1024;
private:
    Settings *set = NULL;
    int mainSocket = -1; //Создаём главный сокет для ожидания подключения
    int connectedSocket = -1; //подчинёный сокет для соеденившегося FF   
};

#endif /* NET_HPP */

