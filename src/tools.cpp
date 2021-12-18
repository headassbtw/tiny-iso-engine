#include "tools.hpp"
#include "main.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#define PI 3.14159265
vector2* Tools::iso_axis(int axis, float distance){
        if(axis == 0){
            return new vector2(cos(radians(30)) * distance,sin(radians(30)) * distance);
        }
        else if(axis == 1){
            return new vector2(cos(radians(150)) * distance,sin(radians(150)) * distance);
        }
        else if(axis == 2){
            return new vector2(0,distance);
        }
        else{
            return new vector2(0,0);
        }
    }


float Tools::radians(float deg){
    return deg * (PI/180);
}
float Tools::degrees(float rad){
    return rad * (180/PI);
}
vector2* Tools::iso_projection(vector3* point){
    vector2* xadd = Tools::iso_axis(0, point->x);
    vector2* yadd = Tools::iso_axis(1, point->y);
    vector2* zadd = Tools::iso_axis(2, point->z);
    vector2* rtn = new vector2(xadd->x + yadd->x + zadd->x, xadd->y+yadd->y+zadd->y);
    delete xadd;
    delete yadd;
    delete zadd;
    return rtn;
}

float Tools::lerp(float a, float b, float t)
{
    return a + t * (b - a);
}


vector3* Tools::lerp(vector3* start, vector3* end, float t){
    float x = Tools::lerp(start->x, end->x, t);
    float y = Tools::lerp(start->y, end->y, t);
    float z = Tools::lerp(start->z, end->z, t);
    return new vector3(x,y,z);
}

bool Tools::CompareVector3Distance(Face* a, Face* b){
    vector3* a1 = Tools::rotate(a->position, Globals::rotation);
    vector3* b1 = Tools::rotate(b->position, Globals::rotation);


    vector3* aa = a1->add(new vector3(100,100,-100));
    vector3* bb = b1->add(new vector3(100,100,-100));
    bool rt = (aa->normalize() > bb->normalize());
    delete aa;
    delete bb;
    delete a1;
    delete b1;
    

    return rt;
}


vector3* Tools::rotate(vector3* input, vector3* tool){
    //A SQUARED PLUS B SQUARED EQUALS C SQUARED
    float hyp_x = sqrt(((input->y)*(input->y)) + ((input->z)*(input->z)));
    float hyp_y = sqrt(((input->x)*(input->x)) + ((input->z)*(input->z)));
    float hyp_z = sqrt(((input->x)*(input->x)) + ((input->y)*(input->y)));
    

    //float xang_p = atan(input->z/input->y);
    float xang = tool->x;
    float xrad = radians(xang);
    float xcos = cos(xrad);
    float xsin = sin(xrad);
    
    //float yang_p = atan(input->z/input->x);
    float yang = tool->y;
    float yrad = radians(yang);
    float ycos = cos(yrad);
    float ysin = sin(yrad);

    //float zang_p = atan(input->y/input->x);
    float zang = tool->z;
    float zrad = radians(zang);
    float zcos = cos(zrad);
    float zsin = sin(zrad);


    float xoff_z = (input->x * zcos) - (input->y * zsin);
    float yoff_z = (input->x * zsin) + (input->y * zcos);

    float zoff_x = (yoff_z * xsin) + (input->z * xcos);
    float yoff_x = (yoff_z * xcos) - (input->z * xsin);

    


    float xpos = xoff_z;
    float ypos = yoff_x;
    float zpos = zoff_x;
    return new vector3(xpos,ypos,zpos);
}

Uint32 Tools::getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

switch (bpp)
{
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
      }
}

SDL_Color Tools::getcolor(SDL_Surface *surface,Uint32 pixel){
    SDL_Color rgb;
    Uint32 data = getpixel(surface, 200, 200);
    SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);
    return rgb;
}

void Tools::PadNum(std::string &str, const size_t num)
{
    if(num > str.size())
        str.insert(0, num - str.size(), '0');
}