#include <iostream>
#include <cmath>
#include "point.hpp"

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
    std::cout<<"("<<this->x<<","<<this->y<<","<<this->z<<")"<<std::endl;
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

Point::Point(float xVal,float yVal,float zVal){
	this->x=xVal;this->y=yVal;this->z=zVal;valid = false;
}
