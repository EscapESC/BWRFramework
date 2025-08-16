#pragma once
#include "Channel.cpp"
#include "PhysicsMath.cpp"
#include <iostream>

class Reactor
{
private:

public:

    static const int MAX_THERMAL_POWER = 3200; //MW
    static const int MAX_ELETRIC_POWER = 1000; //MW

    static const int SIZE = 8;
    static const int HEIGHT = 10;

    Channel* channels[SIZE][SIZE][HEIGHT];
    int changeMetrix[SIZE][SIZE][HEIGHT];

    float waterAmmount = 60000.0f; //Liters
    float steamAmmount = 0;

    float pressure = 1;

    float waterTemperature = 20; //°C

    float avarage_controlRodPosition = 0;//output

    double reactorPeriod;//output

    float avarage_iodine = 0;//output
    float avarage_xenon = 0;//output

    long long int total_neutrons = 0;
    int numberOfChannels = 0;
    int idleNeutrons = 0;

    long long int maxNeutrons = 100000000000;

    long long int oldNeutrons;

    double reactor_power = 0;//output

    Reactor(bool circle = true, long long int maxN = 100000000000, int idleN = 1000, int water = 60000){
        //POPULATE THE REACTOR WITH CHANNELS
        const float center = (SIZE - 1) / 2.0f;
        const float radius = SIZE / 2.0f;
        waterAmmount = water;

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < HEIGHT; z++) {
                    float dx = x - center;
                    float dy = y - center;
                    if(circle){
                        if (dx * dx + dy * dy <= radius * radius) {
                            channels[x][y][z] = new Channel(maxN, idleN);
                            numberOfChannels++;
                        }
                        else {
                            channels[x][y][z] = nullptr;
                        }
                    }
                    else{
                        channels[x][y][z] = new Channel(maxN, idleN);
                        numberOfChannels++;
                    }
                }
            }
        }
        maxNeutrons = maxN * numberOfChannels;
        idleNeutrons = idleN * numberOfChannels;
    }

    void update(float delta){
        oldNeutrons = total_neutrons;
        avarage_controlRodPosition = 0;
        total_neutrons = 0;

        avarage_iodine = 0;
        avarage_xenon = 0;

        //UPDATE CHANNELS
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < HEIGHT; z++) {
                    if(channels[x][y][z] != nullptr){
                        channels[x][y][z]->update(delta);
                        total_neutrons += channels[x][y][z]->neutrons;
                        avarage_controlRodPosition += channels[x][y][z]->controlRodPosition;
                        redistributeNeutrons(channels[x][y][z], x,y,z,channels,changeMetrix);

                        avarage_iodine += channels[x][y][z]->iodine;
                        avarage_xenon += channels[x][y][z]->xenon;
                    }
                }
            }
        }
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < HEIGHT; z++) {
                    if(channels[x][y][z] != nullptr){
                        channels[x][y][z]->neutrons += changeMetrix[x][y][z];
                    }
                }
            }
        }

        //WATER BOILING
        waterTemperature += ((long double)total_neutrons / (long double)maxNeutrons) * 3200000000.0f * delta / (4200.0f * waterAmmount);

        avarage_iodine /= numberOfChannels;
        avarage_xenon /= numberOfChannels;

        avarage_controlRodPosition = avarage_controlRodPosition / numberOfChannels;
        if (oldNeutrons > 0 && total_neutrons > 0) {
            reactorPeriod = delta / log((long double)total_neutrons / (long double)oldNeutrons);
        } else {
            reactorPeriod = INFINITY; // avoid div by zero
        }
        reactor_power = (long double)total_neutrons / (long double)maxNeutrons * 100.0f;
    }

    int moveRod(int x, int y, float rodPosition){

        if(channels[x][y][0] == nullptr){return -1;}

        float decrementedPosition = rodPosition;

        const float section = 100.0f/(float)HEIGHT;

        for(int z = 0; z < HEIGHT; z++){
            if(decrementedPosition - section >= 0){
                channels[x][y][z]->controlRodPosition = 100;
                decrementedPosition -= section;
            }
            else{
                channels[x][y][z]->controlRodPosition = decrementedPosition/section*100.0f;
                for(int tz = z+1; tz < HEIGHT; tz++){
                    channels[x][y][tz]->controlRodPosition = 0;
                }
                return 0;
            }
        }
        return 0;

    }

    int redistributeNeutrons(Channel* channel, int x, int y, int z, Channel* channels[SIZE][SIZE][HEIGHT], int tempMatrix[SIZE][SIZE][HEIGHT]){
        channel->neutrons /= 2;
        
        for (int tx = -1; tx < 2; tx++)
        {
            for (int ty = -1; ty < 2; ty++)
            {
                for (int tz = -1; tz < 2; tz++)
                {
                    if(!(tx == 0 && ty == 0 && tz == 0) && !((tx+x < 0 || tx+x >= SIZE) || (ty+y < 0 || ty+y >= SIZE) || (tz+z < 0 || tz+z >= HEIGHT))){
                        int dist = std::abs(tx)+std::abs(ty)+std::abs(tz);
                        if(dist == 2){
                            if(channels[tx+x][ty+y][tz+z] != nullptr){
                                tempMatrix[tx+x][ty+y][tz+z] += channel->neutrons/24;
                            }
                            else{
                                tempMatrix[x][y][z] += channel->neutrons/48;
                            }
                        }
                        else if(dist == 1){
                            if(channels[tx+x][ty+y][tz+z] != nullptr){
                                tempMatrix[tx+x][ty+y][tz+z] += channel->neutrons/12;
                            }
                            else{
                                tempMatrix[x][y][z] += channel->neutrons/24;
                            }
                        }
                    }
                }    
            }
        }
        return 0;
    }

    ~Reactor() {
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            for (int z = 0; z < HEIGHT; z++) {
                delete channels[x][y][z];
            }
        }
    }
}

};
