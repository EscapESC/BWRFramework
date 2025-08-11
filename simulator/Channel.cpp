#pragma once
#include "PhysicsMath.cpp"

class Channel
{
private:

public:

    float controlRodPosition = 100;

    float iodine;
    float xenon;

    long long int neutrons = 0;
    int idleNeutrons = 1000;
    long long int maxNeutrons = 100000000000;

    long long int oldNeutrons = 0;

    float power;

    Channel(long long int maxN = 100000000000, int idleN = 1000){
        maxNeutrons = maxN;
        idleNeutrons = idleN;
    }

    void update(float delta){
        float factor = 2 * (1 - (controlRodPosition/100));

        neutrons = factor * (idleNeutrons + oldNeutrons);

        neutrons = ((neutrons - oldNeutrons) * delta) + oldNeutrons; //Delta time implementation

        //Min-max checks
        if(neutrons < 0){neutrons = 0;}
        oldNeutrons = neutrons;
        power = neutrons / maxNeutrons;
    }

};