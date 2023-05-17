//============================================================================
// Name        : vectorGraphics.cpp
// Author      : D01000100
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <ctime>

#include <unistd.h>
#include <sys/time.h>

#include <SDL2/SDL.h>

#include "SDL2_gfxPrimitives.h"
#include "point.hpp"
#include "mesh.hpp"
#include "world.hpp"
#include "camera.hpp"
#include "screen_sdl.hpp"

#define WIDTH 400
#define HEIGHT 400

using namespace std;


void loop(SDL_Renderer*renderer,Camera cam, World world){
    SDL_Event ev;
    cam.update();
    cam.render();
    SDL_RenderPresent(renderer);

    int timer = 0;

    for(;;){

        //SDL_RenderPresent(renderer);
        //b1.print();
        world.update();
        cam.update();
        cam.render();

        int ticks = SDL_GetTicks();
        int timePassed = ticks - timer;
        timer = ticks;
        float dist = timePassed/100.0;

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        //Camera movement
        if(state[SDL_SCANCODE_W]){cam.move_x(dist); }
        if(state[SDL_SCANCODE_S]){cam.move_x(-dist);}
        if(state[SDL_SCANCODE_D]){cam.move_y(dist); }
        if(state[SDL_SCANCODE_A]){cam.move_y(-dist);}
        if(state[SDL_SCANCODE_Q]){cam.move_z(dist); }
        if(state[SDL_SCANCODE_E]){cam.move_z(-dist);}
        //Camera rotation
        if(state[SDL_SCANCODE_RIGHT]){cam.rot_yaw(dist/2.0); }
        if(state[SDL_SCANCODE_LEFT]) {cam.rot_yaw(-dist/2.0);}
        if(state[SDL_SCANCODE_UP])   {cam.rot_pitch(dist/2.0); }
        if(state[SDL_SCANCODE_DOWN]) {cam.rot_pitch(-dist/2.0);}

        while(SDL_PollEvent(&ev)){
            // SDL_RenderPresent(renderer);

            if(ev.type == SDL_QUIT)
                return;
            else if(ev.type == SDL_KEYDOWN) {
                switch(ev.key.keysym.sym)
                {
                    case SDLK_ESCAPE: return;
                    case 'h':
                                      {
                                          timespec t1,t2;
                                          clock_gettime(CLOCK_REALTIME, &t1);
                                          cam.render();
                                          clock_gettime(CLOCK_REALTIME, &t2);
                                          cout << t2.tv_sec-t1.tv_sec << " sec ";
                                          cout << t2.tv_nsec-t1.tv_nsec << " nsec" << endl;
                                          break;
                                      }
                    // Change rendering modes
                    case 'y':cam.wireModeToggle();break;
                    case 'x':cam.dotModeToggle();break;
                    case 'c':cam.polyModeToggle();break;
                    case 'v':cam.snakeModeToggle();break;
                    case 'b':cam.meshBoxToggle();break;
                    case 'm':cam.acidShaderToggle();break;


                    // Change FOV
                    case 'k':cam.FOVinc();break;
                    case 'l':cam.FOVdec();break;
                    default:{
                                break;
                            }
                }
            }
            else
                break;
        }
        usleep(10000);
    }
}


int main(int argc, char** argv) {
    // Load world
    World world;
    auto world_tick_ms_cb = [](void) { return static_cast<int>(SDL_GetTicks()); };
    world.register_get_ticks(world_tick_ms_cb);
    string path;
    float divider=0;
    cout << argv[1]<<endl;
    if(argc <= 1){
        path="ape2.obj";
    }
    else{
        path=argv[1];
    }
    if(argc == 3)divider=atof(argv[2]);
    //world.importMesh(path,0.001);
    if(divider){
        world.importMesh(path,1.0/divider);
    }
    else{
        world.importMesh(path);
    }
    world.printDebug();

    const int width=WIDTH,height=HEIGHT;
    SDL_Window* window = SDL_CreateWindow
        (
         "Vector Graphics", SDL_WINDOWPOS_UNDEFINED,
         SDL_WINDOWPOS_UNDEFINED,
         width,
         height,
         SDL_WINDOW_SHOWN
        );
    SDL_Renderer*renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    // Create camera
    Point camPos(0,0,-5);
    ScreenSDL screen(renderer,width,height);
    Camera camera(camPos,0,0,screen,width,height,world);
    auto cam_tick_ms_cb = [](void) { return static_cast<int>(SDL_GetTicks()); };
    camera.register_get_time_ms(cam_tick_ms_cb);

    //Black screen
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    loop(renderer,camera,world);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
