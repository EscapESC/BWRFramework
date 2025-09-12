#pragma once

class CondensatePump
{
private:
    /* data */
public:
    static const int MAX_RPM = 5500;
    static const int MAX_THRUGHPUT = 800; //kg/s
    float RPM = 5500.0f; // SET TO 0

    float intakeValve = 100.0f;
    float outtakeValve = 100.0f;

    float update(float delta){
        float waterAmount = 0;
        if(RPM > 0){
            waterAmount = RPM/MAX_RPM * MAX_THRUGHPUT * intakeValve/100.0f * outtakeValve/100.0f * delta;
        }
        return waterAmount;
    }

    CondensatePump(/* args */){}
    ~CondensatePump(){}
};
