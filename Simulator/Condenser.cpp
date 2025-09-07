#include <vector>
#include "CondenserRCPump.cpp"
#include "PhysicsMath.cpp"

class Condenser
{
private:
    
public:
    static const int CONDENSER_VOLUME = 7000; //m3 

    float waterAmount = 0; //Kg optimal 1000000 
    float steamAmount = 0;
    float pressure = 101325;

    std::vector<CondenserRCPump> rcPumps;

    int update(float delta){
        float amount;
        for (int i = 0; i < rcPumps.size(); i++)
        {
            amount = rcPumps[i].update(delta);
            steamAmount -= amount;
            waterAmount += amount;
            if(steamAmount < 0){waterAmount += steamAmount; steamAmount = 0;}
        }
        
        return 0;
    }

    Condenser(int numofRCPumps = 2){
        for (int i = 0; i < numofRCPumps; i++)
        {
            rcPumps.emplace_back();
        }
        
    }
    ~Condenser(){}
};