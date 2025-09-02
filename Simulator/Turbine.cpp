#include <iostream> // DELETE ME

class Turbine
{
private:

public:
    static const int TURBINE_MAX_FLOW = 1500;
    static const int BYPASS_MAX_FLOW = 350;

    static const int SyncRPM = 5; //5%

    int MAX_ELETRIC_POWER = 1000;

    static const int TurbineLag = 10;

    float RPM = 0;
    float turbineValve = 0.0f; // SET TO 0
    float bypassValve = 0.0f;

    float GeneratorOutput;

    int init(const int maxEletricPower){
        MAX_ELETRIC_POWER = maxEletricPower;
        return 0;
    }

    float update(float delta, float reactorPressure, float condenserPressure){
        if(reactorPressure < 7100000){turbineValve -= 0.1f*delta;} //DELETE ME
        if(reactorPressure > 7100000){turbineValve += 0.1f*delta;} //DELETE ME
        turbineValve = std::max(turbineValve, 0.0f);
        turbineValve = std::min(turbineValve, 100.0f);
        std::cout << turbineValve; // DELETE ME

        float turbineSteam = turbineValve/100.0f * (float)TURBINE_MAX_FLOW * reactorPressure/7100000.0f * delta;

        float GoalRPM = (turbineSteam/delta/(TURBINE_MAX_FLOW*(SyncRPM/100.0f))) * 3600.0f;
        RPM += (GoalRPM - RPM) / TurbineLag*delta;

        GeneratorOutput = std::max(((turbineSteam/delta - TURBINE_MAX_FLOW * (SyncRPM / 100.0f)) / (TURBINE_MAX_FLOW - TURBINE_MAX_FLOW * (SyncRPM / 100.0f))) * MAX_ELETRIC_POWER, 0.0f);
        std::cout << "RPM:"<< RPM << " Gen:"<< GeneratorOutput;

        float bypassSteam = bypassValve/100.0f * (float)BYPASS_MAX_FLOW * reactorPressure/7100000.0f * delta;

        return turbineSteam+bypassSteam;// / condenserPressure/101325.0f;
    }

    Turbine(){}
    ~Turbine(){}
};