#include <vector>
#include "FWPump.cpp"
#include "Reactor.cpp"
#include "Condenser.cpp"
#include "Turbine.cpp"
#include "Deaerator.cpp"
#include "CondensatePumps.cpp"

class CoolantLoop
{
private:
public:

    Reactor *reactor;
    Condenser condenser;
    Turbine turbine;
    Deaerator deaerator;

    std::vector<FWPump> fwPumps;
    std::vector<CondensatePumps> condPumps;

    CoolantLoop(Reactor *reactorPointer,int numOfFWPumps = 2){
        reactor = reactorPointer;
        condenser = Condenser();
        turbine = Turbine();

        for (int i = 0; i < numOfFWPumps; i++)
        { 
            fwPumps.emplace_back();//cool new trick
        }

        for (int i = 0; i < numOfFWPumps; i++)
        { 
            condPumps.emplace_back();
        }
        
    }

    int update(float delta){
        float amount;

        //REACTOR -> TURBINE -> CONDENSER
        amount = turbine.update(delta, reactor->pressure, condenser.pressure);
        reactor->steamAmount -= amount;
        condenser.steamAmount += amount;
        if(reactor->steamAmount < 0){condenser.steamAmount += reactor->steamAmount;reactor->steamAmount = 0;}
        std::cout << "Condnser steam: "<< condenser.steamAmount << " "; //DELETE ME

        condenser.update(delta);

        //CONDENSER -> DEAERATOR
        for (int i = 0; i < condPumps.size(); i++)
        { 
            amount = condPumps[i].update(delta);
            deaerator.waterAmount += amount * deaerator.inletValve/100;
            condenser.waterAmount -= amount * deaerator.inletValve/100;
            if(condenser.waterAmount < 0){deaerator.waterAmount += condenser.waterAmount * deaerator.inletValve/100;condenser.waterAmount = 0;}

        }

        deaerator.update(delta);

        //DEAERATOR -> REACTOR
        for (int i = 0; i < fwPumps.size(); i++)
        { 
            amount = fwPumps[i].update(delta);
            reactor->waterAmount += amount * deaerator.outletValve/100;
            deaerator.waterAmount -= amount * deaerator.outletValve/100;
            if(deaerator.waterAmount < 0){reactor->waterAmount += deaerator.waterAmount * deaerator.outletValve/100;deaerator.waterAmount = 0;}

        }
        return 0;
    }

    ~CoolantLoop(){}
};
