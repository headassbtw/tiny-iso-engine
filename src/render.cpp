#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include "SDL_keycode.h"
#include "SDL_render.h"
#include <render.hpp>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <algorithm>
#include <main.hpp>
#include <tools.hpp>
#include <iomanip>
#define GLEW_STATIC
#include <GL/glew.h>


int window_width = 512;
int window_height = 512;
static const GLuint WIDTH = 512;
static const GLuint HEIGHT = 512;

SDL_Window* window;
SDL_Surface* surface;
SDL_Renderer* renderer;
std::vector<Face*> faces;


bool done;
bool color;

static const GLchar* vertex_shader_source =
    "#version 120\n"
    "attribute vec2 coord2d;\n"
    "void main() {\n"
    "    gl_Position = vec4(coord2d, 0.0, 1.0);\n"
    "}\n";
static const GLchar* fragment_shader_source =
    "#version 120\n"
    "void main() {\n"
    "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n";


GLuint fragment_shader, vertex_shader;

vector3* Globals::offset;
vector3* Globals::rotation;


void Sort(){
    std::sort(faces.begin(), faces.end(), Tools::CompareVector3Distance);
    std::cout << "sorted faces" << std::endl;
}

GLuint common_get_shader_program(
    const char *vertex_shader_source,
    const char *fragment_shader_source
) {
    GLint log_length, success;
    GLuint  program;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {

    }
    if (!success) {
        printf("vertex shader compile error\n");
        exit(EXIT_FAILURE);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        std::cout << "frag shader died?" << std::endl;
    }
    if (!success) {
        printf("fragment shader compile error\n");
        exit(EXIT_FAILURE);
    }

    /* Link shaders */
    program = glCreateProgram();
    //glAttachShader(program, vertex_shader);
    //glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        std::cout << "uh, fucK?" << std::endl;
    }
    if (!success) {
        printf("shader link error");
        exit(EXIT_FAILURE);
    }

    /* Cleanup. */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}


GLint attribute_coord2d;
GLint attribute_vcol;
GLuint program, vbo;
SDL_GLContext gl_context;

