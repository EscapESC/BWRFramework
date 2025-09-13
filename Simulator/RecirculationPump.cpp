#pragma once

class RecirculationPump
{
private:

public:

    bool power = true;

    static const int MAX_RPM = 5500;
    static const int MAX_THRUGHPUT = 900; //kg/s
    float RPM = 1100.0f; // SET TO 0

    float intakeValve = 0.0f;
    float outtakeValve = 0.0f;

    bool pumpCavitation = false;

    int numOfRcPump;

    float update(double reactorPower){
        if(reactorPower < 25 && RPM/MAX_RPM > 0.35){
            pumpCavitation = true;
        }

        if(!power){RPM = 0;}

        return RPM/MAX_RPM*intakeValve/100*outtakeValve/100;
    }

    RecirculationPump(int numOfRecirculattionPumps){numOfRcPump = numOfRecirculattionPumps;}
    ~RecirculationPump(){}
};