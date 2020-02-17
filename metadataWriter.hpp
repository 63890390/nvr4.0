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
    //    metadataWriter(const metadataWriter& orig);
    virtual ~metadataWriter();

    int version = 1;
    int channelCount = 0;

    struct channel {
        int number = 0;
        long double firstStart = 0;
        std::uintmax_t fragmentCount = 0;
        std::uintmax_t startPosFirstFragment = 0;

        struct fragment {
            long double startTime = 0;
            std::uintmax_t pozition = 0;
            std::uintmax_t size = 0;
            float duration = 0;
        } *fragments = nullptr;
    } *channels = nullptr;
    std::uintmax_t metaSize = 0;
private:
};

#endif /* METADATAWRITER_HPP */

