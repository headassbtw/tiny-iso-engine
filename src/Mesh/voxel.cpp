#include <Mesh/voxel.hpp>
using namespace Mesh;


Mesh::Voxel::Voxel(){

}
Mesh::Voxel::~Voxel(){
    delete position;
    delete scale;
    delete color;
}