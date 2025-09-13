#pragma once
#include "Channel.cpp"
#include "PhysicsMath.cpp"
#include "RecirculationPump.cpp"
#include <bits/stdc++.h>
#include <iostream>
#include <vector>

#include "config.hpp"

class Reactor
{
private:
    float rp_deltacounter = 0;
    long long int rp_oldneutrons = 0;
public:

    const double Ru = 8.314;                  // J/(mol·K)
    const double M = 0.018;                   // kg/mol

    Channel* channels[REACTOR_CORE_SIZE][REACTOR_CORE_SIZE][REACTOR_CORE_HEIGHT] = {};
    long double changeMetrix[REACTOR_CORE_SIZE][REACTOR_CORE_SIZE][REACTOR_CORE_HEIGHT] = {};

    unsigned const int c_w = 4186;
    unsigned const int L_v = 2260000;

    double waterAmount = 100000.0f; //Kg
    double steamAmount = 0;

    long double pressure = 101325.0; //Pa

    long double waterTemperature = 20.0; //°C

    double avarage_controlRodPosition = 0;//output

    double reactorPeriod;//output
    double reactorPeriod_precise;//output

    float avarage_iodine = 0;//output
    float avarage_xenon = 0;//output

    long double total_neutrons = 0;
    unsigned int numberOfChannels = 0;
    unsigned int idleNeutrons = 0;

    unsigned long long int maxNeutrons = 100000000000;

    unsigned long long int oldNeutrons;

    double reactor_power = 0;//output

    std::vector<RecirculationPump> rcPumps;

    Reactor(unsigned int water = 100000){
        //POPULATE THE REACTOR WITH CHANNELS
        const float center = (REACTOR_CORE_SIZE - 1) / 2.0f;
        const float radius = REACTOR_CORE_SIZE / 2.0f;
        waterAmount = water;

        for (int i = 0; i < NUM_OF_RC_PUMP; i++)
        {
            rcPumps.emplace_back(NUM_OF_RC_PUMP);
        }
        

        for (int x = 0; x < REACTOR_CORE_SIZE; x++) {
            for (int y = 0; y < REACTOR_CORE_SIZE; y++) {
                for (int z = 0; z < REACTOR_CORE_HEIGHT; z++) {
                    float dx = x - center;
                    float dy = y - center;
                    if(REACTOR_CORE_CIRCLE){
                        if (dx * dx + dy * dy <= radius * radius) {
                            channels[x][y][z] = new Channel(MAXIMUM_NEUTRON_COUNT, IDLE_NEUTRON_COUNT);
                            numberOfChannels++;
                        }
                        else {
                            channels[x][y][z] = nullptr;
                        }
                    }
                    else{
                        channels[x][y][z] = new Channel(MAXIMUM_NEUTRON_COUNT, IDLE_NEUTRON_COUNT);
                        numberOfChannels++;
                    }
                }
            }
        }
        maxNeutrons = MAXIMUM_NEUTRON_COUNT * numberOfChannels;
        idleNeutrons = IDLE_NEUTRON_COUNT * numberOfChannels;
    }

    void update(double delta){
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
        for (int x = 0; x < REACTOR_CORE_SIZE; x++) {
            for (int y = 0; y < REACTOR_CORE_SIZE; y++) {
                for (int z = 0; z < REACTOR_CORE_HEIGHT; z++) {
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
        for (int x = 0; x < REACTOR_CORE_SIZE; x++) {
            for (int y = 0; y < REACTOR_CORE_SIZE; y++) {
                for (int z = 0; z < REACTOR_CORE_HEIGHT; z++) {
                    if(channels[x][y][z] != nullptr){
                        channels[x][y][z]->oldNeutrons += changeMetrix[x][y][z];
                    }
                }
            }
        }
        //WATER BOILING
        if(!waterAmount == 0){
            waterTemperature += ((long double)total_neutrons / (long double)maxNeutrons) * (double)REACTOR_MAX_THERMAL_POWER * 1000000.0 * delta / (4200.0 * waterAmount);
        }
        
        double surplus = std::max((double)(waterTemperature - PhysicsMath::calculateBoilingPoint(pressure)),0.0);
        if(surplus > 0.0f){
            double amountBoiled = surplus * waterAmount * c_w / L_v;
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
        double steamVolume = REACTOR_VOLUME - waterVolume;
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

    int moveRod(unsigned int x, unsigned int y, float rodPosition) {

        rodPosition = std::clamp(rodPosition, 0.0f, 100.0f);

        if (channels[x][y][0] == nullptr) { 
            return -1; 
        }

        float remainingPosition = rodPosition;

        const float section = 100.0f / static_cast<float>(REACTOR_CORE_HEIGHT);

        for (int z = 0; z < REACTOR_CORE_HEIGHT; z++) {
            if (remainingPosition - section >= 0) {
                channels[x][y][z]->controlRodPosition = 100;
                remainingPosition -= section;
            } 
            else {
                channels[x][y][z]->controlRodPosition = remainingPosition / section * 100.0f;
                for (int tz = z + 1; tz < REACTOR_CORE_HEIGHT; tz++) {
                    channels[x][y][tz]->controlRodPosition = 0;
                }
                return 0;
            }
        }

        return 0;
    }

    float getRodPos(unsigned int x, unsigned int y) {

        float rodPosition = 0.0f;
        const float section = 100.0f / static_cast<float>(REACTOR_CORE_HEIGHT);

        for (int z = 0; z < REACTOR_CORE_HEIGHT; z++) {
            rodPosition += channels[x][y][z]->controlRodPosition * (section / 100.0f);
        }

        return rodPosition;
    }

    int redistributeNeutrons(Channel* channel, int x, int y, int z, Channel* channels[REACTOR_CORE_SIZE][REACTOR_CORE_SIZE][REACTOR_CORE_HEIGHT],long double tempMatrix[REACTOR_CORE_SIZE][REACTOR_CORE_SIZE][REACTOR_CORE_HEIGHT]){
        channel->neutrons *= 0.75;
        channel->oldNeutrons *= 0.25;
        
        for (int tx = -1; tx < 2; tx++)
        {
            for (int ty = -1; ty < 2; ty++)
            {
                for (int tz = -1; tz < 2; tz++)
                {
                    if(!(tx == 0 && ty == 0 && tz == 0) && !((tx+x < 0 || tx+x >= REACTOR_CORE_SIZE) || (ty+y < 0 || ty+y >= REACTOR_CORE_SIZE) || (tz+z < 0 || tz+z >= REACTOR_CORE_HEIGHT))){
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
    for (int x = 0; x < REACTOR_CORE_SIZE; x++) {
        for (int y = 0; y < REACTOR_CORE_SIZE; y++) {
            for (int z = 0; z < REACTOR_CORE_HEIGHT; z++) {
                delete channels[x][y][z];
            }
        }
    }
}

};
