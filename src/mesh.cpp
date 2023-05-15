#include "mesh.hpp"

int boxIndexes[12][2] = {
	{0,1},{1,3},{3,2},{2,0},{0,4},{1,5},{3,7},{2,6},{4,5},{5,7},{7,6},{6,4}
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
    std::cout<<"initMesh " <<name<< std::endl;
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

	// Create box
	box[0].assign(x_min,y_min,z_min);
	box[1].assign(x_max,y_min,z_min);
	box[2].assign(x_min,y_min,z_max);
	box[3].assign(x_max,y_min,z_max);
	box[4].assign(x_min,y_max,z_min);
	box[5].assign(x_max,y_max,z_min);
	box[6].assign(x_min,y_max,z_max);
	box[7].assign(x_max,y_max,z_max);

	// Compute center of box
	pos = box[0] + (box[1]-box[0])/2.0 + (box[2]-box[0])/2.0 + (box[4]-box[0])/2.0;
    std::cout << "Center : " << std::endl; pos.print();
}

void Mesh::zeroEdge(){
	// Find minimum index
	int minI = 0x7fffffff;
	for(polygon f : face){
		for(int i : f.edges){
			if(i<minI) minI = i;
		}
	}
	// Substract the index
	for(polygon&f : face){
		for(int&i : f.edges){
			i -= minI;
		}
	}
}

void Mesh::printDebug(){
    std::cout << "Object name : " << name << std::endl;
	for(Point pt : box){
        std::cout << pt.x << ":" << pt.y << ":" << pt.z << std::endl;
	}
}

void Mesh::rotX(float rad){
	for(Point&v : vertex){
		Point res = v - pos;
		res.rotX(rad);
		v = res + pos;
	}
	for(Point&v : box){
		Point res = v - pos;
		res.rotX(rad);
		v = res + pos;
	}
}

void Mesh::rotY(float rad){
	for(Point&v : vertex){
		Point res = v - pos;
		res.rotY(rad);
		v = res + pos;
	}
	for(Point&v : box){
		Point res = v - pos;
		res.rotY(rad);
		v = res + pos;
	}
}

void Mesh::rotZ(float rad){
	for(Point&v : vertex){
		Point res = v - pos;
		res.rotZ(rad);
		v = res + pos;
	}
	for(Point&v : box){
		Point res = v - pos;
		res.rotZ(rad);
		v = res + pos;
	}
}

void Mesh::move(Point& pt){
	for(Point&v : vertex){
		v = v + pt;
	}
	for(Point&v : box){
		v = v + pt;
	}
	pos = pos + pt;
}

