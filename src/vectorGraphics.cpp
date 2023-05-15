//============================================================================
// Name        : vectorGraphics.cpp
// Author      : D01000100
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
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

#define WIDTH 400
#define HEIGHT 400

using namespace std;

//###################################### World Class #######################################################


class World{
private:
		int timer;
public:
		World(){timer=0;};
	vector<Mesh> meshes;
	void importMesh(string&,float);
	void printDebug();
	void update();
	void initMesh();
	vector<Mesh> getMeshes(){return meshes;};
	vector<Point> vertex;
	vector<polygon> face;
};

void World::importMesh(string&path,float scale=1.0){
	ifstream objFile;
	objFile.open(&path[0]);
	string line;
	if(objFile.is_open()){
		while(getline(objFile,line)){
			char str[10];
			float x,y,z;
			if(sscanf(&line[0],"o %s",str)){
				// New object definition
				meshes.push_back(Mesh());
				meshes.back().name = str;

			}
			else if(sscanf(&line[0],"%s %f %f %f",str,&x,&y,&z)){
				if(strcmp(str,"v")==0){
					// Vertex definition
					//printf("Vertex : %f, %f, %f\n",x,y,z);
					Point pt(x,y,z);
					pt.scale(scale);
					vertex.push_back(pt);
					if(meshes.size()==0){
							meshes.push_back(Mesh());
							meshes.back().name = "Unknown mesh";
					}
					meshes.back().vertex.push_back(pt);
				}
				else if(strcmp(str,"f")==0){
					// Face definition
					polygon poly;
					char delim[] = "//";
					char*token;
					int vertexIndex;
					token = strtok(&line[0], delim);
					while(token != NULL){
						if(sscanf(token,"%*s %d",&vertexIndex)){
							poly.edges.push_back(vertexIndex-1);
						}
						else break;
						token = strtok(NULL, delim);

					}
					//cout << endl << endl;
					poly.edges.push_back(poly.edges[0]);
					/*
					cout << "Line : "<<line<<endl;
					for(unsigned i=0; i<poly.edges.size(); i++)printf("%d ,",poly.edges[i]);
					cout << endl << endl;
					*/
					poly.color[0] = rand();
					poly.color[1] = rand();
					poly.color[2] = rand();
					//cout<<poly.color<<endl;
					face.push_back(poly);
					meshes.back().face.push_back(poly);
				}
			}
		}
	}
	else {
		cout << "Error while opening " << path << endl;
	}

	initMesh();
	objFile.close();
}

void World::initMesh(){
	for(Mesh& m : meshes){
		m.computeBox();
		m.zeroEdge();
		m.printDebug();
	}
}

void World::printDebug(){
	printf("The world contains %d vertexes forming %d faces.\n",(int)vertex.size(),(int)face.size());
	// cout << "Vertex list:" << endl;
	// for(unsigned int i=0;i<vertex.size(); i++){
	// 	printf("%i : %f,%f,%f\n",i,vertex[i].x,vertex[i].y,vertex[i].z);
	// }
	// cout << "\n\nFace list:" << endl;
	// for(unsigned int i=0;i<face.size(); i++){
	// 	cout << i << " : ";
	// 	for(unsigned int j=0; j<face[i].edges.size(); j++){
	// 		cout<<face[i].edges[j]<<",";
	// 	}
	// 	cout<<endl;
	// }

	printf("The world contains %d objects.\n",(int)meshes.size());
	cout << "Object list :" << endl;
	for(Mesh m : meshes){
		cout << m.name << ":v="<<m.vertex.size()<<endl;
		for(auto pt : m.getBox())
			cout<<pt.x<<":"<<pt.y<<":"<<pt.z << endl;

	}
	//cout<<meshes[0].box[0].x<<meshes[0].box[0].y<<meshes[0].box[0].z<<endl;

}

