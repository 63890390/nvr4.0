#include "Channel.hpp"
#include <string.h>
#include <algorithm>

Channel::Channel(const std::uintmax_t firstStart, const char name[]) {
    this->firstStart = firstStart;
    strncpy(this->name,name,sizeof(this->name));
    videoFragments = new std::vector<VideoFileFragment*>();
};

bool Channel::AddVideoFileFragment(VideoFileFragment * fragment){
    videoFragments->push_back(fragment);
    return false;
}

size_t Channel::Count(){
    return videoFragments->size();
}

Channel::~Channel() {
    for(auto &it : *videoFragments){
        delete it;
    }
};
