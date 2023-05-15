#ifndef SRC_MESH_HPP
#define SRC_MESH_HPP

#include <vector>
#include <string>
#include <iostream>
#include <limits>

#include "point.hpp"

extern int boxIndexes[12][2];

typedef struct{
    std::vector<int> edges;
	unsigned color[3];
}polygon;

class Mesh {
public:
    std::string name;
	Point pos;
    std::vector<Point> vertex;
	std::vector<polygon> face;
	Mesh();
    std::vector<Point>& getBox(){return box;};
	virtual ~Mesh (){};
	void computeBox();
	void printDebug();
	void zeroEdge();

	void rotX(float);
	void rotY(float);
	void rotZ(float);
	void move(Point& pt);

private:
    std::vector<Point> box;
};

#endif // SRC_MESH_HPP
