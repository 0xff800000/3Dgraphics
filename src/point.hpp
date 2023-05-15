#ifndef SRC_POINT_HPP
#define SRC_POINT_HPP

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
	Point(float,float,float);
	
	bool operator==(const Point&pt){
		return	(this->x == pt.x) &&
				(this->y == pt.y) &&
				(this->z == pt.z);
	};

	Point operator+(const Point&pt){
		Point res;
		res.x = this->x + pt.x;
		res.y = this->y + pt.y;
		res.z = this->z + pt.z;
		return res;
	};
	
	void operator+=(const Point&pt){
		this->x += pt.x;
		this->y += pt.y;
		this->z += pt.z;
	};

	Point operator-(const Point&pt){
		Point res;
		res.x = this->x - pt.x;
		res.y = this->y - pt.y;
		res.z = this->z - pt.z;
		return res;
	};
	
	void operator-=(const Point&pt){
		this->x -= pt.x;
		this->y -= pt.y;
		this->z -= pt.z;
	};

	Point operator/(const float&a){
		Point res;
		res.x = this->x / a;
		res.y = this->y / a;
		res.z = this->z / a;
		return res;
	};

	Point operator*(const float&a){
		Point res;
		res.x = this->x * a;
		res.y = this->y * a;
		res.z = this->z * a;
		return res;
	};
};

#endif // SRC_POINT_HPP
