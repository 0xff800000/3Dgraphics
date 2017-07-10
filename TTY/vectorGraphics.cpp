//============================================================================
// Name        : vectorGraphics.cpp
// Author      : D01000100
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <vector>
#include <cmath>
#include <algorithm>
//#include <SDL2/SDL.h>

//#include "SDL2_gfxPrimitives.h"
/*
//#define M_PI  3.14159265358979323846
#define M_PI_2  1.57079632679489661923
#define M_PI_4  0.785398163397448309616
*/
#define WIDTH 1600
#define HEIGHT 900

using namespace std;

// Structure of a pixel in the framebuffer
struct fbPix {
    char blue;
    char green;
    char red;
    char alpha;
};

// Basic colors
struct fbPix cRed = {0,0,0xff,0};
struct fbPix cGreen = {0,0xff,0,0};
struct fbPix cBlue = {0xff,0,0,0};
struct fbPix cWhite = {0xff,0xff,0xff,0};
struct fbPix cBlack = {0,0,0,0};

struct fbPix* colors[] = {&cRed,&cGreen,&cBlue,&cWhite,&cBlack};
int cSize = sizeof(colors) / sizeof(colors[0]);

// Screen resolution
#define resW 1600
#define resH 900

// Frame buffer data
char fbData[resW*resH*4];
int fbSize = sizeof(fbData) / sizeof(fbData[0]);
int fd_fb0;

void fbFlip() {
    fd_fb0 = open("/dev/fb0", O_RDWR);
    if(fd_fb0 < 0){perror("open()");exit(-1);}
    if(0 > write(fd_fb0, fbData, fbSize)) {
        perror("fbFlip:write()");
        exit(-1);
    }
    close(fd_fb0);
}

void fbSetPix(int x, int y, struct fbPix*color) {
    if((x<0)||(y<0)||(x>resW)||(y>resH))return;
    int pos = (y * resW + x) * 4;
    if(pos > fbSize || y < 0 || x < 0 || color == NULL)return;
    fbData[pos] = color->blue;
    fbData[pos+1] = color->green;
    fbData[pos+2] = color->red;
    fbData[pos+3] = color->alpha;
}

void fbDrawLineH(int x1, int x2, int y, struct fbPix*color){
	int temp;
	if((y < 0) || (y > resH))return;
	if(x1 > x2){
		temp = x1;
		x1 = x2;
		x2 = temp;
	}
	if((x1 > resW) || (x2 < 0))return;
	if(x1 < 0)x1 = 0;
	if(x2 > resW)x2 = resW;
	for(int x = x1; x<x2; x++)fbSetPix(x,y,color);
}

void fbDrawLineV(int x, int y1, int y2, struct fbPix*color){
	int temp;
	if((x < 0) || (x > resW))return;
	if(y1 > y2){
		temp = y1;
		y1 = y2;
		y2 = temp;
	}
	if((y1 > resH) || (y2 < 0))return;
	if(y1 < 0)y1 = 0;
	if(y2 > resH)y2 = resH;
	for(int y = y1; y<y2; y++)fbSetPix(x,y,color);
}

int clipCodeGet(int x, int y){
	int code=0;
	if(y < 0)code |= 1;
	if(y > resH)code |= 2;
	if(x < 0)code |= 4;
	if(x > resW)code |= 8;
	return code;
}

int clipLine(int *x1,int *x2, int *y1, int *y2){
	int code,code1, code2,x,y;
	code1 = clipCodeGet(*x1,*y1);
	code2 = clipCodeGet(*x2,*y2);
	while(1){
		if((code1 == 0) && (code2 == 0))return 1;
		if((code1 & code2) != 0)return 0;
		code = (code1)? code1:code2;
		if(code & 1){
			x=(*x1+(((*x2-*x1)*(0 - *y1))/(*y2-*y1)));
			y=0;
		}
		else if(code & 2){
			x=(*x1+(((*x2-*x1)*(resW - *y1))/(*y2-*y1)));
			y=resH;
		}
		else if(code & 4){
			x=0;
			y=(*y1+(((*y2-*y1)*(0-*x1))/(*x2-*x1)));
		}
		else{
			x=resW;
			y=(*y1+(((*y2-*y1)*(resW-*x1))/(*x2-*x1)));
		}
		if(code1){
			*x1 = x;
			*y1 = y;
			code1=clipCodeGet(x,y);
		}
		else{
			*x2 = x;
			*y2 = y;
			code2=clipCodeGet(x,y);
		}
	}
}

