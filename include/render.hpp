#include "tools.hpp"
#include <vector>
#include <SDL2/SDL.h>

#pragma once
class Render{
    
    public:
    static void update_sdl();
    static void draw_sdl();
    static void draw_gl();
    static void line_3D(vector3* start, vector3* end);
    static void init();
};

