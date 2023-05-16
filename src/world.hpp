#ifndef SRC_WORLD_HPP
#define SRC_WORLD_HPP

#include <string>
#include <vector>

#include "mesh.hpp"

class World{
    private:
        int timer;
    public:
        World(){timer=0;};
        std::vector<Mesh> meshes;
        void importMesh(std::string&,float);
        void importMesh(std::string&);
        void printDebug();
        void update();
        void initMesh();
        std::vector<Mesh> getMeshes(){return meshes;};
        std::vector<Point> vertex;
        std::vector<polygon> face;
};

#endif // SRC_WORLD_HPP
