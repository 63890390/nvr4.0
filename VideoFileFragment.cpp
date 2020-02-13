/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VideoFileFragment.cpp
 * Author: tenevoi
 * 
 * Created on 11 февраля 2020 г., 15:44
 */

#include "VideoFileFragment.hpp"

VideoFileFragment::VideoFileFragment() {
}

VideoFileFragment::VideoFileFragment(long double begintTime, long double endTime, std::uintmax_t beginPos, std::uintmax_t size) {
    this->begintTime = begintTime;
    this->endTime = endTime;
    this->beginPos = beginPos;
    this->size = size;
}

VideoFileFragment::VideoFileFragment(const VideoFileFragment& orig) {
}

VideoFileFragment::~VideoFileFragment() {
}

