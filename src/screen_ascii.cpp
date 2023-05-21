#include "screen_ascii.hpp"

const std::string CSI{"\x1b["};
void set_cursor_pos(int x, int y) {
    std::cout << CSI << x << ';' << y << std::endl;
}

ScreenASCII::ScreenASCII(int width, int height)
    , width (width)
    , height (height)
{
    if(!width || !height)
        throw;
    m_r=0; m_g=0; m_b=0; m_a=0;
    m_char_color = ' ';

    for(int y=0; y<height; y++) {
        std::vector<char> line;
        for(int x=0; x<width; x++)
            line.push_back(' ');
        pixels.push_back(line);
    }
}

void ScreenASCII::clear() {
    for(int y=0; y<height; y++)
        for(int x=0; x<width; x++)
            pixels[x][y] = ' ';
    render();
}

void ScreenASCII::render(void) {
    set_cursor_pos(0,0);
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++)
            printf("%c", pixels[x][y]);
        printf("\n", pixels[x][y]);
    }
}

void ScreenASCII::set_draw_color(int r, int g, int b, int a) {
    m_r=r; m_g=g; m_b=b; m_a=a;
    if( !r && !g && !b )
        m_char_color = ' ';
    else
        m_char_color = '#';
}

void ScreenASCII::draw_pixel(Point&pt) {
    pixels[pt.x][pt.y] = m_char_color;
}

void ScreenASCII::draw_line(Point&p1, Point&p2) {
    //SDL_RenderDrawLine(renderer,(int)p1.x,(int)p1.y,(int)p2.x,(int)p2.y);
}

void ScreenASCII::draw_poly(std::vector<Point> points) {
    //std::vector<Sint16> x_coords;
    //std::vector<Sint16> y_coords;
    //for(auto pt: points) {
    //    x_coords.push_back((Sint16)pt.x);
    //    y_coords.push_back((Sint16)pt.y);
    //}
    //filledPolygonRGBA(renderer,&x_coords[0],&y_coords[0],x_coords.size(),m_r,m_g,m_b,0xff);
}