void buffer_refresh(std::vector<GLfloat> verts){
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(attribute_vcol, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool render_hardware = true;
bool render_software = false;
bool wireframe = false;

void Render::init(){
    
    GLfloat view_port[4] = {0};
        glGetFloatv(GL_VIEWPORT, &view_port[0]);
        GLfloat screen_width = view_port[2] - view_port[0];
        GLfloat screen_height = view_port[3] - view_port[1];

        //glMatrixMode(GL_PROJECTION);
        //gluPerspective(45.0, screen_width / screen_height, 0.1f, 1000.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
        glClearDepth(1.0f);                                    // Depth Buffer Setup
        glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
        glDepthFunc(GL_LEQUAL);                                // The Type Of Depth Testing To Do
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculations




    Globals::offset = new vector3(0,0,0);
    Globals::rotation = new vector3(0,0,0);
    faces = std::vector<Face*>();
    
    

    
   //back z faces
    faces.push_back(new Face(0,0,-30,new vector3(60,60,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(0,0,-40,new vector3(80,80,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(0,-35,-30,new vector3(60,10,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(-35,0,-30,new vector3(10,60,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(0,35,40,new vector3(80,10,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(35,0,40,new vector3(10,60,1), 0.0f,0.0f,255.0f));
    //back y faces
    faces.push_back(new Face(0,30,0,new vector3(60,1,60), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(0,40,0,new vector3(80,1,80), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(-35,30,0,new vector3(10,1,60), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(0,30,35,new vector3(60,1,10), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(0,-40,-35,new vector3(80,1,10), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(35,-40,0,new vector3(10,1,60), 0.0f,255.0f,0.0f));
    //back x faces
    faces.push_back(new Face(30,0,0,new vector3(1,60,60), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(40,0,0,new vector3(1,80,80), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(30,-35,0,new vector3(1,10,60), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(30,0,35,new vector3(1,60,10), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(-40,35,0,new vector3(1,10,60), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(-40,0,-35,new vector3(1,80,10), 255.0f,0.0f,0.0f));




    faces.push_back(new Face(-50,0,10,new vector3(80,20,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(-50,0,-10,new vector3(80,20,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(-50,-10,0,new vector3(80,1,20), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(-50,10,0,new vector3(80,1,20), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(-90,0,0,new vector3(1,20,20), 255.0f,0.0f,0.0f));

    faces.push_back(new Face(0,-50,10,new vector3(20,80,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(0,-50,-10,new vector3(20,80,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(-10,-50,0,new vector3(1,80,20), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(10,-50,0,new vector3(1,80,20), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(0,-90,0,new vector3(20,1,20), 0.0f,255.0f,0.0f));

    faces.push_back(new Face(0,0,90,new vector3(20,20,1), 0.0f,0.0f,255.0f));

    faces.push_back(new Face(-10,0,50,new vector3(1,20,80), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(10,0,50,new vector3(1,20,80), 255.0f,0.0f,0.0f));


    faces.push_back(new Face(0,-10,50,new vector3(20,1,80), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(0,10,50,new vector3(20,1,80), 0.0f,255.0f,0.0f));





    //front x faces

    faces.push_back(new Face(-40,0,35,new vector3(1,80,10), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(-35,0,35,new vector3(1,60,10), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(-40,-35,0,new vector3(1,10,60), 255.0f,0.0f,0.0f));
    faces.push_back(new Face(-35,-35,0,new vector3(1,10,60), 255.0f,0.0f,0.0f));

    //front y faces

    faces.push_back(new Face(0,-40,35,new vector3(80,1,10), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(0,-35,35,new vector3(60,1,10), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(-35,-40,0,new vector3(10,1,60), 0.0f,255.0f,0.0f));
    faces.push_back(new Face(-35,-35,0,new vector3(10,1,60), 0.0f,255.0f,0.0f));

    //top z faces
    faces.push_back(new Face(0,-35,40,new vector3(80,10,1), 0.0f,0.0f,255.0f));
    faces.push_back(new Face(-35,0,40,new vector3(10,60,1), 0.0f,0.0f,255.0f));
    
    

    Sort();
    
    
    done = false;
    window = NULL;
    surface = NULL;
    renderer = NULL;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "fuck: %s\n", SDL_GetError());
    }

    SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer);
    if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    }
    gl_context = SDL_GL_CreateContext(window);
    glewInit();
    
    program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    attribute_coord2d = glGetAttribLocation(program, "coord2d");
    attribute_vcol = glGetAttribLocation(program, "v_color");
    glUseProgram(program);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    renderer = SDL_GetRenderer(window);
    SDL_SetWindowTitle(window, "yeet");
    surface = SDL_GetWindowSurface(window);
    color = false;



    bool rendermode = true;
    int lo = 0;
    //while(!done){
    for(int iii = 0; iii < 2; iii++){
    for(int lo = 0; lo < 360; lo++){
        auto start = std::chrono::high_resolution_clock::now();
        Render::update_sdl();
        if(render_software)
            Render::draw_sdl();
        if(render_hardware)
            Render::draw_gl();

        if(rendermode){
            SDL_Surface *sshot = SDL_CreateRGBSurface(0, window_width, window_height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
            SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
            std::string num = std::to_string(lo + (360*iii));
            Tools::PadNum(num, 4);
            std::string imgname = std::string("/home/headass/CLionProjects/iso_sdl/ss/screenshot" + num + ".bmp");
            SDL_SaveBMP(sshot, imgname.c_str());
            SDL_FreeSurface(sshot);
            Globals::rotation->z += 1;
            Globals::rotation->x += 1;
            Sort();
        }

        else {
            auto finish = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
            float DeltaTime = ms.count();

            //if(DeltaTime < 14.4927536232f){
            //    SDL_Delay(14.4927536232f - DeltaTime);
            //}

            auto finish2 = std::chrono::high_resolution_clock::now();
            auto ms2 = std::chrono::duration_cast<std::chrono::microseconds>(finish2-start);
            float DeltaTime2 = ms2.count();

            std::string fps = std::string(std::to_string((int)(1000000.0f/DeltaTime2)) + " FPS");
            std::string title = std::string("Yeet | " + fps);
            
            SDL_SetWindowTitle(window, title.c_str());
        }

    }
    wireframe = true;
    }
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    if(rendermode){
        system("ffmpeg -r 60 -f image2 -s 480x360 -i ./ss/screenshot%04d.bmp -vcodec libx264 -crf 15  -pix_fmt yuv420p test.mp4");
    }
    
}


void Render::line_3D(vector3* start, vector3* end){
    for(float i = 0; i < 80; i++){
        vector3* lerpdpos = Tools::lerp(start,end, (i/80));
        vector3* whenyoudrawit = lerpdpos->add(Globals::offset);
        vector2* draw11 = Tools::iso_projection(whenyoudrawit);
        vector2* draw12 = draw11->add(240,180);
        SDL_RenderDrawPoint(renderer, draw12->x, draw12->y);
        delete draw11;
        delete draw12;
        delete lerpdpos;
        delete whenyoudrawit;
    }
}

vector3* lastrot = NULL;
struct v2intcompare
{
    vector2* key;
    v2intcompare(vector2* const &i): key(i) {}
 
    bool operator()(vector2* const &i) {
        return (((int)key->x == (int)i->x) && ((int)key->y == (int)i->y));
    }
};
static void isoplane_beta(){
    

    

    for(int i = 0; i < faces.size(); i++){
        Face* face = faces[i];
        SDL_SetRenderDrawColor(renderer, face->color->x, face->color->y, face->color->z, SDL_ALPHA_OPAQUE);
        //SDL_RenderDrawLine(renderer, 240, 0, 240, 360);
        float bodyz = (face->scale->z <= 0) ? 1 : face->scale->z;
        //the "scale" property of faces determines width/height, but making it a vec3 allows for one variable

        float sc = 1.3f;
        std::vector<vector2*> pixels;
        for(float j = 0; j < face->scale->z*sc; j++){
            float zpos = ((face->position->z) + (j/sc) - (face->scale->z/2));
            for(float k = 0; k < face->scale->y*sc; k++){
                float ypos = ((face->position->y) + (k/sc) - (face->scale->y/2));
                for(float l = 0; l < face->scale->x*sc; l++){
                    float xpos = ((face->position->x) + (l/sc) - (face->scale->x/2));


                    vector3* a = new vector3(xpos,ypos,zpos);
                    vector3* rot = Tools::rotate(a, Globals::rotation);
                    vector3* whenyoudrawit = rot->add(Globals::offset);
                    
                    vector2* draw11 = Tools::iso_projection(whenyoudrawit);
                    vector2* draw12 = draw11->add(240,180);

                    //if(std::find(pixels.begin(), pixels.end(), draw12->round())== pixels.end()){
                        SDL_RenderDrawPointF(renderer, draw12->x, draw12->y);
                    //    pixels.push_back(draw12->round());
                    //}
                    delete draw11;
                    delete draw12;
                    delete a;
                    delete rot;
                    delete whenyoudrawit;

                }
            }
        }
    }
}



void Render::update_sdl(){
    SDL_Event event;
        while (SDL_PollEvent(&event))
        {      
            switch(event.type){
                case SDL_QUIT:
                    done = SDL_TRUE;
                break;
                case SDL_KEYDOWN:
                std::string posLog = "(" + std::to_string(Globals::offset->x) + "," + std::to_string(Globals::offset->y) + "," + std::to_string(Globals::offset->z) + ")";
                    std::cout << posLog << std::endl;
                    switch(event.key.keysym.sym){
                        //position
                        case SDL_KeyCode::SDLK_UP:
                        Globals::offset->y -= 20;
                        break;
                        case SDL_KeyCode::SDLK_DOWN:
                        Globals::offset->y += 20;
                        break;
                        case SDL_KeyCode::SDLK_LEFT:
                        Globals::offset->x -= 20;
                        break;
                        case SDL_KeyCode::SDLK_RIGHT:
                        Globals::offset->x += 20;
                        break;
                        case SDL_KeyCode::SDLK_PAGEUP:
                        Globals::offset->z += 20;
                        Sort();
                        break;
                        case SDL_KeyCode::SDLK_PAGEDOWN:
                        Globals::offset->z -= 20;
                        Sort();
                        break;

                        //rotation
                        case SDL_KeyCode::SDLK_w:
                        Globals::rotation->y -= 10;
                        Sort();
                        break;
                        case SDL_KeyCode::SDLK_s:
                        Globals::rotation->y += 10;
                        Sort();
                        break;
                        case SDL_KeyCode::SDLK_a:
                        Globals::rotation->x -= 10;
                        Sort();
                        break;
                        case SDL_KeyCode::SDLK_d:
                        Globals::rotation->x += 10;
                        Sort();
                        break;
                        case SDL_KeyCode::SDLK_q:
                        Globals::rotation->z += 10;
                        Sort();
                        break;
                        case SDL_KeyCode::SDLK_e:
                        Globals::rotation->z -= 10;
                        Sort();
                        break;


                        case SDL_KeyCode::SDLK_z:
                        wireframe = false;
                        break;
                        case SDL_KeyCode::SDLK_x:
                        wireframe = true;
                        break;



                    }
                break;
            }
        }
}



void Render::draw_sdl(){
    color = !color;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    //these seem likee they're needed, but when i remove them, it stops memory leaking???????
    SDL_RenderClear(renderer);
    //SDL_UpdateWindowSurface(window);
    isoplane_beta();


    SDL_RenderPresent(renderer);
    //SDL_Delay(25);
}




float l = 0.0f;
void Render::draw_gl(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);


    float x1 = cos(l)*0.5;
    float y1 = sin(l)*0.5;
    
    float x2 = cos(l+2)*0.5;
    float y2 = sin(l+2)*0.5;

    float x3 = cos(l+4)*0.5;
    float y3 = sin(l+4)*0.5;

    std::vector<GLfloat> verts3;
    std::vector<GLfloat> cols3;

    for(int i = 0; i < faces.size(); i++){
        Face* face = faces[i];


        float sc = 2;

        float x_top    = face->position->x + (face->scale->x / 2);
        float y_top    = face->position->y + (face->scale->y / 2);
        float z_top    = face->position->z + (face->scale->z / 2);

        float x_bottom = face->position->x - (face->scale->x / 2);
        float y_bottom = face->position->y - (face->scale->y / 2);
        float z_bottom = face->position->z - (face->scale->z / 2);

        vector3* tl = new vector3(0, 0, 0);
        vector3* tr = new vector3(0, 0, 0);
        vector3* bl = new vector3(0, 0, 0);
        vector3* br = new vector3(0, 0, 0);
        for(int i = 0; i < 6;i++){
            cols3.push_back(face->color->x / 255.0f);
            cols3.push_back(face->color->y / 255.0f);
            cols3.push_back(face->color->z / 255.0f);
        }

        if(face->scale->x == 1){
            tl = new vector3(face->position->x, y_bottom, z_top);
            tr = new vector3(face->position->x, y_top, z_top);
            bl = new vector3(face->position->x, y_bottom, z_bottom);
            br = new vector3(face->position->x, y_top, z_bottom);
        }
        if(face->scale->y == 1){
            tl = new vector3(x_top, face->position->y,z_bottom);
            tr = new vector3(x_top, face->position->y,z_top);
            bl = new vector3(x_bottom, face->position->y,z_bottom);
            br = new vector3(x_bottom, face->position->y,z_top);
        }
        if(face->scale->z == 1){
            tl = new vector3(y_bottom,x_bottom,face->position->z);
            tr = new vector3(y_top,x_bottom,face->position->z);
            bl = new vector3(y_bottom,x_top,face->position->z);
            br = new vector3(y_top,x_top,face->position->z);
        }

        vector3* tl2 = Tools::rotate(tl, Globals::rotation);
        vector3* tr2 = Tools::rotate(tr, Globals::rotation);
        vector3* bl2 = Tools::rotate(bl, Globals::rotation);
        vector3* br2 = Tools::rotate(br, Globals::rotation);
        

        vector2* tl3 = Tools::iso_projection(tl2);
        vector2* tr3 = Tools::iso_projection(tr2);
        vector2* bl3 = Tools::iso_projection(bl2);
        vector2* br3 = Tools::iso_projection(br2);


        float mult = 0.01f;

        verts3.push_back(tl3->x*mult);
        verts3.push_back(tl3->y*mult);
        verts3.push_back(tr3->x*mult);
        verts3.push_back(tr3->y*mult);
        verts3.push_back(bl3->x*mult);
        verts3.push_back(bl3->y*mult);

        verts3.push_back(br3->x*mult);
        verts3.push_back(br3->y*mult);
        verts3.push_back(tr3->x*mult);
        verts3.push_back(tr3->y*mult);
        verts3.push_back(bl3->x*mult);
        verts3.push_back(bl3->y*mult);
    }


    
    /** Setup Drawing Triangles **/
    GLfloat normals[] = {
        0, 0, 1,   0, 0, 1,   0, 0, 1,
        0, 0, 1,   0, 0, 1,   0, 0, 1
    };



    

    //Draw both triangles in a single call..
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normals);
    glColorPointer(3, GL_FLOAT, 0, cols3.data());
    glVertexPointer(2, GL_FLOAT, 0, verts3.data());

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, verts3.size()/2); //6 vertices (X, Y, Z) for 2 triangles..
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    l+= 0.01f;
    SDL_GL_SwapWindow(window);
}