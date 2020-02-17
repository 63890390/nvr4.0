#include "Channel.hpp"
#include <string.h>
#include <algorithm>

Channel::Channel(const long double firstStart, const char name[]) {
    this->firstStart = firstStart;
    strncpy(this->name,name,sizeof(this->name));
    videoFragments = new std::vector<VideoFileFragment*>();
};

bool Channel::AddVideoFileFragment(VideoFileFragment * fragment){
    videoFragments->push_back(fragment);
    return false;
}

uintmax_t Channel::Count(){
    return videoFragments->size();
}

Channel::~Channel() {
    for(auto &it : *videoFragments){
        delete it;
    }
};
