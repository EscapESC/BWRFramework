#pragma once
#include "config.hpp"

class CAR
{
public:
    bool power = true;
    
    double* condenserPressure;

    float update(double delta){
        if(power){
            if(*condenserPressure < 85126.0){
                power = false;
            }
            else if(*condenserPressure < 86126.0){
                return 0;
            }
            return CAR_RATED_AIR_REMOVAL * delta;
        }
        return 0;
    }
    CAR(double* condPressure){
        condenserPressure = condPressure;
    }
};
