/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VideoFileFragment.hpp
 * Author: tenevoi
 *
 * Created on 11 февраля 2020 г., 15:44
 */

#ifndef VIDEOFILEFRAGMENT_HPP
#define VIDEOFILEFRAGMENT_HPP
#include <cstdint>

class VideoFileFragment {
public:
    VideoFileFragment();
    VideoFileFragment(std::uintmax_t begintTime, std::uintmax_t endTime, std::uintmax_t beginPos, std::uintmax_t endPos);
    VideoFileFragment(const VideoFileFragment& orig);
    virtual ~VideoFileFragment();
    std::uintmax_t begintTime = 0;
    std::uintmax_t endTime = 0;
    std::uintmax_t beginPos = 0;
    std::uintmax_t endPos = 0;
private:

};

#endif /* VIDEOFILEFRAGMENT_HPP */

