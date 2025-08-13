#include <iostream>
#include "Simulator/Unit.cpp"

int main(int argc, char const *argv[])
{
    Unit* unit = new Unit();
    
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
            std::cout << "Temperature: " << unit->reactor->waterTemperature << "°C | ReactorPower: " << unit->reactor->reactor_power << "% | AvarageControRod: " << unit->reactor->avarage_controlRodPosition << "% | ReactorPeriod: " <<unit->reactor->reactorPeriod  << "s\n";
        }
    }
    
    delete unit;

    return 0;
}
