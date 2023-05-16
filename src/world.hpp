#ifndef SRC_WORLD_HPP
#define SRC_WORLD_HPP

#include <string>
#include <vector>

#include "mesh.hpp"

class World{
    private:
        int timer;
        int (*get_ticks_cb)(void);
    public:
        World(){timer=0;get_ticks_cb=NULL;};
        std::vector<Mesh> meshes;
        void importMesh(std::string&,float);
        void importMesh(std::string&);
        void printDebug();
        void update();
        void initMesh();
        std::vector<Mesh> getMeshes(){return meshes;};
        std::vector<Point> vertex;
        std::vector<polygon> face;

        void register_get_ticks(int (*)(void));
        int get_ticks_ms();
};

#endif // SRC_WORLD_HPP
