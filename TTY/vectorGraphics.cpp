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

#define WIDTH 1280
#define HEIGHT 1024

using namespace std;

// Structure of a pixel in the framebuffer
struct fbPix {
  char blue;
  char green;
  char red;
  char alpha;
};

// Basic colors
struct fbPix cRed = {(char)0x00,(char)0x00,(char)0xff,(char)0x00};
struct fbPix cGreen = {(char)0x00,(char)0xff,(char)0x00,(char)0x00};
struct fbPix cBlue = {(char)0xff,(char)0x00,(char)0x00,(char)0x00};
struct fbPix cWhite = {(char)0xff,(char)0xff,(char)0xff,(char)0x00};
struct fbPix cBlack = {(char)0x00,(char)0x00,(char)0x00,(char)0x00};

struct fbPix* colors[] = {&cRed,&cGreen,&cBlue,&cWhite,&cBlack};
int cSize = sizeof(colors) / sizeof(colors[0]);

// Screen resolution
int resW;
int resH;

// Frame buffer data
char *fbData;
int fbSize;
int fd_fb0;

void fbInit(int w, int h){
  resW = w; resH = h;
  fd_fb0 = open("/dev/fb0", O_RDWR);
  if(!fd_fb0){
    perror("fbInit()");
    exit(-1);
  }
  fbData = (char*) malloc(resW*resH*4);
  fbSize = resW*resH*4;
  if(fbData == NULL){
    perror("malloc screen()");
    exit(-1);
  }

}

void fbDeinit(){
  close(fd_fb0);
}

void fbFlip() {
  if(fd_fb0 < 0){perror("open()");exit(-1);}
  if(0 > write(fd_fb0, fbData, fbSize)) {
    perror("fbFlip:write()");
    exit(-1);
  }
  lseek(fd_fb0,0,SEEK_SET);
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

void Camera::update(float dt) {
  float dist = dt*10;
  float x = dist*sin(rotation[0]), z = dist*cos(rotation[0]);
  // Keys
  world.rotateX(0.1);
  //rotation[0]+=0.1;
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

  // Compute screen coords for each vertex in the world
  vector<Point> screenCoords;
  for(unsigned i=0; i<world.vertex.size(); i++) {
    float x,y,z;
    getScreenCoord(world.vertex[i],&x,&y,&z);
    screenCoords.push_back(Point(x,y,z));
    // Render points
    if(dotMode) {
      if(z<0) {
      }
    }
  }

  // Render wireframe
  if(wireMode) {
    for(unsigned f=0; f<world.face.size(); f++) {
      for(unsigned v=1; v<world.face[f].edges.size(); v++) {
        if(screenCoords[world.face[f].edges[v]].z<0 && screenCoords[world.face[f].edges[v-1]].z<0) {
          int vertex1 = world.face[f].edges[v];
          int vertex2 = world.face[f].edges[v-1];
          fbDrawLine((int)screenCoords[vertex1].x,(int)screenCoords[vertex1].y,(int)screenCoords[vertex2].x,(int)screenCoords[vertex2].y,&cBlack);
        }
      }
    }
  }
}

//#############################################################################################

void loop(/*SDL_Renderer*renderer,*/Camera cam) {

  for(;;) {

    fbFlip();

    cam.update(/*&ev,*/(1.0/100));
    cam.render();
    fbFlip();
  }
}


int main(int argc, char** argv) {
  // Load world
  float divider=0;
  World world;
  string path;
  if(argc == 1){
    path="ape2.obj";
  }
  else{
    path=argv[1];
  }
  if(argc == 3)divider=atof(argv[2]);
  if(divider){
    world.importMesh(path,1.0/divider);
  }
  else{
    world.importMesh(path);
  }
  //world.printDebug();

  int width=WIDTH,height=HEIGHT;
  // Auto detect framebuffer resolution
  int fb = open("/sys/class/graphics/fb0/modes", O_RDONLY);
  if(fb != 0){
    char line[32];
    int w,h;
    char s1='\0',*s2=NULL;
    read(fb,line,32);
    int count = sscanf(line, "%c%c%dx%d%s",&s1,&s1,&w,&h,s2);
    if(count == 4){
      printf("Resolution of fb0 : %dx%d\n",w,h);
      width = w; height = h;
    }
  }
  // Create camera
  Point camPos(0,0,-5); 
  Camera camera(camPos,0,0,width,height,world);

  //Black screen
  fbInit(width, height);
  fbScreenFill(&cBlack);
  fbFlip();
  loop(camera);
  fbDeinit();
  return 0;
}
