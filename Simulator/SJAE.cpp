#pragma once
#include "config.hpp"
#include "Reactor.cpp"

class SJAE
{
private:
    Reactor* reactor;
public:
    bool power = true;

    float valve = 100.0;
    double* condenserPressure;

    float update(float delta){
        valve = std::clamp(valve,100.0f,0.0f);
        if(power){
            return (valve/100.0f) * SJAE_RATED_AIR_REMOVAL * (reactor->pressure-*condenserPressure)/7141300 * delta;
        }
        return 0;
    }

    SJAE(Reactor* reactorPointer, double* condenserPressurePointer){
        reactor = reactorPointer;
        condenserPressure = condenserPressurePointer;
    }
};