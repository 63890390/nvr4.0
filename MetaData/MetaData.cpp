/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "MetaData.hpp"


MetaData::MetaData(){
    
}
MetaData::~MetaData(){
    
}

bool MetaData::CloseExistingDstFile(FILE* &dstFile, map<int, Channel*> &channels) {
    if (dstFile == NULL)
        return true;
    std::uintmax_t i = 0;
    std::uintmax_t j = 0;
    MetaData md;
    md.version = 1;
    md.channelCount = channels.size();
    md.channels = (MetaData::channel*)malloc(sizeof (MetaData::channel) * md.channelCount);

    i = 0;
    for (const auto &ch : channels)
    {
        md.channels[i].number = ch.first;
        md.channels[i].firstStart = ch.second->firstStart;
        md.channels[i].fragmentCount = ch.second->Count();
        md.channels[i].fragments = (MetaData::channel::fragment*)malloc(sizeof (MetaData::channel::fragment) * md.channels[i].fragmentCount);

        j = 0;
        for (const auto &fr : *(ch.second->videoFragments))
        {
            md.channels[i].fragments[j].duration = fr->begintTime - fr->endTime;
            md.channels[i].fragments[j].startTime = fr->begintTime;
            md.channels[i].fragments[j].pozition = fr->beginPos;
            md.channels[i].fragments[j].size = fr->size;
            j++;
        }
        i++;
    }


    /*Дописываем все в конец*/

    /*перематываем в конец файла*/
    fseeko64(dstFile, 0, SEEK_END);

    /*Записываем текущую позицию в файле*/
    md.p1 = ftello64(dstFile);

    /*записываем версию метаданных*/
    fwrite(&md.version, sizeof (md.version), 1, dstFile);

    /*записываем количество каналов в файле*/
    fwrite(&md.channelCount, sizeof (md.channelCount), 1, dstFile);


    /*запоминаем позицию в файле*/
    md.p2 = ftello64(dstFile);

    /*Записываем структуры с данными о каналах*/
    fseeko64(dstFile, md.p2, SEEK_SET);
    for (int i = 0; i < md.channelCount; i++)
    {
        fwrite(&md.channels[i].number, sizeof (MetaData::channel::number), 1, dstFile);
        fwrite(&md.channels[i].firstStart, sizeof (MetaData::channel::firstStart), 1, dstFile);
        fwrite(&md.channels[i].fragmentCount, sizeof (MetaData::channel::fragmentCount), 1, dstFile);
        fwrite(&md.channels[i].startPosFirstFragment, sizeof (MetaData::channel::startPosFirstFragment), 1, dstFile);
    }

    /*запоминаем позицию в файле*/
    md.p3 = ftello64(dstFile);

    /*записываем структуры с данными о видео фрагментах*/
    for (int i = 0; i < md.channelCount; i++)
    {
        md.channels[i].startPosFirstFragment = ftello64(dstFile);
        for (int j = 0; j < md.channels[i].fragmentCount; j++)
        {
            fwrite(&md.channels[i].fragments[j].startTime, sizeof (MetaData::channel::fragment::startTime), 1, dstFile);
            fwrite(&md.channels[i].fragments[j].duration, sizeof (MetaData::channel::fragment::duration), 1, dstFile);
            fwrite(&md.channels[i].fragments[j].pozition, sizeof (MetaData::channel::fragment::pozition), 1, dstFile);
            fwrite(&md.channels[i].fragments[j].size, sizeof (MetaData::channel::fragment::size), 1, dstFile);
        }
    }

    /*запоминаем позицию в файле*/
    md.p4 = ftello64(dstFile);

    /*возвращаемся к позиции записи структур с данными о каналах*/
    fseeko64(dstFile, md.p2, SEEK_SET);

    /*Перезаписываем структуры с данными о каналах*/
    for (int i = 0; i < md.channelCount; i++)
    {
        fwrite(&md.channels[i].number, sizeof (MetaData::channel::number), 1, dstFile);
        fwrite(&md.channels[i].firstStart, sizeof (MetaData::channel::firstStart), 1, dstFile);
        fwrite(&md.channels[i].fragmentCount, sizeof (MetaData::channel::fragmentCount), 1, dstFile);
        fwrite(&md.channels[i].startPosFirstFragment, sizeof (MetaData::channel::startPosFirstFragment), 1, dstFile);
    }

    /*Возвращаемся к месту где записываем позицию начала метаданных*/
    fseeko64(dstFile, md.p4, SEEK_SET);
    auto s = sizeof (md.p1);
    fwrite(&md.p1, s, 1, dstFile);

    /*Записываем ОК byte*/
    fwrite(&md.okbyte, sizeof (md.okbyte), 1, dstFile);

    fflush(dstFile);
    fclose(dstFile);
    dstFile = NULL;

    /*освобождаем память*/
    for (int ii = 0; ii < i; ii++)
    {
        free(md.channels[ii].fragments);
        md.channels[ii].fragments = nullptr;
    }
    free(md.channels);
    md.channels = nullptr;

    /*обнуляем данные о фрагментах*/
    for (pair<int, Channel*> ch : channels)
    {
        Channel *currentChannel = ch.second;
        for (VideoFileFragment* vf : *(currentChannel->videoFragments))
            delete(vf);
        currentChannel->videoFragments->clear();
    }




    return false;
}