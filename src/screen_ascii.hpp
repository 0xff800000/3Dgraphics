#ifndef SRC_SCREEN_ASCII_HPP
#define SRC_SCREEN_ASCII_HPP

#include <vector>

#include "point.hpp"
#include "screen.hpp"

class ScreenASCII : public Screen {
    public:
        ScreenASCII(int width, int height);
        void clear(void);
        void render(void);
        void set_draw_color(int,int,int,int);
        void draw_pixel(Point&);
        void draw_line(Point&, Point&);
        void draw_poly(std::vector<Point>);
        int width, height;
    private:
        int m_r, m_g, m_b, m_a;
        char m_char_color = ' ';
        std::vector< std::vector<char> > pixels;
};

#endif // SRC_SCREEN_ASCII_HPP

