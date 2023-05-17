#ifndef SRC_SCREEN_HPP
#define SRC_SCREEN_HPP

#include <vector>

#include "point.hpp"

class Screen {
    public:
        virtual void clear(void) = 0;
        virtual void render(void) = 0;
        virtual void set_draw_color(int,int,int,int) = 0;
        virtual void draw_pixel(Point&) = 0;
        virtual void draw_line(Point&, Point&) = 0;
        virtual void draw_poly(std::vector<Point>) = 0;
};

#endif // SRC_SCREEN_HPP
