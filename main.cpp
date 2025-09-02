#include <iostream>
#include "Simulator/Unit.cpp"

int main(int argc, char const *argv[])
{
    Unit* unit = new Unit();

    float rodPosition = 100;
    
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            if(unit->reactor->channels[x][y][0] != nullptr){
                std::cout << "|";
            }
            else{
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }

    while (true)
    {
        if(unit->update() == 0){
            std::cout << "Temperature: " << unit->reactor->waterTemperature << "°C | ReactorPower: " << unit->reactor->reactor_power << "% | AvarageControRod: " << unit->reactor->avarage_controlRodPosition << "% | ReactorPeriod: " <<unit->reactor->reactorPeriod  << "s | Xenon: " << unit->reactor->avarage_xenon << "% | Iodine: " << unit->reactor->avarage_iodine << "% | Pressure: "<<unit->reactor->pressure<<"% | wat: "<<unit->reactor->waterAmount<<"L | steam: "<<unit->reactor->steamAmount<<"L\n";
            if ((unit->reactor->reactorPeriod_precise > 40 || unit->reactor->reactorPeriod_precise < 0) && unit->reactor->reactor_power < 5){
                for(int x = 0; x < unit->reactor->SIZE; x++){
                    for(int y = 0; y < unit->reactor->SIZE; y++){
                        unit->reactor->moveRod(x,y,rodPosition);
                    }
                }
                rodPosition -= 0.001;
            }
            if ((unit->reactor->reactorPeriod_precise < 30 && unit->reactor->reactorPeriod_precise > 0) || unit->reactor->reactor_power > 5){
                for(int x = 0; x < unit->reactor->SIZE; x++){
                    for(int y = 0; y < unit->reactor->SIZE; y++){
                        unit->reactor->moveRod(x,y,rodPosition);
                    }
                }
                rodPosition += 0.001;
            }
        }
        if(rodPosition > 100){rodPosition = 100;}
        if(rodPosition < 0){rodPosition = 0;}
    }
    
    delete unit;

    return 0;
}