void fbDrawLine(int x1,int y1,int x2,int y2, struct fbPix*color) {
	if(x1 == x2){fbDrawLineV(x1,y1,y2,color);return;}
	if(y1 == y2){fbDrawLineH(x1,x2,y1,color);return;}
	if(clipLine(&x1,&x2,&y1,&y2) == 0)return;
	//Steep
	int steep = (((y2 > y1)?(y2-y1):(y1-y2)) > ((x2 > x1)?(x2-x1):(x1-x2)))?1:0;
	if(steep){
		int error;
		error = x1;
		x1 = y1;
		y1 = error;
		error = x2;
		x2 = y2;
		y2 = error;
	}

	if(x1 > x2){
		int error;
		error = x1;
		x1 = x2;
		x2 = error;
		error = y1;
		y1 = y2;
		y2 = error;
	}
	int dx = x2-x1;
	int dy = (y2>y1)?(y2-y1):(y1-y2);
	int error = -dx >> 1;
	int yStep = (y1 < y2)?1:-1;
	for(; x1 <= x2; x1++){
		if(steep)fbSetPix(y1,x1,color);
		else fbSetPix(x1,y1,color);
		error += dy;
		if(error > 0){
			y1 += yStep;
			error -= dx;
		}
	}
}
	
/*
void fbDrawLine(int xStart,int yStart,int xEnd,int yEnd, struct fbPix*color) {
    if(xStart==xEnd) {
        for(int i=yStart; i<yEnd; i++) {
            fbSetPix(xStart,i,color);
        }
    }
    else {
        if(xStart>xEnd) {
            int tempX=xStart,tempY=yStart;
            xStart=xEnd;
            yStart=yEnd;
            xEnd=tempX;
            yEnd=tempY;
        }
        float slope=((float)yEnd-(float)yStart)/((float)xEnd-(float)xStart);
        for(float x=(float)xStart; x<(float)xEnd; x+=0.1) {
            fbSetPix((int)x,(int)((float)yStart+(float)x*slope),color);
        }
    }
}
*/
void fbScreenFill(struct fbPix*color){
	for(int y=0; y<resH; y++){
		for(int x=0; x<resW; x++){
			fbSetPix(x,y,color);
		}
	}
}

//###################################### Vector Class #######################################################

class Point {
public:
    float x;
    float y;
    float z;

    void rotX(float);
    void rotY(float);
    void rotZ(float);
    void scale(float);

    void print();
    void assign(float,float,float);
    bool disp2D();

    Point();
    Point(float,float,float);
};

void Point::rotX(float rad) {
    float tY=this->y;
    float tZ=this->z;
    this->y=cos(rad)*tY-sin(rad)*tZ;
    this->z=sin(rad)*tY+cos(rad)*tZ;
}

void Point::rotY(float rad) {
    float tX=this->x;
    float tZ=this->z;
    this->x=cos(rad)*tX+sin(rad)*tZ;
    this->z=-sin(rad)*tX+cos(rad)*tZ;
}

void Point::rotZ(float rad) {
    float tX=this->x;
    float tY=this->y;
    this->x=cos(rad)*tX-sin(rad)*tY;
    this->y=sin(rad)*tX+cos(rad)*tY;
}

void Point::scale(float coef) {
    this->x*=coef;
    this->y*=coef;
    this->z*=coef;
}

void Point::print() {
    cout<<"("<<this->x<<","<<this->y<<","<<this->z<<")"<<endl;
}

void Point::assign(float xV,float yV,float zV) {
    this->x=xV;
    this->y=yV;
    this->z=zV;
}

bool Point::disp2D() {
    return (this->x>=0&&this->y>=0)?true:false;
}

Point::Point() {
    this->x=0.0;
    this->y=0.0;
    this->z=0.0;
}

Point::Point(float xVal,float yVal,float zVal) {
    this->x=xVal;
    this->y=yVal;
    this->z=zVal;
}

//###################################### World Class #######################################################

typedef struct {
    vector<int> edges;
    unsigned color[3];
} polygon;

