#include <vector>
#include <algorithm>
#include <cmath>

#include "camera.hpp"

Camera::Camera(Point&pos,int rot1,int rot2,Screen&screen,int width,int height,World w)
    : screen (screen)
{
    position = pos;
    rotation[0] = rot1; rotation[1] = rot2;
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
    std::cout << cx << " " << cy << std::endl;
    timer = 0;
    rendering = true;

    get_time_ms_cb = NULL;
}

void Camera::update(){
    rendering = true;
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
    std::vector<Point> points;
    float z;
    int r,g,b;
    int max_x = std::numeric_limits<int>::min();
    int max_y = std::numeric_limits<int>::min();
    int min_x = std::numeric_limits<int>::max();
    int min_y = std::numeric_limits<int>::max();

    bool operator<(const zBuffer & rhs) const
    {
        return z < rhs.z;
    }

    void push_back_point(Point&pt) {
        points.push_back(pt);
        if(max_x < pt.x) max_x = pt.x;
        if(max_y < pt.y) max_y = pt.y;
        if(min_x > pt.x) min_x = pt.x;
        if(min_y > pt.y) min_y = pt.y;
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
    // Clear screen
    screen.clear();
    // Get meshes in camera view
    std::vector<Mesh> mesh;
    for(Mesh m : world.getMeshes()){
        float x,y,z;
        std::vector<Point> v = m.getBox();
        for(Point& pt : m.getBox()){
            getScreenCoord(pt,&x,&y,&z);
            if(x>=0&&x<=resX&&y>=0&&y<=resY&&z<0){
                mesh.push_back(m);
                break;
            }
        }
    }

    // Sort meshes according to the distance from the camera
    std::vector<Mesh> orderedMesh;
    std::vector<zBufferMesh> buffer;
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
    std::sort(buffer.begin(),buffer.end());
    for(zBufferMesh&m : buffer){
        orderedMesh.push_back(m.mesh);
    }
    mesh = orderedMesh;


    for(Mesh&m : mesh){
        screen.set_draw_color(0,0,0,0);
        // Compute screen coords for each vertex in the world
        std::vector<Point> screenCoords;
        for(Point&pt : m.vertex){
            float x,y,z;
            bool validity=false;
            getScreenCoord(pt,&x,&y,&z);
            // Clip points
            if((z<0.0))validity=true;
            Point new_pt = Point(x,y,z,validity);

            if(acidShader){
                static long int ms = 0;
                ms +=  get_time_ms() % 2;

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
                    screen.draw_pixel(pt);
                }
            }
        }

        // Render snake
        if(snakeMode){
            for(unsigned i=1; i<screenCoords.size(); i++){
                if(screenCoords[i].valid && screenCoords[i-1].valid){
                    screen.draw_line(screenCoords[i],screenCoords[i-1]);
                }
            }
        }

        // Render wireframe
        if(wireMode){
            std::vector<int> line_drawn[screenCoords.size()];
            for(unsigned f=0; f<m.face.size(); f++){
                for(unsigned v=1; v<m.face[f].edges.size(); v++){
                    if(screenCoords[m.face[f].edges[v]].valid && screenCoords[m.face[f].edges[v-1]].valid){
                        int vertex1 = m.face[f].edges[v];
                        int vertex2 = m.face[f].edges[v-1];
                        auto it = std::find(line_drawn[vertex1].begin(), line_drawn[vertex1].end(), vertex2);
                        if(it != line_drawn[vertex1].end()) continue;
                        screen.draw_line(screenCoords[vertex1],screenCoords[vertex2]);
                        line_drawn[vertex1].push_back(vertex2);
                        line_drawn[vertex2].push_back(vertex1);
                    }
                }
            }
        }

        // Render mesh box
        if(meshBox){
            screen.set_draw_color(0xff,0,0,0);
            std::vector<Point> boxEdge;
            for(Point pt : m.getBox()){
                float x,y,z;
                bool validity = false;
                getScreenCoord(pt,&x,&y,&z);
                if(z<0)validity = true;
                boxEdge.push_back(Point(x,y,z,validity));
            }
            for(int i = 0; i<12; i++){
                if(boxEdge[boxIndexes[i][0]].valid && boxEdge[boxIndexes[i][1]].valid){
                    screen.draw_line(boxEdge[boxIndexes[i][0]],boxEdge[boxIndexes[i][1]]);
                }
            }
        }

        if(polyMode){
            std::vector<zBuffer> faceList;
            // Create and sort faceList
            for(unsigned f=0; f<m.face.size(); f++){
                zBuffer currentFace;
                std::vector<Point> points;
                std::vector<float> zbuff;
                for(unsigned v=0; v<m.face[f].edges.size(); v++){
                    currentFace.push_back_point(screenCoords[m.face[f].edges[v]]);
                    zbuff.push_back(screenCoords[m.face[f].edges[v]].z);
                }
                currentFace.z = *std::min_element(&zbuff[0],&zbuff[zbuff.size()]);

                // Get face color
                //world.getColor(f,&currentFace.r,&currentFace.g,&currentFace.b);
                currentFace.r = (int)(m.face[f].color[0]);
                currentFace.g = (int)(m.face[f].color[1]);
                currentFace.b = (int)(m.face[f].color[2]);

                // Check validity : if one of the edges is out the screen => false
                bool valid = true;
                if(currentFace.min_x < 0)valid=false;
                else if(currentFace.max_x>resX)valid=false;
                else if(currentFace.min_y < 0)valid=false;
                else if(currentFace.max_y>resY)valid=false;


                if(currentFace.z<0 && valid)faceList.push_back(currentFace);
                //else std::cout<<"Polygon reject"<<std::endl;
            }
            std::sort(&faceList[0],&faceList[faceList.size()]);

            // Display faces
            for(unsigned f=0; f<faceList.size(); f++){
                screen.set_draw_color(faceList[f].r*2,faceList[f].g/2,faceList[f].b,0xff);
                //screen.set_draw_color(rand(),rand(),rand(),0xff);
                screen.draw_poly(faceList[f].points);
            }
        }
    }

    screen.render();
}

void Camera::move(Point&pt) {
    position += pt;
    rendering = true;
}


void Camera::move_x(float dist) {
    float x = dist*sin(rotation[0]), z = dist*cos(rotation[0]);
    Point pt(x,0,z);
    move(pt);
}

void Camera::move_y(float dist) {
    float z = -dist*sin(rotation[0]), x = dist*cos(rotation[0]);
    Point pt(x,0,z);
    move(pt);
}

void Camera::move_z(float dist) {
    Point pt(0,dist,0);
    move(pt);
}

void Camera::rot_yaw(float angle) {
    rotation[0] += angle;
}

void Camera::rot_pitch(float angle) {
    rotation[1] += angle;
}

int Camera::get_time_ms()
{
    if(!get_time_ms_cb)
        return ++timer;
    else
        return get_time_ms_cb();
}

void Camera::register_get_time_ms(int (*cb)(void)) {
    get_time_ms_cb = cb;
}
