/*
    COPYRIGHT (C) Linus Ericson 2024
    Email: ljlericson@protonmail.com

    this header file is licensed by the creative commons attribution
    non-commercial-noderivatrives 4.0 international license

    License: CC BY-NC-ND 4.0
    License link: https://creativecommons.org/licenses/by-nc-nd/4.0/deed.en

    NOTICES: 
    1. namespace and all emuns are named under 'engine' not 'S3DL'
    2. S3DL uses the SDL2 libraries and cannot work without them installed
       SDL2 libraries include:
        - SDL2 
        - SDL2 image
        - SDL2 mixer
        - SDL2 net
    3. I hope you have realised by this point but this is a C++ library, this
       library does not work natively with C, nor do I intend to release a C
       port going into the future, sorry for any inconveiniance (not really)

    please visit (s3dl.com) for more info or (s3dl.com/wiki.html) for the 
    wiki page (if the website is down I may not have made them yet haha)

    compile command (only for GNU and mingw, not for clang or others):
    g++ example.cpp -lmingw32 -lSDL2 -llibSDL2_image -llibSDL2_net -llibSDL2_ttf -o example.exe
    (replace example with you project name)
*/

// All the libraries that are required 
#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <list>

// macros
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (y))

#ifdef ENGINE_ENABLE_DEBUG_PRINTS
#define LOG(x) std::cout << x << '\n'
#else
#define LOG(x)
#endif

const int pi = 3.14159265359f;

// declaring structs 

struct vector3
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

    bool operator==(vector3 v);
};

struct triLens
{
    float w, h;
};

struct vector2
{
    float x;
    float y;
};

struct quad
{
    vector3 p[5];
    SDL_Color col;
};

struct triangle
{
    vector3 p[3];
    SDL_Color col;
};

struct rotate
{
    bool rot;
    char dir;
    float step;

    float object_rot_speed;
};

struct mesh
{
    rotate r;
    vector3 pos;
    std::vector<triangle> tris;
};

struct mat4x4
{
    float m[4][4] = {0};
};


struct colour
{
    float r;
    float g;
    float b;
    float a;
};

struct directionAndCollision
{
    char direction;
    bool collision;
};

// basic template, networking may be coming out in
// a future library version, but for now you're on
// your own

struct packet
{
    vector3 pos;
    const char* objectType;
    int status;
};

// main namespace, note that class object and class
// camera are not included

namespace s3dl
{
        // algorithm to find the number that is not in the squence.
    // Sequence must start at 0 and must add 1 each elemet for
    // alg to work. Returns -1 on failure.

    typedef enum
    {
        // engine enums
        ENGINE_SHOW     = 0,
        ENGINE_HIDE     = 1,
        ENGINE_ROTATE   = 2,
        ENGINE_MOVE     = 3,

        // movement enums
        ENGINE_FOWARD   = 10,
        ENGINE_STRAFE   = 20,
        ENGINE_VERTICAL = 30,

        // rotation enums
        ENGINE_YAW = 100,
        ENGINE_PITCH = 200,
        ENGINE_BANK = 300,

        // actor enums
        ENGINE_ACTORSOUND = -1,
        ENGINE_ACTORLIGHT = -2,

        // command enums
        ENGINE_COMMANDS_ACTIVE = -10,
        ENGINE_COMMANDS_CLOSE = -20,

        // rendering emuns
        ENGINE_RENDERER_SDL2 = -100,
        ENGINE_RENDERER_OPENGL = -200,

    } engine;

    std::string get_vs_src();

    std::string get_fs_src();

    GLuint compileShader(GLuint type, const std::string& src);

    GLuint create_shader(const std::string& vs_src, const std::string& fs_src);

    void drawTriangle(vector3 p1, vector3 p2, vector3 p3, SDL_Color colour, SDL_Renderer* renderer);

    void init(GLuint vao, GLuint vbo);

