#pragma once

class Turbine
{
private:

public:
    static const int TURBINE_MAX_FLOW = 1500;
    static const int BYPASS_MAX_FLOW = 350;

    static const int SyncRPMReactorPower = 5; //5%
    static const int SyncRPM = 3600;

    int MAX_ELETRIC_POWER = 1000;

    static const int TurbineLag = 10;

    float RPM = 0;
    float turbineValve = 5.0f; // SET TO 0
    float bypassValve = 0.0f;

    float GeneratorOutput;

    bool synced = false; 

    double phase = 0;

    int init(const int maxEletricPower){
        MAX_ELETRIC_POWER = maxEletricPower;
        return 0;
    }

    float update(float delta, float reactorPressure, float condenserPressure, float reactorSteamAmount){
        
        if(reactorPressure > 7100000){turbineValve += 0.1f*delta;}
        if(reactorPressure < 7100000){turbineValve -= 0.1f*delta;}

        turbineValve = std::clamp(turbineValve, 0.0f,100.0f);
        bypassValve = std::clamp(bypassValve, 0.0f,100.0f);

        float turbineSteam = turbineValve/100.0f * (float)TURBINE_MAX_FLOW * reactorPressure/7100000.0f * delta;

        if(reactorSteamAmount < turbineSteam){
            turbineSteam = turbineSteam + (reactorSteamAmount-turbineSteam);
        }

        if(!synced){
            float GoalRPM = (turbineSteam/delta/(TURBINE_MAX_FLOW*(SyncRPMReactorPower/100.0f))) * 3600.0f;
            RPM += (GoalRPM - RPM) / TurbineLag*delta;
        } else{RPM = 3600; GeneratorOutput = 0;}

        if(synced){
            GeneratorOutput = std::max(((turbineSteam/delta - TURBINE_MAX_FLOW * (SyncRPMReactorPower / 100.0f)) / (TURBINE_MAX_FLOW - TURBINE_MAX_FLOW * (SyncRPMReactorPower / 100.0f))) * MAX_ELETRIC_POWER, 0.0f);
        }

        float bypassSteam = bypassValve/100.0f * (float)BYPASS_MAX_FLOW * reactorPressure/7100000.0f * delta;

        if(reactorSteamAmount < bypassSteam){ bypassSteam = bypassSteam + (reactorSteamAmount-bypassSteam); }

        //Synchroscope calculations
        phase = std::fmod(phase + (1/delta) * (RPM - SyncRPM), SyncRPM);
        float angle = phase/SyncRPM * 360;
        if(angle < 0){angle += 360;}

        return turbineSteam+bypassSteam;// / condenserPressure/101325.0f;
    }

    Turbine(){}
    ~Turbine(){}
};