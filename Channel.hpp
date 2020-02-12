/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Channel.hpp
 * Author: tenevoi
 *
 * Created on 11 февраля 2020 г., 13:05
 */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <cstdint> //for std::uintmax_t
#include <vector>

#include "VideoFileFragment.hpp"
class Channel {
public:
    Channel();
    //Channel(const Channel& orig);
    virtual ~Channel();
    
    char name[32]={0,};
    std::uintmax_t firstStart;
    std::vector<VideoFileFragment *> videoFragments;
    
private:

};

#endif /* CHANNEL_HPP */