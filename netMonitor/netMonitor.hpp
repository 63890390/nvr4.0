/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   netMonitor.hpp
 * Author: tenevoi
 *
 * Created on 20 февраля 2020 г., 17:44
 */

#ifndef NETMONITOR_HPP
#define NETMONITOR_HPP

class netMonitor {
public:
    netMonitor();
    netMonitor(const netMonitor& orig);
    virtual ~netMonitor();
    int mainSocket;
    int connectedSocket;
private:

};

#endif /* NETMONITOR_HPP */

