#pragma once
#include "Channel.cpp"
#include "PhysicsMath.cpp"
#include "RecirculationPump.cpp"
#include <bits/stdc++.h>
#include <iostream>
#include <vector>

class Reactor
{
private:
    float rp_deltacounter = 0;
    long long int rp_oldneutrons = 0;
public:

    static const int MAX_THERMAL_POWER = 3200; //MW
    static const int MAX_ELETRIC_POWER = 1000; //MW

    static const int SIZE = 8;
    static const int HEIGHT = 10;

    const double vesselVolume = 120.0;         // m³, total vessel volume (water + steam)
    const double Ru = 8.314;                  // J/(mol·K)
    const double M = 0.018;                   // kg/mol

    Channel* channels[SIZE][SIZE][HEIGHT] = {};
    long double changeMetrix[SIZE][SIZE][HEIGHT] = {};

    unsigned const int c_w = 4186;
    unsigned const int L_v = 2260000;

    double waterAmount = 100000.0f; //Kg
    double steamAmount = 0;

    float pressure = 101325; //Pa

    float waterTemperature = 20; //°C

    float avarage_controlRodPosition = 0;//output

    double reactorPeriod;//output
    double reactorPeriod_precise;//output

    float avarage_iodine = 0;//output
    float avarage_xenon = 0;//output

    long long int total_neutrons = 0;
    int numberOfChannels = 0;
    int idleNeutrons = 0;

    unsigned long long int maxNeutrons = 100000000000;

    unsigned long long int oldNeutrons;

    double reactor_power = 0;//output

    std::vector<RecirculationPump> rcPumps;

