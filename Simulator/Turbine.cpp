#include <iostream> // DELETE ME

class Turbine
{
private:

public:
    static const int TURBINE_MAX_FLOW = 1500;
    static const int BYPASS_MAX_FLOW = 350;

    float turbineValve = 0.0f; // SET TO 0
    float bypassValve = 100.0f;

    float update(float delta, float reactorPressure, float condenserPressure){
        if(reactorPressure < 7100000){bypassValve -= 0.1f*delta;} //DELETE ME
        if(reactorPressure > 7100000){bypassValve += 0.1f*delta;} //DELETE ME
        bypassValve = std::max(bypassValve, 0.0f);
        bypassValve = std::min(bypassValve, 100.0f);
        std::cout << bypassValve; // DELETE ME

        float turbineSteam = turbineValve/100.0f * (float)TURBINE_MAX_FLOW * reactorPressure/7100000.0f * delta;
        float bypassSteam = bypassValve/100.0f * (float)BYPASS_MAX_FLOW * reactorPressure/7100000.0f * delta;

        return turbineSteam+bypassSteam;// / condenserPressure/101325.0f;
    }

    Turbine(){}
    ~Turbine(){}
};