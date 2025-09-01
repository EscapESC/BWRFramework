#pragma once
#include <chrono>
#include "Reactor.cpp"
#include "CoolantLoop.cpp"

class Unit
{
private:
public:

    Reactor* reactor;
    CoolantLoop* coolantLoop;

    int maxTicks; //Yeah, turns out 1000 is still less then 5% of CPU usage.

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
        coolantLoop->update(deltaTime.count() / 1000000000.0f);

        return 0;
    }

    Unit(int maxTicksPerSec = 1000,bool circle = true, long long int maxN = 100000000000, int idleN = 1000){
        maxTicks = maxTicksPerSec;
        
        //REACTOR
        reactor = new Reactor(circle, maxN, idleN);
        //COOLANTLOOP
        coolantLoop = new CoolantLoop(reactor);

        lastTick = std::chrono::high_resolution_clock::now();
    }

    ~Unit(){
        delete reactor;
    }
};