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

#ifndef METADATA_HPP
#define METADATA_HPP

#include <cstdint>
#include <map>
#include <unistd.h>
#include "../Channel/Channel.hpp"
#include "../terminal_control.hpp"
using namespace std;

class MetaData {
public:

    MetaData();

    char okbyte = 85;
    int version = 1;
    int channelCount = 0;
    off64_t p1, p2, p3, p4;

    struct channel {
        int number = 0;
        long double firstStart = 0;
        std::uintmax_t fragmentCount = 0;
        off64_t startPosFirstFragment = 0;

        struct fragment {
            long double startTime = 0;
            off64_t pozition = 0;
            std::uintmax_t size = 0;
            float duration = 0;
        } *fragments = nullptr;
    } *channels = nullptr;
    std::uintmax_t metaSize = 0;

    virtual ~MetaData();

    static bool CloseExistingDstFile(FILE* &dstFile, map<int, Channel*> &channels);
};

#endif /* METADATAWRITER_HPP */

