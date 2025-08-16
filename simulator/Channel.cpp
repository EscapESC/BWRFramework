#pragma once
#include "PhysicsMath.cpp"

class Channel
{
private:

public:

    float controlRodPosition = 100;

    float iodine = 0;
    float xenon = 0;

    long long int neutrons = 0;
    int idleNeutrons = 1000;
    long long int maxNeutrons = 100000000000;

    long long int oldNeutrons = 0;

    float waterDensity = 1000;

    float power;

    const float I_prod_factor     = 0.000073;   // I-135 production from fission
    const float I_decay_factor    = 0.000105;   // I-135 decay constant
    const float Xe_decay_factor   = 0.000021;   // Xe-135 radioactive decay
    const float Xe_absorb_factor  = 0.000052;   // Xe-135 neutron absorption at full power

    Channel(long long int maxN = 100000000000, int idleN = 1000){
        maxNeutrons = maxN;
        idleNeutrons = idleN;
    }

    void update(float delta){

        //XENON/IODINE stuff
        iodine += (long double)neutrons/(long double)maxNeutrons * I_prod_factor * delta;

        float iDecay = iodine * I_decay_factor * delta;

        iodine -= iDecay;
        xenon += iDecay;

        xenon -= xenon * Xe_decay_factor * delta;
        xenon -= xenon * neutrons / maxNeutrons * Xe_absorb_factor * delta;

        xenon = std::max(0.0f, xenon);
        iodine = std::max(0.0f, iodine);
        ////////////////////////////////

        long double factor = 4.81f * ( 0.2f + (100.0f - controlRodPosition) * 0.8f / 100.0f ) * waterDensity / 1000.0f * ( 1 - xenon / 4.5 );

        neutrons = factor * (idleNeutrons + oldNeutrons);

        neutrons = ((neutrons - oldNeutrons) * delta) + oldNeutrons; //Delta time implementation

        //Min-max checks
        if(neutrons < 0){neutrons = 0;}
        oldNeutrons = neutrons;
        power = neutrons / maxNeutrons;
    }

};