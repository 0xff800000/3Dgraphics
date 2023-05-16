#ifndef SRC_CAMERA_HPP
#define SRC_CAMERA_HPP

#include <SDL2/SDL.h> // TODO: remove dependency

#include "point.hpp"
#include "world.hpp"

class Camera{
    private:
        Point position;
        float rotation[2];
        SDL_Renderer*renderer;
        World world;
        bool dotMode;
        bool wireMode;
        bool polyMode;
        bool snakeMode;
        bool meshBox;
        bool acidShader;
        float cx,cy;
        int resX,resY;
        int fovCoef;
        int timer;
        bool rendering;

        void getScreenCoord(Point&,float*,float*,float*);
        void rotate2D(float,float,float,float*,float*);
    public:
        Camera(Point&,int,int,SDL_Renderer*,int,int,World);
        void update();
        void render();
        void wireModeToggle(){wireMode = !wireMode;};
        void dotModeToggle(){dotMode = !dotMode;};
        void polyModeToggle(){polyMode = !polyMode;};
        void snakeModeToggle(){snakeMode = !snakeMode;};
        void meshBoxToggle(){meshBox = !meshBox;};
        void acidShaderToggle(){acidShader = !acidShader;};
        void FOVinc(){fovCoef++;};
        void FOVdec(){(fovCoef>0)?fovCoef--:0;};
        Point getPos();

        void move(Point&);
        void move_x(float);
        void move_y(float);
        void move_z(float);

        void rot_yaw(float);
        void rot_pitch(float);
};

#endif // SRC_CAMERA_HPP
