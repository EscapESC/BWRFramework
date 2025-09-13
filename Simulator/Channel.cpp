#pragma once
#include "PhysicsMath.cpp"
#include <iostream>

class Channel
{
private:

public:

    float controlRodPosition = 0;

    float iodine = 0.0f; // use "(iodine/6)*100.0f" for display use with 50% being normal operation
    float xenon = 0.0f; // use "(xenon/0.6)*100.0f" for display use with 50% being normal operation

    long double neutrons = 0;
    int idleNeutrons = 1000;
    unsigned long long int maxNeutrons = 100000000000;

    long double oldNeutrons = 0;

    double power;

    const float I_prod_factor     = 7.3e-5;    // iodine yield per fission 
    const float I_decay_factor    = 2.93e-5;   // I-135 half-life ~6.6h
    const float Xe_decay_factor   = 2.11e-5;   // Xe-135 half-life ~9.2h
    const float Xe_absorb_factor  = 2.2e-4;    // tuned so xenon burns out fast enough at 100% power


    Channel(long long int maxN = 100000000000, int idleN = 1000){
        maxNeutrons = maxN;
        idleNeutrons = idleN;
    }

    void update(double delta, float waterTemp, float recirculationPumpPW){

        //XENON/IODINE stuff
        iodine += neutrons/maxNeutrons * I_prod_factor * delta;

        float iDecay = iodine * I_decay_factor * delta;

        iodine -= iDecay;
        xenon += iDecay;

        xenon -= xenon * Xe_decay_factor * delta;
        xenon -= xenon * neutrons / maxNeutrons * Xe_absorb_factor * delta;

        xenon = std::max(0.0f, xenon);
        iodine = std::max(0.0f, iodine);
        ////////////////////////////////

        double voidCoefficient = (1 - 0.3 * neutrons / maxNeutrons) * (1-recirculationPumpPW);
        double waterDensity = PhysicsMath::waterDensity(waterTemp);

        long double factor = 2.0 * ( 0.4 + controlRodPosition * 0.6 / 100.0 ) * (((waterDensity / 1000.0 - 1.0) * 0.5) + 1.0) * expf(-0.067 * xenon) * voidCoefficient;

        neutrons = factor * (idleNeutrons + oldNeutrons);

        //neutrons = (neutrons - oldNeutrons) * (long double)delta + oldNeutrons; //Delta time implementation

        //Min-max checks
        if(neutrons < 0){neutrons = 0;}
        oldNeutrons = neutrons;
        power = neutrons / (long double)maxNeutrons;
    }

};