    void fillMeshGL(const mesh& i, int width, int height, float far, float near, GLuint vao, GLuint vbo, GLuint shaderProgram);

    void fillTriangle(vector3 p[3], SDL_Color col, SDL_Renderer* renderer, SDL_Texture* texture);

    // Inits all required SDL subsystems for S3DL's operations
    void initSDL();

    void initGl();
    
    // Reverses vector of any data type, inputting data type
    // using <> is not required by most compilers
    template<typename t>
    std::vector<t> reverseVector(std::vector<t> i);

    // Loads png file and returns SDL_Texture
    SDL_Texture* loadTexture(std::string filePath, SDL_Renderer* renderTarget);
    
    bool checkParam(float i, int low, int high);

    void switchBool(bool& switchTarget);

    directionAndCollision collisionCheck(int posX, int posY, int checkX1, int checkY1, int checkX2, int checkY2);

    mesh loadOBJ(const char* filePath);

    vector3 matrix_MultiplyVector(vector3 &i, mat4x4 &m);

    mat4x4 matrix_MakeIdentity();

    mat4x4 matrix_MakeTranslation(float x, float y, float z);

    mat4x4 matrix_multiplyMatrix(const mat4x4 &m1, const mat4x4 &m2);

    vector3 calculateNormal(triangle i);

    float dotProduct(vector3 &v1, vector3 &v2);

    vector3 vectorSub(vector3 &v1, vector3 &v2);

    vector3 vectorAdd(const vector3 &v1, const vector3 &v2);

    vector3 vectorMul(vector3 &v1, float k);
    
    vector3 vectorDiv(vector3 &v1, float k);

    float vectorLength(vector3 &v);

    vector3 vectorNormalise(vector3 &v);

    vector3 vectorCrossProduct(vector3 &v1, vector3 &v2);

	vector3 Vector_IntersectPlane(vector3 &plane_p, vector3 &plane_n, vector3 &lineStart, vector3 &lineEnd);

    mat4x4 matrix_PointAt(vector3 &pos, vector3 &target, vector3 &up);

    mat4x4 matrix_QuickInverse(mat4x4 &m);

    mat4x4 matrix_MakeRotationX(float fAngleRad);

	mat4x4 matrix_MakeRotationY(float fAngleRad);

	mat4x4 matrix_MakeRotationZ(float fAngleRad);

    float degreeToRad(const float& i);

    float radToDegree(const float& i);

    inline float absoluteValue(const float& i);

    inline float inverseNum(const float& i);

    // NOT MY CODE!!! (stolen i mean borowed from javidx9)
    int triangle_ClipAgainstPlane(vector3 plane_p, vector3 plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2);

    mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);

    Mix_Chunk* loadWAV(const char* filename);

    void playSound(Mix_Chunk* sound, int volume, int channel, int loops);

    std::string make_lowercase(const std::string& i);

    std::string remove_character(const std::string& i, char search);

    // Parses individual words in a string that are separated by a chosen character
    // \param std::string i string input
    // \param char separating_char character used to separate string put ' ' if unsure
    // \return std::vector<std::string> string vector where elemets are individual
    // words that are sorted in order first appeared 
    std::vector<std::string> split_string(const std::string& i, char separating_char);

    triLens tri_findWidthHeight(float __angle_theta, float __len_hypo);

    float add_degrees(const float& i, const float& degrees);

    float fFind_Max_Vector(const std::vector<float>& i);

    float get_distance(float x1, float y1, float x2, float y2);

    class srcText
    {
    private:
        std::string message;
        TTF_Font* font;
        SDL_Color col;

    public:
        SDL_Rect r { 0, 0, 0, 0 };
        int w = 32;

        void init(SDL_Color col, TTF_Font* font);

        void set_mes(const std::string& mes);

        void present(SDL_Renderer* renderer);
    }; 


    // The engine itself
    class engineState
    {
    protected:
        std::vector<triangle> trisToRender;
        std::vector<mesh> meshesToRender;
        int ticks;
        int counts;
        int fontSize;
        int renderer;
        const char* winTitle;
        bool showFPS = false;
        bool showLines = false;
        bool renderTris = true;
        bool useCommands;
        bool commandsActive;
        bool quitting;
        TTF_Font* commandFont;

    public:
        bool windowFocus;
        bool running = true;

        // init class with constructer
        engineState(const char* windowName, bool useCommands, const char* font_filePath, SDL_Window* window);

        ~engineState();

        void set_renderer(int i);

        int get_renderer();

        void quit();

        // Update engineState in main loop for use of S3DL
        void update(SDL_Window* window, const char* windowName, SDL_Event e, SDL_Renderer* renderer);

        void project(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, float far, float near);

        void project(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, float far, float near, GLuint vao, GLuint vbo, GLuint shaderProgram);

        // for showing fps
        void displayFPS(const int& toggle, SDL_Window* window);

        int getFPS();

        // IMPORTANT!! For engine use only, regular programmers should not use this function.
        // Function adds triangles to render list 
        void __ADD_TO_RENDER_LIST(const std::vector<triangle>& __i, const mesh& __i2);

        void command_box_toggle(const int& toggle);
    };
}


// main classes

class camera
{
protected: 
    float fYaw = 0.0f;
    float fPitch = 0.0f;
    float fBank = 0.0f;

public:
    float fFov;
    bool safeMode = true;
    vector3 cameraPos = {0, 0, 0}; 

    // using function sinse all movements apart from y require math, however
    // cameraPos is still public in the case that a programmer wnats to
    // implement their own movement algorithm 
    void move(int direction, float step);

    void rotate(int type, float degrees);

    // returns fYaw/fPitch because they are private
    float fgetYaw();

    float fgetPitch();
    
    float fgetBank();

    void resetPitch();

    void resetYaw();

    void setYaw(float degrees);
};

class object
{
protected:
    mesh m_object;
    mat4x4 matRotZ, matRotX, matRotY, matProj;
    vector3 normal;
    rotate rotateR;
    const char* name;
    float fThetaX, fThetaZ, fThetaY;
    float degreesX, degreesZ, degreesY;
    float rotateSpeed;
    float light;
    float* pDepthBuffer = nullptr;

public:
    vector3 pos;
    SDL_Color objectColour;

    void init(const mesh& objectImput, const SDL_Color& objectColourInput, const char* objectName, const char* filePath);

    /* Adds the object to the render list of the engine 
    *  \param height Height of screen.
    *  \param width Width of screen.
    *  \param fFar Far view plane for rendering.
    *  \param fNear Near view plane for rendering.
    *  \param engineCam Camera to be used for rendering.
    *  \param lightDirs Vector of vector3s that will be treated as lights when rendering the scene.
    *  \param engineState Current engineState 
    *  \sa object::project
    */
    void addToRenderList(int height, int width, float fFar, float fNear, camera engineCam, const std::vector<vector3>& lightDirs, s3dl::engineState& engineState);
    
    /*
    * Transforms object in differernt ways,
    * \param type Type of transformation, `flags` include:
    * - `s3dl::ENGINE_ROTATE` For rotational translations,
    * - `s3dl::ENGINE_MOVE` For cardinal/normal translations.
    * \param axis Axis for desired translation.
    * - Can be 'x', 'y', or 'z'.
    * \param degrees Degree of translation, such as degree
    * of rotation or degree of movement.
    */
    void transform(int type, char axis, float degrees);
};

class soundActor
{
private:
    int channel;
    Mix_Chunk* audio;

public:
    vector2 pos = { 0.0f, 0.0f };
    int volume = 100;

    void init(int channel, Mix_Chunk* audio, int volume);

    void play(int loops);

    void do_volume(float x, float y);
};