cmake_minimum_required(VERSION 3.20)
project(iso_sdl)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake_Modules/")
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(include)
include_directories(include/IO)
include_directories(include/Mesh)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_image_INCLUDE_DIRS})

file(GLOB SRC
    "src/*.cpp"
    "src/IO/*.cpp"
    "src/Mesh/*.cpp"
)


add_executable(iso_sdl ${SRC})

target_link_libraries(iso_sdl OpenGL GLEW SDL2_image ${SDL2_image_LIBRARIES} ${SDL2_LIBRARIES})
