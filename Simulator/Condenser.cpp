#pragma once
#include <vector>
#include "CondenserRCPump.cpp"
#include "CAR.cpp"
#include "SJAE.cpp"
#include "PhysicsMath.cpp"
#include "config.hpp"

constexpr double R_universal = 8.314462618;     // J/(mol·K)
constexpr double M_water = 0.01801528;          // kg/mol
constexpr double Rv = R_universal / M_water;    // J/(kg·K) ≈ 461.5

constexpr double M_air = 0.02897;               // kg/mol (dry air)
constexpr double R_air = R_universal / M_air;   // J/(kg·K) ≈ 287

class Condenser
{
private:
    
public:

    double waterAmount = 1000000; //Kg optimal 1000000 
    double steamAmount = 0;
    double airAmount = 0;
    double pressure = 101325;

    float waterTemp = 32.0f;

    std::vector<CondenserRCPump> rcPumps;
    std::vector<CAR> car;
    std::vector<SJAE> sjae;

    int update(float delta, float waterAmountFromTurbine){
        
        //Water temp
        if(waterAmount > waterAmountFromTurbine){
            waterTemp = ((waterAmount-waterAmountFromTurbine)*waterTemp + waterAmountFromTurbine*PhysicsMath::condensationTemperature(pressure)) / waterAmount;
        }
        double T_K = waterTemp + 273.15;

        //Air leeking into condenser
        airAmount += (1.0-pressure/ATMOSPHERIC_PRESSURE)*CONDENSER_AIR_LEKAGE;

        //Water transfer
        float amount;
        for (int i = 0; i < rcPumps.size(); i++)
        {
            amount = rcPumps[i].update(delta);
            steamAmount -= amount;
            waterAmount += amount;
            if(steamAmount < 0){waterAmount += steamAmount; steamAmount = 0;}
        }
        for (int i = 0; i < car.size(); i++)
        {
            amount = car[i].update(delta);
            airAmount -= amount;
            steamAmount -= amount/CAR_RATED_AIR_REMOVAL * 881944444;
            if(steamAmount < 0){steamAmount = 0;}
            if(airAmount < 0){airAmount = 0;}
        }
        for (int i = 0; i < sjae.size(); i++)
        {
            amount = sjae[i].update(delta);
            airAmount -= amount;
            if(airAmount < 0){airAmount = 0;}
        }

        pressure = (steamAmount * Rv * T_K) / (CONDNENSER_VOLUME-waterAmount / 1000.0);

        //AIR
        double Pair = 0.0;
        if(airAmount > 0.0){
            constexpr double R_air = 287.058; // J/(kg·K)
            Pair = (airAmount * R_air * T_K) / (CONDNENSER_VOLUME-waterAmount / 1000.0);
        }

        //Abs Pressure
        pressure = pressure + Pair;

        std::cout << "Water: "<<waterAmount<<" Steam: "<<steamAmount<<" Air: "<<airAmount<<" Pressure: "<<pressure<<"\n";
        
        return 0;
    }

    Condenser(Reactor* reactor){
        for (int i = 0; i < NUM_OF_CONDENSER_RC_PUMP; i++)
        {
            rcPumps.emplace_back();
        }
        for (int i = 0; i < NUM_OF_CONDENSER_CAR; i++)
        {
            car.emplace_back(&pressure);
        }
        for (int i = 0; i < NUM_OF_CONDENSER_SJAE; i++)
        {
            sjae.emplace_back(reactor, &pressure);
        }

        double T_K = waterTemp + 273.15; // assume 32°C initial water temp
        airAmount = (ATMOSPHERIC_PRESSURE * (CONDNENSER_VOLUME - waterAmount / 1000.0)) / (R_air * T_K);
        pressure = ATMOSPHERIC_PRESSURE;
        
    }
};