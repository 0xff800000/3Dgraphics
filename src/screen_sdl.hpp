#ifndef SRC_SCREEN_SDL_HPP
#define SRC_SCREEN_SDL_HPP

#include <vector>

#include "point.hpp"
#include "screen.hpp"

class ScreenSDL : public Screen {
    public:
        ScreenSDL(SDL_Renderer* renderer, int width, int height);
        void clear(void);
        void render(void);
        void set_draw_color(int,int,int,int);
        void draw_pixel(Point&);
        void draw_line(Point&, Point&);
        void draw_poly(std::vector<Point>);
        int width, height;
    private:
        SDL_Renderer* renderer;
        int m_r, m_g, m_b, m_a;
};

#endif // SRC_SCREEN_SDL_HPP