void World::update(){
	if(meshes.size()>=4){
		int ticks = SDL_GetTicks();
		int timePassed = ticks - timer;
		timer = ticks;
		meshes[0].rotY(timePassed*0.01);
		meshes[0].rotX(timePassed*0.01);
		meshes[1].rotY(timePassed*0.02);
		meshes[2].rotZ(timePassed*0.03);
		Point mov = Point(0.1*sin(timePassed*timer/1000.0),0,0);
		meshes[3].move(mov);
	}
}

//#############################################################################################


//######################################## Camera Class #####################################################

class Camera{
private:
	Point position;
	float rotation[2];
	SDL_Renderer*renderer;
	World world;
	bool dotMode;
	bool wireMode;
	bool polyMode;
	bool snakeMode;
	bool meshBox;
	bool acidShader;
	float cx,cy;
	int resX,resY;
	int facesToRender;
	int fovCoef;
	int timer;
	bool rendering;

	void getScreenCoord(Point&,float*,float*,float*);
	void rotate2D(float,float,float,float*,float*);
public:
	Camera(Point&,int,int,SDL_Renderer*,int,int,World);
	void update();
	void render();
	void wireModeToggle(){wireMode = !wireMode;};
	void dotModeToggle(){dotMode = !dotMode;};
	void polyModeToggle(){polyMode = !polyMode;};
	void snakeModeToggle(){snakeMode = !snakeMode;};
	void meshBoxToggle(){meshBox = !meshBox;};
	void acidShaderToggle(){acidShader = !acidShader;};
	void FOVinc(){fovCoef++;};
	void FOVdec(){(fovCoef>0)?fovCoef--:0;};
	Point getPos();
};

Camera::Camera(Point&pos,int rot1,int rot2,SDL_Renderer*surface,int width,int height,World w){
	position = pos;
	rotation[0] = rot1; rotation[1] = rot2;
	renderer = surface;
	world = w;
	world.initMesh();
	dotMode = false;
	wireMode = true;
	polyMode = true;
	snakeMode = false;
	meshBox = false;
	acidShader = false;
	resX = width; resY = height;
	cx = width/2;
	cy = height/2;
	fovCoef = 200;
	cout << cx << " " << cy << endl;
	facesToRender = 2;
	timer = 0;
	rendering = true;
}

void Camera::update(){
	int ticks = SDL_GetTicks();
	int timePassed = ticks - timer;
	timer = ticks;
	float dist = timePassed/100.0;
	float x = dist*sin(rotation[0]), z = dist*cos(rotation[0]);

	rendering = true;
	// Keys
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	//Camera movement
	if(state[SDL_SCANCODE_W]){position.x += x;position.z += z;rendering = true;}
	if(state[SDL_SCANCODE_S]){position.x -= x;position.z -= z;rendering = true;}
	if(state[SDL_SCANCODE_D]){position.x += z;position.z -= x;rendering = true;}
	if(state[SDL_SCANCODE_A]){position.x -= z;position.z += x;rendering = true;}
	if(state[SDL_SCANCODE_Q]){position.y += dist;rendering = true;}
	if(state[SDL_SCANCODE_E]){position.y -= dist;rendering = true;}
	//Camera rotation
	if(state[SDL_SCANCODE_RIGHT]){rotation[0]+=dist/2.0;rendering = true;}
	if(state[SDL_SCANCODE_LEFT]){rotation[0]-=dist/2.0;rendering = true;}
	if(state[SDL_SCANCODE_UP]){rotation[1]+=dist/2.0;rendering = true;}
	if(state[SDL_SCANCODE_DOWN]){rotation[1]-=dist/2.0;rendering = true;}
}

void Camera::rotate2D(float posX, float posY, float angle,float*xo,float*yo){
	float c = cos(angle), s = sin(angle);
	*xo = posX*c - posY*s;
	*yo = posX*s + posY*c;
}

void Camera::getScreenCoord(Point&vertex,float*screenX,float*screenY,float*depth){
	float x = vertex.x, y = vertex.y, z = vertex.z;
	x += position.x;
	y += position.y;
	z += position.z;
	rotate2D(x,z,rotation[0],&x,&z);
	rotate2D(y,z,rotation[1],&y,&z);
	float f = fovCoef / z;
	x *= f;
	y *= f;
	*screenX = x + cx;
	*screenY = y + cy;
	*depth = z;
}

