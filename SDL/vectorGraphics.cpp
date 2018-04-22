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

#include <unistd.h>
#include <sys/time.h>

#include <SDL2/SDL.h>

#include "SDL2_gfxPrimitives.h"

#define WIDTH 400
#define HEIGHT 400

using namespace std;

//###################################### Vector Class #######################################################

class Point{
public:
	float x;
	float y;
	float z;

	bool valid;

	void rotX(float);
	void rotY(float);
	void rotZ(float);
	void scale(float);

	void print();
	void assign(float,float,float);
	bool disp2D();

	Point();
	Point(float,float,float,bool);
};

void Point::rotX(float rad){
	float tY=this->y;float tZ=this->z;
	this->y=cos(rad)*tY-sin(rad)*tZ;
	this->z=sin(rad)*tY+cos(rad)*tZ;
}

void Point::rotY(float rad){
	float tX=this->x;float tZ=this->z;
	this->x=cos(rad)*tX+sin(rad)*tZ;
	this->z=-sin(rad)*tX+cos(rad)*tZ;
}

void Point::rotZ(float rad){
	float tX=this->x;float tY=this->y;
	this->x=cos(rad)*tX-sin(rad)*tY;
	this->y=sin(rad)*tX+cos(rad)*tY;
}

void Point::scale(float coef){
	this->x*=coef;
	this->y*=coef;
	this->z*=coef;
}

void Point::print(){
	cout<<"("<<this->x<<","<<this->y<<","<<this->z<<")"<<endl;
}

void Point::assign(float xV,float yV,float zV){
	this->x=xV;this->y=yV;this->z=zV;
}

bool Point::disp2D(){
	return (this->x>=0&&this->y>=0)?true:false;
}

Point::Point(){
	this->x=0.0;this->y=0.0;this->z=0.0;valid = false;
}

Point::Point(float xVal,float yVal,float zVal, bool v=false){
	this->x=xVal;this->y=yVal;this->z=zVal;valid = v;
}

//###################################### Mesh Class #######################################################
typedef struct{
	vector<int> edges;
	unsigned color[3];
}polygon;

class Mesh {
public:
	string name;
	Point pos;
	vector<Point> vertex;
	vector<polygon> face;
	Mesh();
	vector<Point>& getBox(){return box;};
	virtual ~Mesh (){};
	void computeBox();
	void printDebug();
	void zeroEdge();
private:
	vector<Point> box;
};

Mesh::Mesh(){
	pos.x=0;
	pos.y=0;
	pos.z=0;
	//box.push_back(8);
	for(int i=0; i<8; i++){
		box.push_back(Point(0,0,0));
	}
};

