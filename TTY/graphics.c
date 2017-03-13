#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

// Structure of a pixel in the framebuffer
struct fbPix{
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
#define resW 1920
#define resH 1080

// Frame buffer data
char fbData[resW*resH*4];
int fbSize = sizeof(fbData) / sizeof(fbData[0]);
int fd_fb0;

void fbFlip(){
	if(0 > write(fd_fb0, fbData, fbSize)){perror("fbFlip:write()");exit(-1);}
}

void fbSetPix(int x, int y, struct fbPix*color){
	int pos = (y * resW + x) * 4;
	if(pos > fbSize || y < 0 || x < 0 || color == NULL)return;
	fbData[pos] = color->blue;
	fbData[pos+1] = color->green;
	fbData[pos+2] = color->red;
	fbData[pos+3] = color->alpha;
}

void fbDrawLine(int xStart,int yStart,int xEnd,int yEnd, struct fbPix*color){
	if(xStart==xEnd){
		for(int i=yStart;i<yEnd;i++){
			fbSetPix(xStart,i,color);
		}
	}
	else{
		if(xStart>xEnd){
			int tempX=xStart,tempY=yStart;
			xStart=xEnd;
			yStart=yEnd;
			xEnd=tempX;
			yEnd=tempY;
		}
		float slope=((float)yEnd-(float)yStart)/((float)xEnd-(float)xStart);
		for(float x=(float)xStart;x<(float)xEnd;x+=0.1){
			fbSetPix((int)x,(int)((float)yStart+(float)x*slope),color);
		}
	}
}

//################## Test functions #######################
void testPixelTriangle(){
	struct fbPix*color;
	for(int y=0; y<resH; y++){
		for(int x=0; x<resW; x++){
				color = (x>16/9*y)?&cRed:&cGreen;
				fbSetPix(x,y,color);
		}
	}
}

void testPixelColors(){
	int index = 0;
	for(int y=0; y<resH; y++){
		for(int x=0; x<resW; x++){
				fbSetPix(x,y,colors[index]);
				if(x%10 == 0)index = (index >= cSize - 1)?0:index+1;
		}
	}
}

#define PI (3.1415)
void testLines(){
	const int lines = 10;
	const float radius = 50.0;

	fbDrawLine(700,700,500+50,500+50,&cWhite);
	fbDrawLine(700,700,500+50,500-50,&cGreen);
	fbDrawLine(500,500,500-50,500-50,&cGreen);
	fbDrawLine(500,500,500-50,500+50,&cBlue);

	float phi = 0.0, dPhi = 2.0*PI / (float)lines;
	for(int i=0;i<lines; i++){
		int x = (int)(radius*cos(phi));
		int y = (int)(radius*sin(phi));
		//printf("%d, %d\n",x,y);
		fbDrawLine(300,0,x+resW/2,y+resH/2,&cWhite);
		phi += dPhi;
	}
}


int main()
{
	// Opening frame buffer
	fd_fb0 = open("/dev/fb0", O_RDWR);
	if(fd_fb0 < 0){perror("open()");return -1;}

	// Test line
	//for(int i=0;i<600;i++)fbSetPix(resW/2,i,&white);
	//fbDrawLine(0,0,resW/2,resH/2,&white);
	//testLines();
	testPixelColors();
	fbFlip();
	
	close(fd_fb0);
	return 0;
}
