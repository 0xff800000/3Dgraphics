#ifndef SRC_CAMERA_HPP
#define SRC_CAMERA_HPP

#include "point.hpp"
#include "screen.hpp"
#include "world.hpp"

class Camera{
    private:
        Screen& screen;
        Point position;
        float rotation[2];
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

        int (*get_time_ms_cb)(void);

        void getScreenCoord(Point&,float*,float*,float*);
        void rotate2D(float,float,float,float*,float*);
        int get_time_ms();
    public:
        Camera(Point&,int,int,Screen&,int,int,World);
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

        void register_get_time_ms(int (*)(void));
};

#endif // SRC_CAMERA_HPP