class World {
private:
public:
    void rotateX(float);
    void importMesh(string&,float);
    void printDebug();
    vector<Point> vertex;
    vector<polygon> face;
};

void World::rotateX(float rad){
	for(int p=0; p<vertex.size(); p++){
		vertex[p].rotY(rad);
	}
}

void World::importMesh(string&path,float scale=1.0) {
    ifstream objFile;
    objFile.open(&path[0]);
    string line;
    if(objFile.is_open()) {
        while(getline(objFile,line)) {
            //cout << line << endl;
            char str[10];
            float x,y,z;
            sscanf(&line[0],"%s %f %f %f",str,&x,&y,&z);
            //cout << str << endl;
            if(strcmp(str,"v")==0) {
                // Vertex definition
                //printf("Vertex : %f, %f, %f\n",x,y,z);
                Point pt(x,y,z);
                pt.scale(scale);
                vertex.push_back(pt);
            }
            else if(strcmp(str,"f")==0) {
                // Face definition
                //cout << line<<endl;
                polygon poly;
                char delim[] = "//";
                char*token;
                int vertexIndex;
                token = strtok(&line[0], delim);
                while(token != NULL) {
                    if(sscanf(token,"%*s %d",&vertexIndex)) {
                        //cout << vertexIndex << endl;
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
            }
        }
    }
    else {
        cout << "Error while opening " << path << endl;
    }
}

void World::printDebug() {
    printf("The world contains %d vertexes forming %d faces.\n",vertex.size(),face.size());
    cout << "Vertex list:" << endl;
    for(unsigned int i=0; i<vertex.size(); i++) {
        printf("%i : %f,%f,%f\n",i,vertex[i].x,vertex[i].y,vertex[i].z);
    }
    cout << "\n\nFace list:" << endl;
    for(unsigned int i=0; i<face.size(); i++) {
        cout << i << " : ";
        for(unsigned int j=0; j<face[i].edges.size(); j++) {
            cout<<face[i].edges[j]<<",";
        }
        cout<<endl;
    }

}

//#############################################################################################


//######################################## Camera Class #####################################################

class Camera {
private:
    Point position;
    float rotation[2];
    //SDL_Renderer*renderer;
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
    Camera(Point&,int,int,/*SDL_Renderer*,*/int,int,World);
    void update(/*SDL_Event*,*/float);
    void render();
    Point getPos();
};

Camera::Camera(Point&pos,int rot1,int rot2,/*SDL_Renderer*surface,*/int width,int height,World w) {
    position = pos;
    rotation[0] = rot1;
    rotation[1] = rot2;
    /*renderer = surface;*/
    world = w;
    dotMode = true;
    wireMode = true;
    polyMode = false;
    snakeMode = false;
    resX = width;
    resY = height;
    cx = width/2;
    cy = height/2;
    fovCoef = 350;
    //cout << cx << " " << cy << endl;
    facesToRender = 2;
}

void Camera::update(/*SDL_Event*ev,*/ float dt) {
    float dist = dt*10;
    float x = dist*sin(rotation[0]), z = dist*cos(rotation[0]);
    // Keys
    world.rotateX(0.1);
    //rotation[0]+=0.1;
/*
    switch(ev->type) {
    case SDL_QUIT: {
        return;
    }
    case SDL_KEYDOWN: {
        switch(ev->key.keysym.sym) {
        //Camera rotation
        case SDLK_RIGHT:
            rotation[0]+=0.1;
            break;
        case SDLK_LEFT:
            rotation[0]-=0.1;
            break;
        case SDLK_UP:
            rotation[1]+=0.1;
            break;
        case SDLK_DOWN:
            rotation[1]-=0.1;
            break;

        //Camera movement
        case 'w':
            position.x += x;
            position.z += z;
            break;
        case 's':
            position.x -= x;
            position.z -= z;
            break;
        case 'd':
            position.x += z;
            position.z -= x;
            break;
        case 'a':
            position.x -= z;
            position.z += x;
            break;
        case 'e':
            position.y += dist;
            break;
        case 'q':
            position.y -= dist;
            break;

        // Add vertex
        case 'r':
            world.vertex.push_back(Point(-position.x,-position.y,-position.z));
            break;

        // Change rendering modes
        case 'y':
            wireMode= !wireMode;
            break;
        case 'x':
            dotMode= !dotMode;
            break;
        case 'c':
            polyMode= !polyMode;
            break;
        case 'v':
            snakeMode= !snakeMode;
            break;

        // Render faces
        case 'm':
            facesToRender++;
            break;
        case 'n':
            (facesToRender>0)?facesToRender--:0;
            break;

        // Change FOV
        case 'k':
            fovCoef++;
            break;
        case 'l':
            (fovCoef>0)?fovCoef--:0;
            break;

        default: {
            break;
        }
        }
        //printf("Cam position : %f, %f, %f\n",position.x,position.y,position.z);
        break;
    }
    default:
        break;
    }*/
}

void Camera::rotate2D(float posX, float posY, float angle,float*xo,float*yo) {
    float c = cos(angle), s = sin(angle);
    *xo = posX*c - posY*s;
    *yo = posX*s + posY*c;
}

void Camera::getScreenCoord(Point&vertex,float*screenX,float*screenY,float*depth) {
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

Point Camera::getPos() {
    return position;
}

typedef struct zBuffer {
    vector<int> x;
    vector<int> y;
    float z;
    int r,g,b;
    bool operator<(const zBuffer & rhs) const
    {
        return z < rhs.z;
    }
};

void Camera::render() {
    // Clear screen
    fbScreenFill(&cWhite);
    //fbFlip();
/*
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
*/

    // Compute screen coords for each vertex in the world
    vector<Point> screenCoords;
    for(unsigned i=0; i<world.vertex.size(); i++) {
        float x,y,z;
        getScreenCoord(world.vertex[i],&x,&y,&z);
        screenCoords.push_back(Point(x,y,z));
        // Render points
        if(dotMode) {
            if(z<0) {
                //SDL_RenderDrawPoint(renderer,(int)x,(int)y);
                //filledCircleRGBA(renderer,(int)x,(int)y,2,0x00,0x00,0x00,0xff);
            }
        }
    }
/*
    // Render snake
    if(snakeMode) {
        for(unsigned i=1; i<screenCoords.size(); i++) {
            if(screenCoords[i].z<0.0 && screenCoords[i-1].z<0.0) {
                SDL_RenderDrawLine(renderer,(int)screenCoords[i].x,(int)screenCoords[i].y,(int)screenCoords[i-1].x,(int)screenCoords[i-1].y);
                //thickLineRGBA(renderer,(int)screenCoords[i].x,(int)screenCoords[i].y,(int)screenCoords[i-1].x,(int)screenCoords[i-1].y,3,0,0,0,0xff);
            }
        }
    }
*/
    // Render wireframe
    if(wireMode) {
        for(unsigned f=0; f<world.face.size(); f++) {
            for(unsigned v=1; v<world.face[f].edges.size(); v++) {
                if(screenCoords[world.face[f].edges[v]].z<0 && screenCoords[world.face[f].edges[v-1]].z<0) {
                    int vertex1 = world.face[f].edges[v];
                    int vertex2 = world.face[f].edges[v-1];
                    //SDL_RenderDrawLine(renderer,(int)screenCoords[vertex1].x,(int)screenCoords[vertex1].y,(int)screenCoords[vertex2].x,(int)screenCoords[vertex2].y);
fbDrawLine((int)screenCoords[vertex1].x,(int)screenCoords[vertex1].y,(int)screenCoords[vertex2].x,(int)screenCoords[vertex2].y,&cBlack);
                }
            }
        }
    }
/*
    // Render polygon
    if(polyMode) {
        vector<zBuffer> faceList;
        // Create and sort faceList
        for(unsigned f=0; f<world.face.size(); f++) {
            zBuffer currentFace;
            vector<Sint16> x;
            vector<Sint16> y;
            vector<float> zbuff;
            for(unsigned v=0; v<world.face[f].edges.size(); v++) {
                x.push_back((int)screenCoords[world.face[f].edges[v]].x);
                y.push_back((int)screenCoords[world.face[f].edges[v]].y);
                zbuff.push_back(screenCoords[world.face[f].edges[v]].z);
            }
            currentFace.x = x;
            currentFace.y = y;
            currentFace.z = *min_element(&zbuff[0],&zbuff[zbuff.size()]);

            // Get face color
            //world.getColor(f,&currentFace.r,&currentFace.g,&currentFace.b);
            currentFace.r = (Sint16)(world.face[f].color[0]);
            currentFace.g = (Sint16)(world.face[f].color[1]);
            currentFace.b = (Sint16)(world.face[f].color[2]);

            // Check validity
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
        for(unsigned f=0; f<faceList.size(); f++) {
            filledPolygonRGBA(renderer,&faceList[f].x[0],&faceList[f].y[0],faceList[f].x.size(),faceList[f].r*2,faceList[f].g/2,faceList[f].b,0xff);
            //filledPolygonRGBA(renderer,&faceList[f].x[0],&faceList[f].y[0],faceList[f].x.size(),rand(),rand(),rand(),0xff);
        }
    }
    /*
    // Render polygon
    if(polyMode){
    	vector<zBuffer> faceList;
    	// Create and sort faceList
    	for(unsigned f=0; f<world.face.size(); f++){
    		zBuffer currentFace;
    		vector<Sint16> x;
    		vector<Sint16> y;
    		vector<float> zbuff;
    		for(unsigned v=0; v<world.face[f].edges.size(); v++){
    			x.push_back((int)screenCoords[world.face[f].edges[v]].x);
    			y.push_back((int)screenCoords[world.face[f].edges[v]].y);
    			zbuff.push_back(screenCoords[world.face[f].edges[v]].z);
    		}
    		currentFace.x = x;
    		currentFace.y = y;
    		currentFace.z = *max_element(&zbuff[0],&zbuff[zbuff.size()]);
    		world.getColor(f,&currentFace.r,&currentFace.g,&currentFace.b);

    		if(currentFace.z<0)faceList.push_back(currentFace);
    		//else cout<<"Polygon reject"<<endl;
    	}

    	// Display faces
    	for(unsigned f=0; f<faceList.size(); f++){
    		//filledPolygonRGBA(renderer,&faceList[f].x[0],&faceList[f].y[0],faceList[f].x.size(),faceList[f].r,faceList[f].g,faceList[f].b,0xff);
    		filledPolygonRGBA(renderer,&faceList[f].x[0],&faceList[f].y[0],faceList[f].x.size(),rand(),rand(),rand(),0xff);
    	}
    }*/

}

//#############################################################################################

void loop(/*SDL_Renderer*renderer,*/Camera cam) {

    //SDL_Event ev;
    for(;;) {

        //SDL_RenderPresent(renderer);
fbFlip();
        //b1.print();

        /*while(SDL_PollEvent(&ev)) {*/
            cam.update(/*&ev,*/(1.0/100));
            cam.render();
            fbFlip();//SDL_RenderPresent(renderer);
            /*switch(ev.type) {
            case SDL_QUIT: {
                return;
            }
            case SDL_KEYDOWN: {
                switch(ev.key.keysym.sym) {
                case SDLK_ESCAPE:
                    return;

                default: {
                    break;
                }
                }
                break;
            }
            default:
                break;
            }
        }*/
        /*
            	SDL_Delay(20);
        		SDL_SetRenderDrawColor(renderer,0,0,0,0);
        		SDL_RenderClear(renderer);
        		SDL_SetRenderDrawColor(renderer,255,255,255,255);*/
    }
}


int main(int argc, char** argv) {
    // Load world
    World world;
    string path;
    if(argc == 1){
        path="ape2.obj";
    }
    else{
        path=argv[1];
    }
    //world.importMesh(path,0.001);
    world.importMesh(path);
    //world.printDebug();

    //freopen("CON", "w", stdout);

    const int width=WIDTH,height=HEIGHT;
/*
    SDL_Window* window = SDL_CreateWindow
                         (
                             "Vector Graphics", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             width,
                             height,
                             SDL_WINDOW_SHOWN
                         );

    SDL_Renderer*renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
*/
    // Create camera
    Point camPos(0,0,-5);
    Camera camera(camPos,0,0,/*renderer,*/width,height,world);

    //Black screen
    fbScreenFill(&cBlack);
    fbFlip();
/*
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
*/
    loop(/*renderer,*/camera);
/*
    SDL_DestroyWindow(window);
*/
    //SDL_Quit();

    return 0;
}