Point Camera::getPos(){
	return position;
}

struct zBuffer{
	vector<Sint16> x;
	vector<Sint16> y;
	float z;
	int r,g,b;
	bool operator<(const zBuffer & rhs) const
	{
		return z < rhs.z;
	}
};

struct zBufferMesh{
	Mesh mesh;
	float z;
	bool operator<(const zBufferMesh & rhs) const
	{
		return z < rhs.z;
	}
};

void Camera::render(){
	if(!rendering)return;
	rendering = false;
	//cout<<"@@@@@@@ Rendering @@@@@@@@"<<endl;
	// Clear screen
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	// Get meshes in camera view
	vector<Mesh> mesh;
	for(Mesh m : world.getMeshes()){
		float x,y,z;
		//cout << "Check the objects\n";
		vector<Point> v = m.getBox();
		//cout << v.size() << endl;
		for(Point& pt : m.getBox()){
			//m.printDebug();
			getScreenCoord(pt,&x,&y,&z);
			//cout << x << ":"<<y<<endl;
			if(x>=0&&x<=resX&&y>=0&&y<=resY&&z<0){
				mesh.push_back(m);
				//cout << m.name << " is valid" << endl;
				break;
			}
		}
	}

	// Sort meshes according to the distance from the camera
	vector<Mesh> orderedMesh;
	vector<zBufferMesh> buffer;
	for(Mesh m : mesh){
		zBufferMesh buf;
		buf.mesh = m;
		float maxZ = -std::numeric_limits<float>::max();
		for(Point pt : m.getBox()){
			float x,y,z;
			getScreenCoord(pt,&x,&y,&z);
			if(z>maxZ)maxZ=z;
		}
		buf.z = maxZ;
		buffer.push_back(buf);
	}
	sort(buffer.begin(),buffer.end());
	for(zBufferMesh&m : buffer){
		orderedMesh.push_back(m.mesh);
	}
	mesh = orderedMesh;


	for(Mesh&m : mesh){
		SDL_SetRenderDrawColor(renderer,0,0,0,0);
		// Compute screen coords for each vertex in the world
		vector<Point> screenCoords;
		for(Point&pt : m.vertex){
			float x,y,z;
			bool validity=false;
			getScreenCoord(pt,&x,&y,&z);
			// Clip points
			if((z<0.0))validity=true;
			Point new_pt = Point(x,y,z,validity);
			
			if(acidShader){
				struct timeval tp;
				gettimeofday(&tp, NULL);
				static long int ms = 0;
		   		if(tp.tv_usec%2){
						ms++;
				}

				new_pt -= Point(resX/2,resY/2,0);
				//new_pt.rotX(sin(z/100.0*(float)ms/30000.0));
				//new_pt.rotY(0.5*sin(z/100.0*(float)ms/30000.0));
				//new_pt.rotZ(0.5*sin(z/300.0*(float)ms/30000.0));
				new_pt.rotZ(0.5*sin((z/300.0)*((float)ms/10000.0)));
				new_pt += Point(resX/2,resY/2,0);
			}

			screenCoords.push_back(new_pt);
		}

		// Render points
		if(dotMode){
			for(Point&pt : screenCoords){
				if(pt.valid){
					SDL_RenderDrawPoint(renderer,(int)pt.x,(int)pt.y);
					//filledCircleRGBA(renderer,(int)x,(int)y,2,0x00,0x00,0x00,0xff);
				}
			}
		}

		// Render snake
		if(snakeMode){
			for(unsigned i=1; i<screenCoords.size(); i++){
				if(screenCoords[i].valid && screenCoords[i-1].valid){
					SDL_RenderDrawLine(renderer,(int)screenCoords[i].x,(int)screenCoords[i].y,(int)screenCoords[i-1].x,(int)screenCoords[i-1].y);
					//thickLineRGBA(renderer,(int)screenCoords[i].x,(int)screenCoords[i].y,(int)screenCoords[i-1].x,(int)screenCoords[i-1].y,3,0,0,0,0xff);
				}
			}
		}

		// Render wireframe
		if(wireMode){
			int line_drawn[screenCoords.size()] = {0};
			for(unsigned f=0; f<m.face.size(); f++){
				for(unsigned v=1; v<m.face[f].edges.size(); v++){
					if(screenCoords[m.face[f].edges[v]].valid && screenCoords[m.face[f].edges[v-1]].valid){
						int vertex1 = m.face[f].edges[v];
						int vertex2 = m.face[f].edges[v-1];
						if(line_drawn[vertex1] == 0 || line_drawn[vertex2] == 0 || line_drawn[vertex1] == f || line_drawn[vertex2] == f){
							   line_drawn[vertex1] = f;
							   line_drawn[vertex2] = f;
						}
						else{
								break;
						}
						SDL_RenderDrawLine(renderer,(int)screenCoords[vertex1].x,(int)screenCoords[vertex1].y,(int)screenCoords[vertex2].x,(int)screenCoords[vertex2].y);
					}
				}
			}
		}

		// Render mesh box
		if(meshBox){
			SDL_SetRenderDrawColor(renderer,0xff,0,0,0);
			vector<Point> boxEdge;
			for(Point pt : m.getBox()){
				float x,y,z;
				bool validity = false;
				getScreenCoord(pt,&x,&y,&z);
				if(z<0)validity = true;
				boxEdge.push_back(Point(x,y,z,validity));
			}
			for(int i = 0; i<12; i++){
				if(boxEdge[boxIndexes[i][0]].valid && boxEdge[boxIndexes[i][1]].valid){
					SDL_RenderDrawLine(renderer,(int)boxEdge[boxIndexes[i][0]].x,(int)boxEdge[boxIndexes[i][0]].y,(int)boxEdge[boxIndexes[i][1]].x,(int)boxEdge[boxIndexes[i][1]].y);
				}
			}
		}

		if(polyMode){
			vector<zBuffer> faceList;
			// Create and sort faceList
			for(unsigned f=0; f<m.face.size(); f++){
				zBuffer currentFace;
				vector<Sint16> x;
				vector<Sint16> y;
				vector<float> zbuff;
				for(unsigned v=0; v<m.face[f].edges.size(); v++){
					x.push_back((int)screenCoords[m.face[f].edges[v]].x);
					y.push_back((int)screenCoords[m.face[f].edges[v]].y);
					zbuff.push_back(screenCoords[m.face[f].edges[v]].z);
				}
				currentFace.x = x;
				currentFace.y = y;
				currentFace.z = *min_element(&zbuff[0],&zbuff[zbuff.size()]);

				// Get face color
				//world.getColor(f,&currentFace.r,&currentFace.g,&currentFace.b);
				currentFace.r = (Sint16)(m.face[f].color[0]);
				currentFace.g = (Sint16)(m.face[f].color[1]);
				currentFace.b = (Sint16)(m.face[f].color[2]);

				// Check validity : if one of the edges is out the screen => false
				bool valid = true;
				if(*(min_element(&x[0],&x[x.size()])) < 0)valid=false;
				else if(*(max_element(&x[0],&x[x.size()]))>resX)valid=false;
				else if(*(min_element(&y[0],&y[y.size()])) < 0)valid=false;
				else if(*(max_element(&y[0],&y[y.size()]))>resY)valid=false;


				if(currentFace.z<0 && valid)faceList.push_back(currentFace);
				//else cout<<"Polygon reject"<<endl;
			}
			sort(&faceList[0],&faceList[faceList.size()]);

			// Display faces
			for(unsigned f=0; f<faceList.size(); f++){
				filledPolygonRGBA(renderer,&faceList[f].x[0],&faceList[f].y[0],faceList[f].x.size(),faceList[f].r*2,faceList[f].g/2,faceList[f].b,0xff);
				//filledPolygonRGBA(renderer,&faceList[f].x[0],&faceList[f].y[0],faceList[f].x.size(),rand(),rand(),rand(),0xff);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

//#############################################################################################

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
