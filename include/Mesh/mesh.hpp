#pragma once
#include <tools.hpp>
#include <vector>
namespace Mesh{
    struct VoxelMesh{
        vector3* position;
        vector3* rotation;
        vector3* scale;

        VoxelMesh();
        ~VoxelMesh();
    };
}