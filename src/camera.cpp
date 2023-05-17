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
    std::vector<int> x;
    std::vector<int> y;
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
            unsigned line_drawn[screenCoords.size()] = {0};
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
                        screen.draw_line(screenCoords[vertex1],screenCoords[vertex2]);
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
                std::vector<int> x;
                std::vector<int> y;
                std::vector<float> zbuff;
                for(unsigned v=0; v<m.face[f].edges.size(); v++){
                    x.push_back((int)screenCoords[m.face[f].edges[v]].x);
                    y.push_back((int)screenCoords[m.face[f].edges[v]].y);
                    zbuff.push_back(screenCoords[m.face[f].edges[v]].z);
                }
                currentFace.x = x;
                currentFace.y = y;
                currentFace.z = *std::min_element(&zbuff[0],&zbuff[zbuff.size()]);

                // Get face color
                //world.getColor(f,&currentFace.r,&currentFace.g,&currentFace.b);
                currentFace.r = (int)(m.face[f].color[0]);
                currentFace.g = (int)(m.face[f].color[1]);
                currentFace.b = (int)(m.face[f].color[2]);

                // Check validity : if one of the edges is out the screen => false
                bool valid = true;
                if(*(std::min_element(&x[0],&x[x.size()])) < 0)valid=false;
                else if(*(std::max_element(&x[0],&x[x.size()]))>resX)valid=false;
                else if(*(std::min_element(&y[0],&y[y.size()])) < 0)valid=false;
                else if(*(std::max_element(&y[0],&y[y.size()]))>resY)valid=false;


                if(currentFace.z<0 && valid)faceList.push_back(currentFace);
                //else std::cout<<"Polygon reject"<<std::endl;
            }
            std::sort(&faceList[0],&faceList[faceList.size()]);

            // Display faces
            for(unsigned f=0; f<faceList.size(); f++){
                std::vector<Point> points;
                for(unsigned pp=0; pp<faceList[f].x.size(); pp++)
                    points.push_back(Point(faceList[f].x[pp], faceList[f].y[pp],0));
                screen.set_draw_color(faceList[f].r*2,faceList[f].g/2,faceList[f].b,0xff);
                //screen.set_draw_color(rand(),rand(),rand(),0xff);
                screen.draw_poly(points);
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
