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
#include "Channel.hpp"
#include "terminal_control.hpp"
using namespace std;

class MetaData {
public:

    MetaData() {
    };
    //    metadataWriter(const metadataWriter& orig);

    virtual ~MetaData() {
    };
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

    static bool CloseExistingDstFile(FILE* &dstFile, map<int, Channel*> &channels) {
        if (dstFile == NULL)
            return true;
        std::uintmax_t i = 0;
        std::uintmax_t j = 0;
        MetaData writer;
        writer.version = 1;
        writer.channelCount = channels.size();
        writer.channels = (MetaData::channel*)malloc(sizeof (MetaData::channel) * writer.channelCount);

        i = 0;
        for (const auto &ch : channels) {
            writer.channels[i].number = ch.first;
            writer.channels[i].firstStart = ch.second->firstStart;
            writer.channels[i].fragmentCount = ch.second->Count();
            writer.channels[i].fragments = (MetaData::channel::fragment*)malloc(sizeof (MetaData::channel::fragment) * writer.channels[i].fragmentCount);

            j = 0;
            for (const auto &fr : *(ch.second->videoFragments)) {
                writer.channels[i].fragments[j].duration = fr->begintTime - fr->endTime;
                writer.channels[i].fragments[j].startTime = fr->begintTime;
                writer.channels[i].fragments[j].pozition = fr->beginPos;
                writer.channels[i].fragments[j].size = fr->size;
                j++;
            }
            cout << "\r\n";
            i++;
        }


        /*Дописываем все в конец*/

        //Записываем текущую позицию в файле    
        fseeko64(dstFile, 0, SEEK_END);
        writer.p1 = ftello64(dstFile);

        /*записываем версию метаданных*/
        fwrite(&writer.version, sizeof (writer.version), 1, dstFile);
        /*записываем количество каналов в файле*/
        fwrite(&writer.channelCount, sizeof (writer.channelCount), 1, dstFile);




        writer.p2 = ftello64(dstFile);
        
        gotoxy(1, 30);printf(ESC "[0K"); 
        fseeko64(dstFile, writer.p2, SEEK_SET);
        printf("1 start poz = %lld\t",ftello64(dstFile));
        for (int i = 0; i < writer.channelCount; i++) {
            fwrite(&writer.channels[i].number, sizeof (writer.channels[i].number), 1, dstFile);
            fwrite(&writer.channels[i].firstStart, sizeof (writer.channels[i].firstStart), 1, dstFile);
            fwrite(&writer.channels[i].fragmentCount, sizeof (writer.channels[i].fragmentCount), 1, dstFile);
            fwrite(&writer.channels[i].startPosFirstFragment, sizeof (writer.channels[i].startPosFirstFragment), 1, dstFile);
        }
        printf("1 end poz = %lld\r\n",ftello64(dstFile));
        fflush(stdout);

//        writer.p3 = ftello64(dstFile);
//        for (int i = 0; i < writer.channelCount; i++) {
//            writer.channels[i].startPosFirstFragment = ftello64(dstFile);
//            for (int j = 0; j < writer.channels[i].fragmentCount; j++) {
//                fwrite(&writer.channels[i].fragments[j].startTime, sizeof (writer.channels[i].fragments[j].startTime), 1, dstFile);
//                fwrite(&writer.channels[i].fragments[j].pozition, sizeof (writer.channels[i].fragments[j].pozition), 1, dstFile);
//                fwrite(&writer.channels[i].fragments[j].size, sizeof (writer.channels[i].fragments[j].size), 1, dstFile);
//            }
//        }
//        writer.p4 = ftello64(dstFile);

        gotoxy(1, 31);printf(ESC "[0K"); 
        fseeko64(dstFile, writer.p2, SEEK_SET);
        printf("2 start poz = %lld   ",ftello64(dstFile));
        fflush(stdout);
        
        for (int i = 0; i < writer.channelCount; i++) {
            fwrite(&writer.channels[i].number, sizeof (writer.channels[i].number), 1, dstFile);
            fwrite(&writer.channels[i].firstStart, sizeof (writer.channels[i].firstStart), 1, dstFile);
            fwrite(&writer.channels[i].fragmentCount, sizeof (writer.channels[i].fragmentCount), 1, dstFile);
            fwrite(&writer.channels[i].startPosFirstFragment, sizeof (writer.channels[i].startPosFirstFragment), 1, dstFile);
        }
        printf("2 end poz = %lld\r\n",ftello64(dstFile));
        fflush(stdout);
        sleep(5);
        cout << "\r\n\r\n";

        fseeko64(dstFile, writer.p4, SEEK_SET);
        auto s = sizeof (writer.p1);
        fwrite(&writer.p1, s, 1, dstFile);

        fwrite(&writer.okbyte, sizeof (writer.okbyte), 1, dstFile);

        fflush(dstFile);
        fclose(dstFile);
        dstFile = NULL;

        return false;
    }

private:
};

#endif /* METADATAWRITER_HPP */

