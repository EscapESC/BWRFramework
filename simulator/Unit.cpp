#pragma once
#include <chrono>
#include "Reactor.cpp"

class Unit
{
private:
public:

    Reactor* reactor;

    int maxTicks = 60;

    std::chrono::_V2::system_clock::time_point lastTick;
    std::chrono::_V2::system_clock::duration overSleep;

    int update(){
        auto now = std::chrono::high_resolution_clock::now();
        auto deltaTime = now - lastTick; //nanoseconds

        if(deltaTime.count()/1000000000.0f < 1.0f/maxTicks && maxTicks != 0){
            return 1;
        }
        lastTick = now;

        reactor->update(deltaTime.count() / 1000000000.0f);

        return 0;
    }

    Unit(int maxTicksPerSec = 60,bool circle = true, long long int maxN = 100000000000, int idleN = 1000){
        maxTicks = maxTicksPerSec;
        reactor = new Reactor(circle, maxN, idleN);
        lastTick = std::chrono::high_resolution_clock::now();
    }

    ~Unit(){
        delete reactor;
    }
};