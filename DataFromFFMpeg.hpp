/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DataFromFFMpeg.hpp
 * Author: tenevoi
 *
 * Created on 20 февраля 2020 г., 11:26
 */

#ifndef DATAFROMFFMPEG_HPP
#define DATAFROMFFMPEG_HPP
/*Структура которую передаёт FF*/
class DataFromFFMpeg {
public:
    DataFromFFMpeg();
    DataFromFFMpeg(const DataFromFFMpeg& orig);
    virtual ~DataFromFFMpeg();
    int index;
    char filename[128];
    long double start;
    float duration;
    long double end;
    char source_dir[512];
    char camname[32];    
private:

};

#endif /* DATAFROMFFMPEG_HPP */