void Mesh::computeBox(){
	cout<<"initMesh " <<name<< endl;
	// Find Min and Max of X,Y,Z
	float x_min=std::numeric_limits<float>::max(),y_min=std::numeric_limits<float>::max(),z_min=std::numeric_limits<float>::max();
	float x_max=-std::numeric_limits<float>::max(),y_max=-std::numeric_limits<float>::max(),z_max=-std::numeric_limits<float>::max();
	for(Point v : vertex){
		if(x_min>v.x)x_min=v.x;
		if(y_min>v.y)y_min=v.y;
		if(z_min>v.z)z_min=v.z;
		if(x_max<v.x)x_max=v.x;
		if(y_max<v.y)y_max=v.y;
		if(z_max<v.z)z_max=v.z;
	}
	//cout <<"minmax:"<< x_min<<":" << x_max<<endl;

	// Create box
	// box.push_back(Point(x_min,y_min,z_min));
	// box.push_back(Point(x_max,y_min,z_min));
	// box.push_back(Point(x_min,y_min,z_max));
	// box.push_back(Point(x_max,y_min,z_max));
	// box.push_back(Point(x_min,y_max,z_min));
	// box.push_back(Point(x_max,y_max,z_min));
	// box.push_back(Point(x_min,y_max,z_max));
	// box.push_back(Point(x_max,y_max,z_max));

	// box[0] = Point(x_min,y_min,z_min);
	// box[1] = Point(x_max,y_min,z_min);
	// box[2] = Point(x_min,y_min,z_max);
	// box[3] = Point(x_max,y_min,z_max);
	// box[4] = Point(x_min,y_max,z_min);
	// box[5] = Point(x_max,y_max,z_min);
	// box[6] = Point(x_min,y_max,z_max);
	// box[7] = Point(x_max,y_max,z_max);

	box[0].assign(x_min,y_min,z_min);
	box[1].assign(x_max,y_min,z_min);
	box[2].assign(x_min,y_min,z_max);
	box[3].assign(x_max,y_min,z_max);
	box[4].assign(x_min,y_max,z_min);
	box[5].assign(x_max,y_max,z_min);
	box[6].assign(x_min,y_max,z_max);
	box[7].assign(x_max,y_max,z_max);

	// Create box
	// box[0].x=x_min;box[0].y=y_min;box[0].z=z_min;
	// box[1].x=x_max;box[1].y=y_min;box[1].z=z_min;
	// box[2].x=x_min;box[2].y=y_min;box[2].z=z_max;
	// box[3].x=x_max;box[3].y=y_min;box[3].z=z_max;
	// box[4].x=x_min;box[4].y=y_max;box[4].z=z_min;
	// box[5].x=x_max;box[5].y=y_max;box[5].z=z_min;
	// box[6].x=x_min;box[6].y=y_max;box[6].z=z_max;
	// box[7].x=x_max;box[7].y=y_max;box[7].z=z_max;
	// for(Point pt : box)
	// cout<<pt.x<<":"<<pt.y<<":"<<pt.z << endl;
}

void Mesh::zeroEdge(){
	// Find minimum index
	int minI = 0x7fffffff;
	for(polygon f : face){
		for(int i : f.edges){
			if(i<minI) minI = i;
		}
	}
	//minI--;
	//cout<<"Minimum :"<<minI<<endl;
	// Substract the index
	for(polygon&f : face){
		for(int&i : f.edges){
			i -= minI;
			//cout<<i<<endl;
			//if(i>vertex.size())cout<<"error"<<endl;
		}
	}
	//cout<<"first:"<<face[0].edges[0]<<endl;
}

void Mesh::printDebug(){
	cout << "Object name : " << name << endl;
	for(Point pt : box){
		cout << pt.x << ":" << pt.y << ":" << pt.z << endl;
	}
}

//###################################### World Class #######################################################


class World{
private:
	vector<Mesh> meshes;
public:
	void importMesh(string&,float);
	void printDebug();
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
	printf("The world contains %d vertexes forming %d faces.\n",vertex.size(),face.size());
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

	printf("The world contains %d objects.\n",meshes.size());
	cout << "Object list :" << endl;
	for(Mesh m : meshes){
		cout << m.name << ":v="<<m.vertex.size()<<endl;
		for(auto pt : m.getBox())
			cout<<pt.x<<":"<<pt.y<<":"<<pt.z << endl;

	}
	//cout<<meshes[0].box[0].x<<meshes[0].box[0].y<<meshes[0].box[0].z<<endl;

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
	float cx,cy;
	int resX,resY;
	int facesToRender;
	int fovCoef;

	void getScreenCoord(Point&,float*,float*,float*);
	void rotate2D(float,float,float,float*,float*);
public:
	Camera(Point&,int,int,SDL_Renderer*,int,int,World);
	void update(SDL_Event*,float);
	void render();
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
	resX = width; resY = height;
	cx = width/2;
	cy = height/2;
	fovCoef = 200;
	cout << cx << " " << cy << endl;
	facesToRender = 2;
}