    Reactor(bool circle = true, unsigned long long int maxN = 100000000000, unsigned int idleN = 1000, unsigned int water = 100000, unsigned int numOfRcPumps = 2){
        //POPULATE THE REACTOR WITH CHANNELS
        const float center = (SIZE - 1) / 2.0f;
        const float radius = SIZE / 2.0f;
        waterAmount = water;

        for (int i = 0; i < numOfRcPumps; i++)
        {
            rcPumps.emplace_back(numOfRcPumps);
        }
        

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < HEIGHT; z++) {
                    float dx = x - center;
                    float dy = y - center;
                    if(circle){
                        if (dx * dx + dy * dy <= radius * radius) {
                            channels[x][y][z] = new Channel(maxN, idleN);
                            numberOfChannels++;
                        }
                        else {
                            channels[x][y][z] = nullptr;
                        }
                    }
                    else{
                        channels[x][y][z] = new Channel(maxN, idleN);
                        numberOfChannels++;
                    }
                }
            }
        }
        maxNeutrons = maxN * numberOfChannels;
        idleNeutrons = idleN * numberOfChannels;
    }

    void update(float delta){
        std::memset(changeMetrix, 0, sizeof(changeMetrix));

        oldNeutrons = total_neutrons;
        avarage_controlRodPosition = 0;
        total_neutrons = 0;

        avarage_iodine = 0;
        avarage_xenon = 0;

        float rcPumpPW = 0;
        for (int i = 0; i < rcPumps.size(); i++)
        {
            rcPumpPW = rcPumps[i].update(reactor_power);
        }

        //UPDATE CHANNELS
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < HEIGHT; z++) {
                    if(channels[x][y][z] != nullptr){
                        channels[x][y][z]->update(delta, waterTemperature, rcPumpPW/rcPumps.size());
                        total_neutrons += channels[x][y][z]->neutrons;
                        avarage_controlRodPosition += channels[x][y][z]->controlRodPosition;
                        redistributeNeutrons(channels[x][y][z], x,y,z,channels,changeMetrix);
                        
                        avarage_iodine += channels[x][y][z]->iodine;
                        avarage_xenon += channels[x][y][z]->xenon;
                    }
                }
            }
        }
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < HEIGHT; z++) {
                    if(channels[x][y][z] != nullptr){
                        channels[x][y][z]->oldNeutrons += changeMetrix[x][y][z];
                    }
                }
            }
        }

        //WATER BOILING
        waterTemperature += ((long double)total_neutrons / (long double)maxNeutrons) * (float)MAX_THERMAL_POWER * 1000000.0f * delta / (4200.0f * waterAmount);

        float surplus = std::max((float)(waterTemperature - calculateBoilingPoint(pressure)),0.0f);
        if(surplus > 0.0f){
            float amountBoiled = surplus * waterAmount * c_w / L_v;
            waterAmount -= amountBoiled;
            steamAmount += amountBoiled;

            if(waterAmount < 0){
                steamAmount += waterAmount;
                waterAmount = 0;
            }

            waterTemperature -= surplus;

        }

        double waterDensity = PhysicsMath::waterDensity(waterTemperature);
        double waterVolume = waterAmount / 1000.0f;
        double steamVolume = vesselVolume - waterVolume;
        steamVolume = std::max(0.1, steamVolume);
        double steamPressureCoefficient = (Ru / (M * steamVolume)) * 373.0;
        pressure = steamAmount * steamPressureCoefficient * (waterTemperature + 273.0) / 373.0 + 101325.0f;

        avarage_iodine /= numberOfChannels;
        avarage_xenon /= numberOfChannels;

        avarage_controlRodPosition = avarage_controlRodPosition / numberOfChannels;

        //REACTOR PERIODs
        rp_deltacounter += delta;
        if(rp_deltacounter >= 1.0f){
            if (rp_oldneutrons > 0 && total_neutrons > 0) {
                reactorPeriod = rp_deltacounter / log((long double)total_neutrons / (long double)rp_oldneutrons);
            } else {
                reactorPeriod = INFINITY; // avoid div by zero
            }
            rp_oldneutrons = oldNeutrons;
            rp_deltacounter -= 1.0f;
        }
        if (oldNeutrons> 0 && total_neutrons > 0) {
                reactorPeriod_precise = delta / log((long double)total_neutrons / (long double)oldNeutrons);
            } else {
                reactorPeriod_precise = INFINITY; // avoid div by zero
            }

        reactor_power = (long double)total_neutrons / (long double)maxNeutrons * 100.0f;
    }

    int moveRod(unsigned int x,unsigned int y, float rodPosition){

        rodPosition = std::clamp(rodPosition, 0.0f,100.0f);
        
        if(channels[x][y][0] == nullptr){return -1;}

        float decrementedPosition = rodPosition;

        const float section = 100.0f/(float)HEIGHT;

        for(int z = HEIGHT-1; z >= 0; z--){
            if(decrementedPosition - section >= 0){
                channels[x][y][z]->controlRodPosition = 100;
                decrementedPosition -= section;
            }
            else{
                channels[x][y][z]->controlRodPosition = decrementedPosition/section*100.0f;
                for(int tz = z-1; tz >= 0; tz--){
                    channels[x][y][tz]->controlRodPosition = 0;
                }
                return 0;
            }
        }
        return 0;

    }

    float getRodPos(unsigned int x,unsigned int y){

        float rodPosition = 0;

        for(int z = 0; z < HEIGHT; z++){
            if(channels[x][y][z]->controlRodPosition == 100){
                rodPosition += std::abs(z-HEIGHT)*100;
                return (float)rodPosition/(float)HEIGHT;
            }
            rodPosition += channels[x][y][z]->controlRodPosition;
        }
        return (float)rodPosition/(float)HEIGHT;
    }

    int redistributeNeutrons(Channel* channel, int x, int y, int z, Channel* channels[SIZE][SIZE][HEIGHT],long double tempMatrix[SIZE][SIZE][HEIGHT]){
        channel->neutrons *= 0.75;
        channel->oldNeutrons /= 4.0;
        
        for (int tx = -1; tx < 2; tx++)
        {
            for (int ty = -1; ty < 2; ty++)
            {
                for (int tz = -1; tz < 2; tz++)
                {
                    if(!(tx == 0 && ty == 0 && tz == 0) && !((tx+x < 0 || tx+x >= SIZE) || (ty+y < 0 || ty+y >= SIZE) || (tz+z < 0 || tz+z >= HEIGHT))){
                        int dist = std::abs(tx)+std::abs(ty)+std::abs(tz);
                        if(dist == 2){
                            if(channels[tx+x][ty+y][tz+z] != nullptr){
                                tempMatrix[tx+x][ty+y][tz+z] += channel->neutrons/24.0;
                            }
                            else{
                                tempMatrix[x][y][z] += channel->neutrons/48.0;
                            }
                        }
                        else if(dist == 1){
                            if(channels[tx+x][ty+y][tz+z] != nullptr){
                                tempMatrix[tx+x][ty+y][tz+z] += channel->neutrons/12.0;
                            }
                            else{
                                tempMatrix[x][y][z] += channel->neutrons/24.0;
                            }
                        }
                    }
                }    
            }
        }
        return 0;
    }

    ~Reactor() {
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            for (int z = 0; z < HEIGHT; z++) {
                delete channels[x][y][z];
            }
        }
    }
}

};
