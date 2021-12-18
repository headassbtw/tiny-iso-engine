#include <string>
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>

#pragma once
struct vector2{
    float x;
    float y;
    vector2(float ix, float iy){
        x = ix;
        y = iy;
    }
    vector2* add(vector2* add){
        return new vector2(add->x + x, add->y + y);
    }
    vector2* add(float addX, float addY){
        return new vector2(addX + x, addY + y);
    }
    vector2* round(){
        return new vector2((int)x,(int)y);
    }
    void log(){
        std::string logline = "(" + std::to_string((int)x) + "," + std::to_string((int)y) + ")";
        std::cout << logline << std::endl;
    }
    void logf(){
        std::string logline = "(" + std::to_string(x) + "," + std::to_string(y) + ")";
        std::cout << logline << std::endl;
    }
};

struct vector3{
    float x;
    float y;
    float z;
    vector3(float ix, float iy, float iz){
        x = ix;
        y = iy;
        z = iz;
    }
    vector3* add(vector3* add){
        return new vector3(add->x + x, add->y + y, add->z + z);
    }
    vector3* add(float addX, float addY, float addZ){
        return new vector3(addX + x, addY + y, addZ + z);
    }
    float normalize(){
        float tmp_zplane = sqrt((x*x)+(y*y));
        return sqrt((tmp_zplane*tmp_zplane)+(z*z));   
    }
    void log(){
        std::string logline = "(" + std::to_string((int)x) + "," + std::to_string((int)y) + "," + std::to_string((int)z) + ")";
        std::cout << logline << std::endl;
    }
    void logf(){
        std::string logline = "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";
        std::cout << logline << std::endl;
    }
};

class Face{
    public:
    vector3* scale;
    vector3* position;
    vector3* color;
    Face(float x, float y, float z, vector3* scalein, float r, float g, float b){
        scale = scalein;
        position = new vector3(x,y,z);
        color = new vector3(r,g,b);
    }
};


class Tools{
    public:
    static float radians(float deg);
    static float degrees(float rad);
    static float lerp(float a, float b, float t);
    static vector3* rotate(vector3* input, vector3* tool);
    static vector3* lerp(vector3* start, vector3* end, float t);
    static vector2* iso_axis(int axis, float distance);
    static vector2* iso_projection(vector3* point);
    static bool CompareVector3Distance(Face* a, Face* b);
    static Uint32 getpixel(SDL_Surface *surface, int x, int y);
    static SDL_Color getcolor(SDL_Surface *surface,Uint32 pixel);
    static void PadNum(std::string &str, const size_t num);
};