void Camera::update(SDL_Event*ev, float dt){
	float dist = dt*10;
	float x = dist*sin(rotation[0]), z = dist*cos(rotation[0]);
	// Keys
	switch(ev->type){
		case SDL_QUIT:{
			return;
		}
		case SDL_KEYDOWN:{
			switch(ev->key.keysym.sym){
				//Camera rotation
				case SDLK_RIGHT:rotation[0]+=0.1;break;
				case SDLK_LEFT:rotation[0]-=0.1;break;
				case SDLK_UP:rotation[1]+=0.1;break;
				case SDLK_DOWN:rotation[1]-=0.1;break;

				//Camera movement
				case 'w':position.x += x;position.z += z;break;
				case 's':position.x -= x;position.z -= z;break;
				case 'd':position.x += z;position.z -= x;break;
				case 'a':position.x -= z;position.z += x;break;
				case 'e':position.y += dist;break;
				case 'q':position.y -= dist;break;

				// Add vertex
				case 'r':world.vertex.push_back(Point(-position.x,-position.y,-position.z));break;

				// Change rendering modes
				case 'y':wireMode= !wireMode;break;
				case 'x':dotMode= !dotMode;break;
				case 'c':polyMode= !polyMode;break;
				case 'v':snakeMode= !snakeMode;break;

				// Render faces
				case 'm':facesToRender++;break;
				case 'n':(facesToRender>0)?facesToRender--:0;break;

				// Change FOV
				case 'k':fovCoef++;break;
				case 'l':(fovCoef>0)?fovCoef--:0;break;

				default:{
					break;
				}
			}
			//printf("Cam position : %f, %f, %f\n",position.x,position.y,position.z);
			break;
		}
		default: break;
	}
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

typedef struct zBuffer{
	vector<Sint16> x;
	vector<Sint16> y;
	float z;
	int r,g,b;
	bool operator<(const zBuffer & rhs) const
	{
		return z < rhs.z;
	}
};

void Camera::render(){
	cout<<"@@@@@@@ Rendering @@@@@@@@"<<endl;
	// Clear screen
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	// Get meshes in camera view
	vector<Mesh> mesh;
	for(Mesh m : world.getMeshes()){
		float x,y,z;
		cout << "Check the objects\n";
		vector<Point> v = m.getBox();
		//cout << v.size() << endl;
		for(Point& pt : m.getBox()){
			//m.printDebug();
			getScreenCoord(pt,&x,&y,&z);
			//cout << x << ":"<<y<<endl;
			if(x>=0&&x<=resX&&y>=0&&y<=resY&&z<0){
				mesh.push_back(m);
				cout << m.name << " is valid" << endl;
				break;
			}
		}
	}

	for(Mesh&m : mesh){
		// Compute screen coords for each vertex in the world
		vector<Point> screenCoords;
		for(Point&pt : m.vertex){
			float x,y,z;
			bool validity=false;
			getScreenCoord(pt,&x,&y,&z);
			// Clip points
			if((z<0.0))validity=true;
			screenCoords.push_back(Point(x,y,z,validity));
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
			for(unsigned f=0; f<m.face.size(); f++){
				for(unsigned v=1; v<m.face[f].edges.size(); v++){
					if(screenCoords[m.face[f].edges[v]].valid && screenCoords[m.face[f].edges[v-1]].valid){
						int vertex1 = m.face[f].edges[v];
						int vertex2 = m.face[f].edges[v-1];
						SDL_RenderDrawLine(renderer,(int)screenCoords[vertex1].x,(int)screenCoords[vertex1].y,(int)screenCoords[vertex2].x,(int)screenCoords[vertex2].y);
					}
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

void loop(SDL_Renderer*renderer,Camera cam){
	SDL_Event ev;
	cam.update(&ev,(1.0/100));
	cam.render();
	SDL_RenderPresent(renderer);

	for(;;){

		//SDL_RenderPresent(renderer);
		//b1.print();

		while(SDL_PollEvent(&ev)){
			cam.update(&ev,(1.0/100));
			cam.render();
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
							cout << t2.tv_sec-t1.tv_sec << " sec";
							cout << t2.tv_nsec-t1.tv_nsec << " nsec" << endl;
							break;
						}

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

	loop(renderer,camera);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
