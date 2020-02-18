/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: tenevoi
 *
 * Created on 18 февраля 2020 г., 11:34
 */
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS  64  //Нужно для работы с файлами больше 2GB

#include <cstdlib>
#include <iostream>
#include <experimental/filesystem>
#include <cstring>
#include"../MetaData.hpp"

using namespace std;

/*
 * 
 */
using namespace std;
namespace fs = std::experimental::filesystem;

bool checkArhiveFile(fs::path f) {

    MetaData md;
    FILE *file = fopen(f.c_str(), "rb");
    if (file == NULL)
    {
        cerr << "error open file";
        cerr << strerror(errno);
        return true;
    }


    
    if (fseeko64(file, -((int)sizeof(md.okbyte)), SEEK_END) != 0)
    {
        cerr << "fseek error1";
        cerr << strerror(errno);
        fclose(file);
        return true;
    }
    int r;
    if ((r = fread(&md.okbyte, sizeof (md.okbyte), 1, file)) != 1)
    {
        cerr << "error read ok byte r= " << r ;
        cerr << strerror(errno);
        fclose(file);
        return true;
    }
    if (md.okbyte != 85)
    {
        cerr << "Byteok =" << (int) md.okbyte << "\r\n";
        return true;
    }else cout << "Byte ok = " << (int) md.okbyte <<"\r\n";


    if (fseeko64(file, -((int)sizeof(md.p4) + (int)sizeof(md.okbyte)), SEEK_END) != 0)
    {
        cerr << "fseek error2";
        cerr << strerror(errno);
        fclose(file);
        return true;
    }
    fread(&md.p4, sizeof (md.p4), 1, file);
    if (fseeko64(file, md.p4, SEEK_SET) != 0)
    {
        cerr << "fseek error3";
        cerr << strerror(errno);
        fclose(file);
        return true;
    }



    fread(&md.version, sizeof (md.version), 1, file);
    cout << "version " << md.version << "\r\n";

    fread(&md.channelCount, sizeof (md.channelCount), 1, file);
    cout << "Channel count " << md.channelCount << "\r\n";

    md.channels = (MetaData::channel*)malloc(sizeof(MetaData::channel)*md.channelCount);
    for(int i=0;i<md.channelCount;i++){
        fread(&md.channels[i].number, sizeof (md.channels[i].number), 1, file);
        fread(&md.channels[i].firstStart, sizeof (md.channels[i].firstStart), 1, file);
        fread(&md.channels[i].fragmentCount, sizeof (md.channels[i].fragmentCount), 1, file);
        fread(&md.channels[i].startPosFirstFragment, sizeof (md.channels[i].startPosFirstFragment), 1, file);
        
        md.channels[i].fragments = (MetaData::channel::fragment*)malloc(sizeof(MetaData::channel::fragment)*md.channels[i].fragmentCount);
    }
    for(int i=0;i<md.channelCount;i++){
        cout << "Ch.Num = " << (std::uintmax_t)md.channels[i].number << "\t";
        cout << "F.Start = " << md.channels[i].firstStart << "\t";
        cout << "Frag.Count = " << md.channels[i].fragmentCount << "\t";
        cout << "Poz = " << md.channels[i].startPosFirstFragment << "\t";
        cout << "\r\n";
    }
    
    
    
    fclose(file);
    return false;

}

int main(int argc, char** argv) {
    fs::path recordDir = "/var/www/video/archive/record";

    fs::directory_iterator dir(recordDir);
    for (fs::directory_entry const &f : dir)
        if (f.path().extension() == ".data")
        {
            if (!checkArhiveFile(f))
                cout << f << "\r\n";
        }


    return 0;
}

