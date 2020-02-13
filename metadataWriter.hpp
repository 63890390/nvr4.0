/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   metadataWriter.hpp
 * Author: tenevoi
 *
 * Created on 13 февраля 2020 г., 16:41
 */

#ifndef METADATAWRITER_HPP
#define METADATAWRITER_HPP

#include <cstdint>

class metadataWriter {
public:
    metadataWriter();
    metadataWriter(const metadataWriter& orig);
    virtual ~metadataWriter();

    int version;
    int channelCount;
    struct channel {
        int number;
        long double firstStart;
        std::uintmax_t fragmentCount;
        struct fragment {
            long double startTime;
            std::uintmax_t pozition;
            std::uintmax_t size;
            float duration;
        } *fragments;
    } *channels;
    std::uintmax_t metaSize;
private:    
};

#endif /* METADATAWRITER_HPP */

