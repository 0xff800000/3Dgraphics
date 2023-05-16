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

#define WIDTH 400
#define HEIGHT 400

using namespace std;


void loop(SDL_Renderer*renderer,Camera cam, World world){
	SDL_Event ev;
	cam.update();
	cam.render();
	SDL_RenderPresent(renderer);

	for(;;){

		//SDL_RenderPresent(renderer);
		//b1.print();
		world.update();
		cam.update();
		cam.render();
		while(SDL_PollEvent(&ev)){
			// SDL_RenderPresent(renderer);
			switch(ev.type){
				case SDL_QUIT:{
					return;
				}
				case SDL_KEYDOWN:{
					switch(ev.key.keysym.sym){
						case SDLK_ESCAPE: return;
						case 'h':{
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
					break;
				}
				default: {break;}
			}
		}
		usleep(10000);
	}
}


int main(int argc, char** argv) {
	// Load world
	World world;
    auto tick_ms_cb = [](void) { return static_cast<int>(SDL_GetTicks()); };
    world.register_get_ticks(tick_ms_cb);
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
	Camera camera(camPos,0,0,renderer,width,height,world);

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
