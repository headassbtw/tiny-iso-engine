#pragma once
#include <tools.hpp>
namespace Mesh{
    struct Voxel{
        vector3* position;
        vector3* scale;
        vector3* color;
        Voxel();
        ~Voxel();
    };
}