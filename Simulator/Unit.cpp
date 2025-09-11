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

    static const int MAX_TPS_REACTOR = 1;
    static const int MAX_TPS_COOLANT = 20; 

    std::chrono::_V2::system_clock::time_point reactor_lastTick;
    std::chrono::_V2::system_clock::time_point coolant_lastTick;

    int update(){
        auto now = std::chrono::high_resolution_clock::now();
        auto reactor_deltaTime = now - reactor_lastTick;
        auto turbine_deltaTime = now - coolant_lastTick; //nanoseconds

        if(reactor_deltaTime.count()/1000000000.0 > 1.0/MAX_TPS_REACTOR && MAX_TPS_REACTOR != 0){
            reactor->update(reactor_deltaTime.count() / 1000000000.0);
            reactor_lastTick = now;
        }

        if(reactor_deltaTime.count()/1000000000.0 > 1.0/MAX_TPS_COOLANT && MAX_TPS_COOLANT != 0){
            coolantLoop->update(turbine_deltaTime.count() / 1000000000.0);
            coolant_lastTick = now;
        }
        return 0;
    }

    Unit(bool circle = true, long long int maxN = 100000000000, int idleN = 1000){
        
        //REACTOR
        reactor = new Reactor(circle, maxN, idleN);
        //COOLANTLOOP
        coolantLoop = new CoolantLoop(reactor);

        reactor_lastTick = std::chrono::high_resolution_clock::now();
        coolant_lastTick = std::chrono::high_resolution_clock::now();
    }

    ~Unit(){
        delete reactor;
        delete coolantLoop;
    }
};