#include "include/raylib.h"
#include "Simulator/Unit.cpp"
#include <chrono>

#include "Simulator/config.hpp"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

constexpr int TILE_SIZE = 54;

Unit* unit;

static const float MAX_FPS = 60.0f;

std::chrono::_V2::system_clock::time_point lastTick;

uint32_t heatColorIndicator(double* powerLevel){

    *powerLevel = std::clamp(*powerLevel,0.0,1.0);

    uint8_t R = (int8_t)(*powerLevel * 255.0);
    uint8_t G = 0;
    uint8_t B = (1.0-*powerLevel) * 255.0;
    uint8_t A = 255;

    if(*powerLevel > 1.0){R = 255; B = 0;}

    return ((uint32_t)R << 24) | ((uint32_t)G << 16) | ((uint32_t)B << 8) | ((uint32_t)A);
}

int main()
{

    unit = new Unit();

    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(800, 600, "BWR4k");
    MaximizeWindow();
    SetTargetFPS(60); // Must be greater (or 0) then the maximum TPS in the Simulator since raylib stops the main thread that also updates the simulation.

    bool showMessageBox = false;

    GuiLoadStyle("style_dark.rgs");

    std::cout << "INFO: RAYLIB sucessfully loaded\n";
    std::cout << "INFO: BWR4k sucessfully loaaded\b";
    
    bool rodSelect[REACTOR_CORE_SIZE][REACTOR_CORE_SIZE];
    std::memset(rodSelect,false, sizeof(rodSelect));

    int drawState = 1;
    int drawSpeed = 0;
    int sel = 1;
    int Zindex = 0;
    int modSel = 0;
    int IPRsel = 0;

    lastTick = std::chrono::high_resolution_clock::now();

    while (!WindowShouldClose())
    {

        unit->update();

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        GuiToggleGroup((Rectangle){TILE_SIZE*2-2,TILE_SIZE*9,TILE_SIZE*2,TILE_SIZE},"Group;Rods", &sel);
        GuiToggleGroup((Rectangle){TILE_SIZE*2-2,TILE_SIZE*10,TILE_SIZE*4/3,TILE_SIZE},"INS.;N;WIDTH.", &drawState);
        GuiToggleGroup((Rectangle){TILE_SIZE*2-2,TILE_SIZE*11,TILE_SIZE*4/3,TILE_SIZE},"Slow;Med.;Fast", &drawSpeed);
        GuiToggleGroup((Rectangle){TILE_SIZE*2-2,TILE_SIZE*13,TILE_SIZE*4/3,TILE_SIZE},"SDR;IPR;RUN", &modSel);

        GuiDrawText(TextFormat("Reactor period: %lf\nReactor period:%f",unit->reactor->reactorPeriod_precise,unit->reactor->reactorPeriod),(Rectangle){TILE_SIZE*6,0,TILE_SIZE*8,TILE_SIZE},TEXT_ALIGN_LEFT, WHITE);
        GuiDrawText(TextFormat("Reactor power: %lf",unit->reactor->reactor_power),(Rectangle){TILE_SIZE*2,TILE_SIZE*8,TILE_SIZE*4,TILE_SIZE},TEXT_ALIGN_CENTER,WHITE);
        GuiSpinner((Rectangle){TILE_SIZE*7,TILE_SIZE*9,TILE_SIZE*2,TILE_SIZE},"SRM",&Zindex,0,9,false);

        

        GuiDrawText(TextFormat("Avg. rod: %.2f%%",unit->reactor->avarage_controlRodPosition),(Rectangle){0,TILE_SIZE*8,TILE_SIZE*3,TILE_SIZE},TEXT_ALIGN_LEFT, WHITE);

        GuiSpinner((Rectangle){TILE_SIZE*7,TILE_SIZE*10,TILE_SIZE*2,TILE_SIZE},"IPR",&IPRsel,0,6,false);

        auto tempColor = GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL);

        for(int x = 0; x < REACTOR_CORE_SIZE; x++){
            for(int y = 0; y < REACTOR_CORE_SIZE; y++){
                if(unit->reactor->channels[x][y][0]){
                    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, heatColorIndicator(&unit->reactor->channels[x][y][Zindex]->power));
                    GuiToggle((Rectangle){(float)x*54,(float)y*54,54,54},TextFormat("%.2f%%\n%.2f%%",unit->reactor->getRodPos(x,y),unit->reactor->channels[x][y][Zindex]->power*100.0f),&rodSelect[x][y]);
                    if((sel == 0 || rodSelect[x][y]) && drawState == 0){
                        unit->reactor->moveRod(x,y,unit->reactor->getRodPos(x,y)+(-0.01f-0.2f*(float)drawSpeed)*GetFrameTime());
                    }
                    else if((sel == 0 || rodSelect[x][y]) && drawState == 2){
                        unit->reactor->moveRod(x,y,unit->reactor->getRodPos(x,y)+(+0.01f+0.2f*(float)drawSpeed)*GetFrameTime());
                    }
                }
            }
        }

        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, tempColor);

        if(GuiButton((Rectangle){432-54,432-54,54,54},"Clr.")){std::memset(rodSelect,0,sizeof(rodSelect));}

        GuiDrawText(TextFormat("WTemp: %.2Lf°C\nPres.: %.0LfPa",unit->reactor->waterTemperature,unit->reactor->pressure),(Rectangle){0,0,54*2,54},TEXT_ALIGN_LEFT,WHITE);

        EndDrawing();
    }

    CloseWindow();

    delete unit;

    return 0;
}