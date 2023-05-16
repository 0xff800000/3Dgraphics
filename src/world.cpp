#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

#include <SDL2/SDL.h>

#include "world.hpp"

void World::importMesh(std::string&path,float scale=1.0){
    std::ifstream objFile;
    objFile.open(&path[0]);
    std::string line;
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
                    Point pt(x,y,z);
                    pt.scale(scale);
                    vertex.push_back(pt);
                    if(meshes.size()==0){
                        meshes.push_back(Mesh());
                        meshes.back().name = "Unknown mesh";
                    }
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
                    poly.edges.push_back(poly.edges[0]);
                    poly.color[0] = rand();
                    poly.color[1] = rand();
                    poly.color[2] = rand();
                    face.push_back(poly);
                    meshes.back().face.push_back(poly);
                }
            }
        }
    }
    else {
        std::cout << "Error while opening " << path << std::endl;
    }

    initMesh();
    objFile.close();
}

void World::importMesh(std::string&path){
    importMesh(path, 1.0);
}

void World::initMesh(){
    for(Mesh& m : meshes){
        m.computeBox();
        m.zeroEdge();
        m.printDebug();
    }
}

void World::printDebug(){
    printf("The world contains %d vertexes forming %d faces.\n",(int)vertex.size(),(int)face.size());
    printf("The world contains %d objects.\n",(int)meshes.size());
    std::cout << "Object list :" << std::endl;
    for(Mesh m : meshes){
        std::cout << m.name << ":v="<<m.vertex.size()<<std::endl;
        for(auto pt : m.getBox())
            std::cout<<pt.x<<":"<<pt.y<<":"<<pt.z << std::endl;

    }

}

void World::update(){
    if(meshes.size()>=4){
        int ticks = SDL_GetTicks(); // TODO: call back
        int timePassed = ticks - timer;
        timer = ticks;
        meshes[0].rotY(timePassed*0.01);
        meshes[0].rotX(timePassed*0.01);
        meshes[1].rotY(timePassed*0.02);
        meshes[2].rotZ(timePassed*0.03);
        Point mov = Point(0.1*sin(timePassed*timer/1000.0),0,0);
        meshes[3].move(mov);
    }
}

