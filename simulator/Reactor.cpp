#pragma once
#include "Channel.cpp"
#include <iostream>

class Reactor
{
private:

public:

    static const int SIZE = 8;
    static const int HEIGHT = 10;

    Channel* channels[SIZE][SIZE][HEIGHT];

    float avarage_controlRodPosition;

    float ReactorPeriod;

    float avarage_iodine;
    float avarage_xenon;

    long long int total_neutrons = 0;
    int numberOfChannels = 0;
    int idleNeutrons = 0;

    long long int maxNeutrons = 100000000000;

    int oldNeutrons;

    double reactor_power = 0;

    Reactor(bool circle = true, long long int maxN = 100000000000, int idleN = 1000){
        //POPULATE THE REACTOR WITH CHANNELS
        const float center = (SIZE - 1) / 2.0f;
        const float radius = SIZE / 2.0f;

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
        total_neutrons = 0;
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < HEIGHT; z++) {
                    if(channels[x][y][z] != nullptr){
                        channels[x][y][z]->update(delta);
                        total_neutrons += channels[x][y][z]->neutrons;
                    }
                }
            }
        }
        reactor_power = (float)total_neutrons / (float)maxNeutrons * 100.0f;
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
