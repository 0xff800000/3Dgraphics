#include <SDL2/SDL.h>

#include "screen_sdl.hpp"

#include "SDL2_gfxPrimitives.h" // TODO: remove dependency


ScreenSDL::ScreenSDL(SDL_Renderer* renderer, int width, int height)
    : renderer (renderer)
    , width (width)
    , height (height)
{
    if(!renderer || !width || !height)
        throw;
    m_r=0; m_g=0; m_b=0; m_a=0;
}

void ScreenSDL::clear() {
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,m_r,m_g,m_b,m_a);
}

void ScreenSDL::render(void) {
    SDL_RenderPresent(renderer);
}

void ScreenSDL::set_draw_color(int r, int g, int b, int a) {
    m_r=r; m_g=g; m_b=b; m_a=a;
    SDL_SetRenderDrawColor(renderer,m_r,m_g,m_b,m_a);
}

void ScreenSDL::draw_pixel(Point&pt) {
    SDL_RenderDrawPoint(renderer,(int)pt.x,(int)pt.y);
}

void ScreenSDL::draw_line(Point&p1, Point&p2) {
    SDL_RenderDrawLine(renderer,(int)p1.x,(int)p1.y,(int)p2.x,(int)p2.y);
}

void ScreenSDL::draw_poly(std::vector<Point> points) {
    std::vector<Sint16> x_coords;
    std::vector<Sint16> y_coords;
    for(auto pt: points) {
        x_coords.push_back((Sint16)pt.x);
        y_coords.push_back((Sint16)pt.y);
    }
    filledPolygonRGBA(renderer,&x_coords[0],&y_coords[0],x_coords.size(),m_r,m_g,m_b,0xff);
}
