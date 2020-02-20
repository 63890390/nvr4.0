/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Settings.hpp
 * Author: tenevoi
 *
 * Created on 20 февраля 2020 г., 10:59
 */

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

class Settings {
public:
    Settings();
    Settings(const Settings& orig);
    virtual ~Settings();
    const int incomingTcpPort = 6000; //Входящий порт для подключений FF
    const int countReconect = 10; //Количество попыток переподключений
    const int timePeriodReconect = 10; //Время очерез которое повторяются попытки переподключений    
    const int displaytime = 0; //000000;
    std::uintmax_t minFreeSpace = 1024 * 1024 * 1024 * 23LL; /*Минимальное свободное которое всегда должно быть.*/
    std::uintmax_t maxFileSize = 1024 * 1024 * 128LL; /*Максиммальный размер файла видеоданных*/
    fs::path recordDir = "/var/www/video/archive/record";        /*Путь к директрии с архивом*/
    bool stopProgramm = false;//глобальная переменная отвечающая за выход из программы
private:

};

#endif /* SETTINGS_HPP */

