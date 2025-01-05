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

#define ENGINE_ENABLE_DEBUG_PRINTS

// macros
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (y))

#ifdef ENGINE_ENABLE_DEBUG_PRINTS
#define LOG(x) std::cout << x << '\n'
#else
#define LOG(x)
#endif

// declaring enums
const int pi = 3.14159265359f;

// declaring functions needed for structs

namespace S3DL
{
    int findNumberNotThere(std::vector<int> in, int actualSize)
    {
        for(int i = 0; i < in.size(); i++)
            if(in[i] != i) {return i;}
        return -1;
    }
}

// declaring structs 

struct vector3
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

    bool operator==(vector3 v)
    {
        if(v.x == x && v.y == y && v.z == z)
            return true;
        else 
            return false;
    }
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

    // Finds same points and merges into quad
    quad operator+(triangle t)
    {
        quad out;
        int count;
        std::vector<int> flaggedNum;
        std::vector<int> flaggedNum2;

        for(int i = 0; i < 3; i++)
        {
            if(t.p[0] == p[i])
            {
                count++;
                flaggedNum.push_back(0);
                flaggedNum2.push_back(i);
            }
        }
        for(int i = 0; i < 3; i++)
        {
            if(t.p[1] == p[i])
            {
                count++;
                flaggedNum.push_back(1);
                flaggedNum2.push_back(i);
            }
        }
        for(int i = 0; i < 3; i++)
        {
            if(t.p[2] == p[i])
            {
                count++;
                flaggedNum.push_back(2);
                flaggedNum2.push_back(i);
            }
        }
        
        if(count == 2)
        {
            if((flaggedNum[0] == 1 && flaggedNum[1] == 2) 
            || (flaggedNum[1] == 1 && flaggedNum[0] == 2))
            {
                out.p[0] = t.p[0];
                out.p[1] = t.p[2];
                out.p[2] = t.p[1];
                out.p[3] = p[S3DL::findNumberNotThere(flaggedNum2, 3)];
            }
            if((flaggedNum[0] == 2 && flaggedNum[1] == 0)
            ||(flaggedNum[1] == 2 && flaggedNum[0] == 0))
            {
                out.p[0] = t.p[0];
                out.p[1] = t.p[1];
                out.p[2] = t.p[2];
                out.p[3] = p[S3DL::findNumberNotThere(flaggedNum2, 3)];
            }
            if((flaggedNum[0] == 1 && flaggedNum[1] == 0)
            ||(flaggedNum[1] == 1 && flaggedNum[0] == 0))
            {
                out.p[0] = t.p[0];
                out.p[1] = t.p[1];
                out.p[2] = t.p[2];
                out.p[3] = p[S3DL::findNumberNotThere(flaggedNum2, 3)];
            }
        }
        out.col = col;
        return out;
    }
};

struct rotate
{
    bool rot;
    char dir;
    float step;
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

namespace S3DL
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

    std::string get_vs_src()
    {
        return R"(
        #version 330 core

        // Input attribute from the vertex buffer
        layout(location = 0) in vec3 position;  // Ensure the correct data type and location

        // Uniform for the projection matrix
        uniform mat4 projection;
        uniform mat4 modelMatLocation;

        void main() {
            // Apply the projection matrix to the vertex position
            vec4 newPosition = projection * modelMatLocation * vec4(position, 1.0f);
            gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
        }
        )"; 
    }

    std::string get_fs_src()
    {
        return R"(
        #version 330 core
        out vec4 FragColor;
        void main() 
        {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
        )";
    }

    GLuint compileShader(GLuint type, const std::string& src) 
    {
        GLuint id = glCreateShader(type);
        const char* src_c = src.c_str();
        glShaderSource(id, 1, &src_c, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE)
        {
            int len;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
            char* mes = (char*)malloc(len * sizeof(char));
            glGetShaderInfoLog(id, len, &len, mes);
            std::cout << "[ERROR]: Shader of type \"" << (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << "\" failed to compile, " << mes << '\n';
            glDeleteShader(id);
            std::cout << "Press entre to exit...";
            std::cin.get();
            throw;
        }

        return id; 
    }

    GLuint create_shader(const std::string& vs_src, const std::string& fs_src)
    {
        GLuint program = glCreateProgram();
        GLuint vs = compileShader(GL_VERTEX_SHADER, vs_src);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fs_src);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glValidateProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    void drawTriangle(vector3 p1, vector3 p2, vector3 p3, SDL_Color colour, SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawColor(renderer, colour.a, colour.b, colour.g, colour.r);
        SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
        SDL_RenderDrawLineF(renderer, p2.x, p2.y, p3.x, p3.y);
        SDL_RenderDrawLineF(renderer, p1.x, p1.y, p3.x, p3.y);
    }

    void init(GLuint vao, GLuint vbo)
    {
        float data[9] = {
            -0.5f, -0.5f, -0.5f,
            0.0f, 0.5f, -0.5f,
            0.5f, -0.5f, -0.5f
        };
            // Bind VAO
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    }

    void fillMeshGL(const mesh& i, int width, int height, float far, float near, GLuint vao, GLuint vbo, GLuint shaderProgram)
    {
        // Clear the buffer to a specific value
        // Clear the buffer (reallocate with no data)
        // for 3D
        float data[i.tris.size() * 9];
        for(int j = 0; j < i.tris.size(); j++)
        {
            data[(j * 9)] = i.tris[j].p[0].x;
            data[(j * 9) + 1] = i.tris[j].p[0].y;
            data[(j * 9) + 2] = i.tris[j].p[0].z;

            data[(j * 9) + 3] = i.tris[j].p[1].x;
            data[(j * 9) + 4] = i.tris[j].p[1].y;
            data[(j * 9) + 5] = i.tris[j].p[1].z;

            data[(j * 9) + 6] = i.tris[j].p[2].x;
            data[(j * 9) + 7] = i.tris[j].p[2].y;
            data[(j * 9) + 8] = i.tris[j].p[2].z;
        }
        
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(i.pos.x, i.pos.y, i.pos.z));
        GLint modelMatLocation = glGetUniformLocation(shaderProgram, "modelMatLocation");
        glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, glm::value_ptr(translate));

        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, near, far);
        GLint projLocation = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));

        GLint colorLocation = glGetUniformLocation(shaderProgram, "inputColor");
        // glUniform4f(colorLocation, i[0].col.r / 255.0f, i[0].col.g / 255.0f, i[0].col.b / 255.0f, i[0].col.a / 255.0f);

        // Update the buffer with new vertex data
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
        glDrawArrays(GL_TRIANGLES, 0, i.tris.size() * 3);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void fillTriangle(vector3 p[3], SDL_Color col, SDL_Renderer* renderer, SDL_Texture* texture)
    {
        const SDL_Vertex v[3] = {{{p[0].x, p[0].y}, col, 0}, {{p[1].x, p[1].y}, col, 0}, {{p[2].x, p[2].y}, col, 0}};
        SDL_RenderGeometry(renderer, texture, v, 3, nullptr, 0);
    }

    std::vector<quad> mergeIdenticalTris(std::vector<triangle> in)
    {
        std::vector<quad> mergedTris;
        for(int i = 0; i < in.size(); i++)
        {
            if(in[i].p == in[i - 1].p)
            {
                mergedTris.push_back(in[i] + in[i - 1]);
            }
        }
        return mergedTris;
    }

    void fillTriangleBatch(std::vector<triangle> tris, SDL_Renderer* renderer, SDL_Texture* texture)
    {
        std::vector<quad> quads = mergeIdenticalTris(tris);
        for(auto& quad : quads)
        {
            const SDL_Vertex v[4] = {{{quad.p[0].x, quad.p[0].y}, quad.col, 0}, 
                                     {{quad.p[1].x, quad.p[1].y}, quad.col, 0}, 
                                     {{quad.p[2].x, quad.p[2].y}, quad.col, 0},
                                     {{quad.p[3].x, quad.p[3].y}, quad.col, 0}};
            SDL_RenderGeometry(renderer, texture, v, 3, nullptr, 0);
        }
    }

    // Inits all required SDL subsystems for S3DL's operations
    void initSDL()
    {
        if(SDL_Init(SDL_INIT_EVENTS) == -1)
            std::cerr << "SDL events failed to init, error: " << SDL_GetError() << '\n';
        
        if(SDL_Init(SDL_INIT_TIMER) == -1)
            std::cerr << "SDL timer failed to init, error: " << SDL_GetError() << '\n';
        
        if(SDL_Init(SDL_INIT_VIDEO) == -1)
            std::cerr << "SDL video failed to init, error: " << SDL_GetError() << '\n';
        
        if(IMG_Init(IMG_INIT_PNG) == -1)
            std::cerr << "IMG png failed to init, error: " << SDL_GetError() << '\n';

        if(Mix_Init(MIX_INIT_MP3) == -1)
            std::cerr << "MIX mp3 failed to init, error: " << SDL_GetError() << '\n';

        if(SDLNet_Init() == -1)
            std::cerr << "SDL net failed to init, error: " << SDL_GetError() << '\n';

        if(TTF_Init() == -1)
            std::cerr << "TTF failed to init, error: " << TTF_GetError() << '\n';
    }

    void initGl()
    {
        if(glewInit() != 0)
            std::cerr << "GLFW failed to init, error: " << glGetError() << '\n';

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    }
    
    // Reverses vector of any data type, inputting data type
    // using <> is not required by most compilers
    template<typename t>
    std::vector<t> reverseVector(std::vector<t> i)
    {
        std::vector<t> o;
        // simple for loop that iterates through input vector
        // and adds reverse to output vector
        for(int in = 0; in < i.size(); in++)
            o.push_back(i[i.size() - in - 1]);

        return o;
    }

    // Loads png file and returns SDL_Texture
    SDL_Texture* loadTexture(std::string filePath, SDL_Renderer* renderTarget)
    {
        SDL_Texture* texture1 = nullptr;
        SDL_Surface* character = IMG_Load(filePath.c_str());

        if(character == NULL)
            std::cout << "[ERROR]: SDL2 Could not load character image: " << IMG_GetError() << "\n";
        
        texture1 = SDL_CreateTextureFromSurface(renderTarget, character);

        if(texture1 == NULL)
            std::cout << "[ERROR]: SDL2 Could not convert character texture to surface, Error: " << IMG_GetError() << "\n";

        return texture1;
    }
    
    bool checkParam(float variableToBeChecked, int lowParam, int highParam)
    {
        if(variableToBeChecked < lowParam || variableToBeChecked > highParam)
        {
            return false;
        }
        return true;
    }

    void switchBool(bool switchTarget)
    {
        if(switchTarget)
            switchTarget = false;
        else if(!switchTarget)
            switchTarget = true;
    }

    directionAndCollision collisionCheck(int posX, int posY, int checkX1, int checkY1, int checkX2, int checkY2)
    {
        directionAndCollision collisionDirection;

        if((checkY1 >= posY && checkY2 <= posY) && (checkX1 <= posX && checkX2 >= posX))
            collisionDirection.collision = true;
        else
            collisionDirection.collision = false;

        if(collisionDirection.collision)
        {
            if(posX <= checkX1)                      // checking facing right
                collisionDirection.direction = 'R';

            else if(posX <= checkX2)                 // checking facing left
                collisionDirection.direction = 'L';

            if(posY >= checkY1)                      // checking facing down
                collisionDirection.direction = 'D';

            else if(posY <= checkY2)                 // checking faceing up
                collisionDirection.direction = 'U';

            return collisionDirection;
        }

        return collisionDirection;
    }

    mesh loadOBJ(const char* filePath)
    {
        int load1 = 0;
        float load2 = 0;
        int progress = 0;
        int tick = 0;
        mesh object;
        std::ifstream file(filePath);

        if(file.fail())
        {
            std::cout << "[ERROR]: (loadObj) file " << filePath << " is invalid\n";
        }
        if(!file.fail())
        {
            char c[10];
            std::stringstream s;

            file.getline(c, 10);
            s << c;
            s >> load1;
            
            std::vector<vector3> verts;

            while(!file.eof())
            {
                char line[128];
                char junk;
                file.getline(line, 128);

                if(line == NULL)
                {
                    std::cout << "[ERROR]: Could not load object, object file was invalid";
                    break;
                }

                std::stringstream str;

                str << line;

                if(line[0] == 'v')
                {
                    vector3 v;
                    str >> junk >> v.x >> v.y >> v.z;
                    verts.push_back(v);
                }
                if(line[0] == 'f')
                {
                    int f[3];
                    str >> junk >> f[0] >> f[1] >> f[2];
                    object.tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});
                }

                progress = load2 * (100.0f / load1);

                if(tick == 10)
                    std::cout << "[INFO]: Loading file " << "\"" << filePath << "\"" << ", progress: " << progress << "%\n";

                load2 ++;

                if(tick > 10)
                    tick = 0;
                else
                    tick ++;
            }
        }

        return object;
    }

    	vector3 matrix_MultiplyVector(vector3 &i, mat4x4 &m)
	{
        vector3 o;
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
		o.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];      
        return o; 
	}

    mat4x4 matrix_MakeIdentity()
    {
        mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4x4 matrix_MakeTranslation(float x, float y, float z)
    {
        mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        matrix.m[3][0] = x;
        matrix.m[3][1] = y;
        matrix.m[3][2] = z;
        return matrix;
    }

    mat4x4 matrix_multiplyMatrix(const mat4x4 &m1, const mat4x4 &m2)
    {
        mat4x4 matrix;
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                matrix.m[j][i] = m1.m[j][0] * m2.m[0][i] + m1.m[j][1] * m2.m[1][i] + m1.m[j][2] * m2.m[2][i] + m1.m[j][3] * m2.m[3][i];
        return matrix;
    }

    vector3 calculateNormal(triangle i)
    {
        vector3 o1, o2, o;

        o1.x = i.p[1].x - i.p[0].x;
        o1.y = i.p[1].y - i.p[0].y;
        o1.z = i.p[1].z - i.p[0].z;

        o2.x = i.p[2].x - i.p[0].x;
        o2.y = i.p[2].y - i.p[0].y;
        o2.z = i.p[2].z - i.p[0].z;

        o.x = o1.y * o2.z - o1.z * o2.y;
        o.y = o1.z * o2.x - o1.x * o2.z;
        o.z = o1.x * o2.y - o1.y * o2.x;

        float l = sqrtf(o.x * o.x + o.y * o.y + o.z * o.z);
        o.x /= l; 
        o.y /= l; 
        o.z /= l;

        return o;
    }

    float dotProduct(vector3 &v1, vector3 &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    vector3 vectorSub(vector3 &v1, vector3 &v2)
    {
        return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    }

    vector3 vectorAdd(const vector3 &v1, const vector3 &v2)
	{
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}

    vector3 vectorMul(vector3 &v1, float k)
    {
        return {v1.x * k, v1.y * k, v1.z * k};
    }
    
    vector3 vectorDiv(vector3 &v1, float k)
    {
        return {v1.x / k, v1.y / k, v1.z / k};
    }

    float vectorLength(vector3 &v)
    {
        return sqrtf(dotProduct(v, v));
    }

    vector3 vectorNormalise(vector3 &v)
    {
        float l = vectorLength(v);
        return {v.x / l, v.y / l, v.z / l};
    }

    vector3 vectorCrossProduct(vector3 &v1, vector3 &v2)
	{
		vector3 v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}

	vector3 Vector_IntersectPlane(vector3 &plane_p, vector3 &plane_n, vector3 &lineStart, vector3 &lineEnd)
	{
		plane_n = vectorNormalise(plane_n);
		float plane_d = dotProduct(plane_n, plane_p);
		float ad = dotProduct(lineStart, plane_n);
		float bd = dotProduct(lineEnd, plane_n);
		float t = (-plane_d - ad) / (bd - ad);
		vector3 lineStartToEnd = vectorSub(lineEnd, lineStart);
		vector3 lineToIntersect = vectorMul(lineStartToEnd, t);
		return vectorAdd(lineStart, lineToIntersect);
	}

    mat4x4 matrix_PointAt(vector3 &pos, vector3 &target, vector3 &up)
    {
		vector3 newForward = vectorSub(target, pos);
		newForward = vectorNormalise(newForward);

		// Calculate new Up direction
		vector3 a = vectorMul(newForward, dotProduct(up, newForward));
		vector3 newUp = vectorSub(up, a);
		newUp = vectorNormalise(newUp);

		// New Right direction is easy, its just cross product
		vector3 newRight = vectorCrossProduct(newUp, newForward);

		// Construct Dimensioning and Translation Matrix	
		mat4x4 matrix;
		matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
		return matrix;
    }

    mat4x4 matrix_QuickInverse(mat4x4 &m) // Only for Rotation/Translation Matrices
	{
		mat4x4 matrix;
		matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

    mat4x4 matrix_MakeRotationX(float fAngleRad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = cosf(fAngleRad);
		matrix.m[1][2] = sinf(fAngleRad);
		matrix.m[2][1] = -sinf(fAngleRad);
		matrix.m[2][2] = cosf(fAngleRad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	mat4x4 matrix_MakeRotationY(float fAngleRad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = cosf(fAngleRad);
		matrix.m[0][2] = sinf(fAngleRad);
		matrix.m[2][0] = -sinf(fAngleRad);
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = cosf(fAngleRad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	mat4x4 matrix_MakeRotationZ(float fAngleRad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = cosf(fAngleRad);
		matrix.m[0][1] = sinf(fAngleRad);
		matrix.m[1][0] = -sinf(fAngleRad);
		matrix.m[1][1] = cosf(fAngleRad);
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

    float degreeToRad(const float& i)
    {
        return (i * (pi / 180.0f));
    }

    float radToDegree(const float& i)
    {
        return (i * (180.0f /pi));
    }

    inline float absoluteValue(const float& i)
    {
        if(i < 0)
            return i * -1.0f;
        else
            return i;
    }

    inline float inverseNum(const float& i)
    {
        return i * -1.0f;
    }

    // NOT MY CODE!!! (stolen i mean borowed from javidx9)
    int triangle_ClipAgainstPlane(vector3 plane_p, vector3 plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2)
	{
		// Make sure plane normal is indeed normal
		plane_n = S3DL::vectorNormalise(plane_n);

		// Return signed shortest distance from point to plane, plane normal must be normalised
		auto dist = [&](vector3 &p)
		{
			vector3 n = S3DL::vectorNormalise(p);
			return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * (p.z * -1.0f) - S3DL::dotProduct(plane_n, plane_p));
		};

		// Create two temporary storage arrays to classify points either side of plane
		// If distance sign is positive, point lies on "inside" of plane
		vector3* inside_points[3];  int nInsidePointCount = 0;
		vector3* outside_points[3]; int nOutsidePointCount = 0;

		// Get signed distance of each point in triangle to plane
		float d0 = dist(in_tri.p[0]);
		float d1 = dist(in_tri.p[1]);
		float d2 = dist(in_tri.p[2]);

		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p[1]; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p[2]; }

		// Now classify triangle points, and break the input triangle into 
		// smaller output triangles if required. There are four possible
		// outcomes...

		if (nInsidePointCount == 0)
		{
			// All points lie on the outside of plane, so clip whole triangle
			// It ceases to exist

			return 0; // No returned triangles are valid
		}
		if (nInsidePointCount == 3)
		{
			// All points lie on the inside of plane, so do nothing
			// and allow the triangle to simply pass through
			out_tri1 = in_tri;

			return 1; // Just the one returned original triangle is valid
		}
		if (nInsidePointCount == 1 && nOutsidePointCount == 2)
		{
			// Triangle should be clipped. As two points lie outside
			// the plane, the triangle simply becomes a smaller triangle

			// The inside point is valid, so keep that...
			out_tri1.p[0] = *inside_points[0];

			// but the two new points are at the locations where the 
			// original sides of the triangle (lines) intersect with the plane
			out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1; // Return the newly formed single triangle
		}
		if (nInsidePointCount == 2 && nOutsidePointCount == 1)
		{
			// Triangle should be clipped. As two points lie inside the plane,
			// the clipped triangle becomes a "quad". Fortunately, we can
			// represent a quad with two new triangles

			// The first triangle consists of the two inside points and a new
			// point determined by the location where one side of the triangle
			// intersects with the plane
			out_tri1.p[0] = *inside_points[0];
			out_tri1.p[1] = *inside_points[1];
			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			// The second triangle is composed of one of he inside points, a
			// new point determined by the intersection of the other side of the 
			// triangle and the plane, and the newly created point above
			out_tri2.p[0] = *inside_points[1];
			out_tri2.p[1] = out_tri1.p[2];
			out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

			return 2; // Return two newly formed triangles which form a quad
		}

        return 0; // this part is mine, kept getting 'control reaches end of non-void function' 
                  // warning and couldn't handle it any more
	}

    mat4x4 Matrix_MakeProjection(const float& fFovDegrees, const float& fAspectRatio, const float& fNear, const float& fFar)
	{
		float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * pi);
		mat4x4 matrix;
		matrix.m[0][0] = fAspectRatio * fFovRad;
		matrix.m[1][1] = fFovRad;
		matrix.m[2][2] = fFar / (fFar - fNear);
		matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matrix.m[2][3] = 1.0f;
		matrix.m[3][3] = 0.0f;
		return matrix;
	}

    Mix_Chunk* loadWAV(const char* filename)
    {
        Mix_Chunk* m = Mix_LoadWAV(filename);

        if(m == NULL)
            std::cout << "[ERROR]: " << filename << " Could not be loaded, Error: \n" << Mix_GetError();

        return m;
    }

    void playSound(Mix_Chunk* sound, const int& volume, const int& channel, const int& loops)
    {
        if(Mix_Playing(2) == 0 || (Mix_Playing(1) == 0 && Mix_Playing(3) == 0))
        {
            Mix_Volume(1, volume);
            Mix_PlayChannel(channel, sound, loops);
        }
    }

    std::string make_lowercase(const std::string& i)
    {
        std::string out;
        for(auto c : i)
        {
            if(c < 'a' && c >= 'A')
                c += 32;
            
            out.push_back(c);
        }
        return out;
    }

    std::string remove_character(const std::string& i, char search)
    {
        std::string out = i;
        for(int j = 0; j < i.size(); j++)
        {
            if(out[j] == search)
            {
                out.erase(out.begin() + j);
                j--;
            }
        }
        return out;
    }

    // Parses individual words in a string that are separated by a chosen character
    // \param std::string i string input
    // \param char separating_char character used to separate string put ' ' if unsure
    // \return std::vector<std::string> string vector where elemets are individual
    // words that are sorted in order first appeared 
    std::vector<std::string> split_string(const std::string& i, char separating_char)
    {
        std::vector<std::string> out;
        // index_f and index_b to keep track of individual word pos
        int index_f, index_b = 0;

        // iterates through the characters in string 'i'
        for(int  j = 0; j < i.length(); j++)
        {
            // updating front index
            index_f = j;
            // checking if i[j] is the separating char or is at the end
            if(i[j] == separating_char || j == i.length() - 1)
            {
                std::string to_go;
                // resizing string due to to_go being freed every 
                // iteration
                to_go.resize(index_f - index_b);

                // copying chars over to new string based on foward and
                // back indexes
                for(int t = 0; t < (index_f - index_b); t++)    
                    to_go[t] = i[t + index_b];

                // adding the last character on in the case it is the 
                // last word in the string
                if(j == i.size() - 1)
                    to_go.push_back(i[j]);

                // adding newly created string to the vector and updating
                // the back index
                out.push_back(to_go);
                index_b = j + 1;
            }
        }
        return out;
    }  

    // IMPORTANT!!! VERY INACURATE, DO NOT USE!
    float tri_findRatio_WidthHeight(const float& __angle_theta)
    {
        float ratioRaw;
        if((cosf(S3DL::degreeToRad(__angle_theta))) == (sinf(S3DL::degreeToRad(__angle_theta))))
            return 0.5f;
        else if((cosf(S3DL::degreeToRad(__angle_theta))) > (sinf(S3DL::degreeToRad(__angle_theta))))
            ratioRaw = (cosf(S3DL::degreeToRad(__angle_theta))) / (sinf(S3DL::degreeToRad(__angle_theta)));
        else if((cosf(S3DL::degreeToRad(__angle_theta))) < (sinf(S3DL::degreeToRad(__angle_theta))))
            ratioRaw = (cosf(S3DL::degreeToRad(__angle_theta))) / (sinf(S3DL::degreeToRad(__angle_theta)));
        
        return 1.0f / (ratioRaw + 1.0f);
    }

    triLens tri_findWidthHeight(const float& __angle_theta, const float& __len_hypo)
    {
            triLens side_lens;
            
            if(__len_hypo > 0)
            {
                side_lens.h = __len_hypo * sinf(S3DL::degreeToRad(__angle_theta)) * -1.0f;
                side_lens.w = __len_hypo * cosf(S3DL::degreeToRad(__angle_theta)) * -1.0f;
            }
            else
            {
                side_lens.h = __len_hypo * sinf(S3DL::degreeToRad(__angle_theta));
                side_lens.w = __len_hypo * cosf(S3DL::degreeToRad(__angle_theta));
            }
            
            return side_lens;
    }

    float add_degrees(const float& i, const float& degrees)
    {
        float out = i;
        if(out + degrees > 360)
        {
            // getting how much is left
            float x = 360.0f - out;
            // accounting for since we know x will be less than degrees
            // and then setting fYaw to value
            out = degrees - x;
        }
        else if(out + degrees < 0)
        {
            // same thing but multiply the degrees by -1 to get absouloute
            // value and then do standard calulations
            // keep in mind the '+' operator is still required since we do
            // not want to minus a negative (because it would produce a
            // possitive result)
            float x = out + degrees;
            out = 360.0f + degrees;
        }
        else
        {
            // no problem here because fyaw + degrees < 360
            out += degrees;
        }
        return out;
    }

    float fFind_Max_Vector(const std::vector<float>& i)
    {
        int flagged = 0;
        for(int j = 1; j < i.size(); j++)
        {
            if(i[flagged] < i[j])
                flagged = j;
        }
        return i[flagged];
    }

    class srcText
    {
    private:
        std::string message;
        TTF_Font* font;
        SDL_Color col;

    public:
        SDL_Rect r { 0, 0, 0, 0 };
        int w = 32;

        void init(SDL_Color col, TTF_Font* font)
        {
            this->r.h = 32;
            this->col = col;
            this->font = font;
        }

        void set_mes(const std::string& mes)
        {
            message = mes;
        }
        void present(SDL_Renderer* renderer)
        {
            r.w = message.size() * w;
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(font, message.c_str(), col));
            if(SDL_RenderCopy(renderer, texture, nullptr, &r) != 0)
            {
                std::cerr << "[ERROR]: failed to render non constant text" << SDL_GetError() << '\n';
            }
        }
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
        bool useCommands;
        bool commandsActive;
        bool quitting;
        TTF_Font* commandFont;

    public:
        bool windowFocus;
        bool running = true;

        // init class with constructer
        engineState(const char* windowName, bool useCommands, const char* font_filePath, SDL_Window* window)
        {
            windowFocus = false;
            ticks = 0;
            winTitle = windowName;
            this->useCommands = useCommands;
            if(useCommands)
            { 
                commandFont = TTF_OpenFont(font_filePath, 32);
            }
        }

        ~engineState()
        {
            TTF_CloseFont(commandFont);
            if(!quitting)
            {
                std::cout << "[FATAL ERROR]: EngineStates cannot go out of scope\n";
                SDL_Quit();
                running = false;
            }
        }

        void set_renderer(int i)
        {
            if(i == ENGINE_RENDERER_OPENGL || i == ENGINE_RENDERER_SDL2)
            {
                renderer = i;
            }
            else
            {
                LOG("[FATAL ERROR]: Inputed renderer is invalid, valid flags:\n- ENGINE_RENDERER_OPENGL\n - ENGINE_RENDERER_SDL2\n");
                throw;
            }
        }

        int get_renderer()
        {
            return renderer;
        }

        void quit()
        {
            running = false;
            quitting = true;
            SDL_Quit();
        }

        // Update engineState in main loop for use of S3DL
        void update(SDL_Window* window, const char* windowName, SDL_Event e, SDL_Renderer* renderer)
        {
            if(showFPS)
            {
                float f = SDL_GetTicks64() / 1000.0f;
                f -= SDL_GetTicks64() / 1000;

                if(ticks < SDL_GetTicks64() / 1000.0f && SDL_GetTicks64() % 1000 == 0)
                {
                    ticks = floor(SDL_GetTicks64() / 1000.0f);
                    // creating stringstream to keep original name
                    std::stringstream s;
                    s << windowName << ", FPS: " << counts;
                    
                    // converting to const char* and applying to window title
                    std::string convert = s.str();
                    const char* title = convert.c_str();
                    SDL_SetWindowTitle(window, title);
                    counts = 0;
                }
                else
                {
                    counts++;
                }
            }
            if(commandsActive)
            {
                SDL_ShowCursor(SDL_ENABLE);
                std::string input;
                std::string output;
                std::vector<std::string> split;
                int width;
                SDL_GetWindowSize(window, &width, nullptr);
                srcText in;
                in.init({255, 0, 0, 0}, commandFont);
                in.w = 16;
                in.r.h = 32;
                in.r.y = 10;
                in.r.x = 10;
                srcText out;
                out.init({0, 0, 255, 0}, commandFont);
                out.w = 16;
                out.r.h = 32;
                out.r.y = 46;
                out.r.x = 10;
                
                while(commandsActive)
                {
                    while(SDL_PollEvent(&e))
                    {
                        if(e.type == SDL_KEYDOWN)
                        {
                            switch(e.key.keysym.sym)
                            {
                            case SDLK_ESCAPE:
                                commandsActive = false;
                                break;
                            case SDLK_BACKSPACE:
                                if(input.length() != 0)
                                {
                                    if(input.length() > 1)
                                        input.erase(input.length() - 1, input.length() - 1);
                                    else if(input.length() == 1)
                                        input.clear();

                                    // check to see whether screen needs clearing (no chars)
                                    if(input.length() == 0)
                                        SDL_RenderClear(renderer);
                                }
                                break;
                            case SDLK_RETURN:
                                SDL_RenderClear(renderer);
                                input = remove_character(input, ' ');
                                input = make_lowercase(input);
                                split = S3DL::split_string(input, '=');

                                if(split[0] == "hello" && split.size() > 0)
                                {
                                    output = "[OUTPUT]: Hello World!";
                                }
                                else if(split[0] == "terminate" && split.size() > 0)
                                {
                                    throw 5;
                                }
                                else if(split[0] == "show-lines" && split.size() > 0)
                                {
                                    if(split.size() == 2)
                                    {
                                        if(split[1] == "true")
                                        {
                                            output = "[OUTPUT]: Showing lines!";
                                            showLines = true;
                                        }
                                        else if(split[1] == "false")
                                        {
                                            output = "[OUTPUT]: Hiding lines!";
                                            showLines = false;
                                        }
                                        else
                                        {
                                            std::stringstream s;
                                            s << "[OUTPUT]: Invalid value \"" << split[1] << "\"" << "must be \"true\" or \"false\"";
                                            output = s.str();
                                        }
                                    }
                                    else
                                    {
                                        output = "[OUTPUT]: Invalid use of command!";
                                    }
                                }
                                else
                                {
                                    std::stringstream s;
                                    s << "[OUTPUT]: Invalid command \"" << input << "\"";
                                    output = s.str();
                                }

                                split.clear();
                                input.clear();
                                break;
                            default:
                                if(input.length() < (width / 16))
                                    input.push_back(e.key.keysym.sym);
                                break;
                            }
                        }
                    }
                    if(input.size() > 0 || output.length() > 0)
                        SDL_RenderClear(renderer);
                    if(input.size() > 0)
                    {
                        in.set_mes(input);
                        in.present(renderer);
                    }
                    if(output.length() > 0)
                    {
                        out.set_mes(output);
                        out.present(renderer);
                    }
                    SDL_RenderPresent(renderer);
                }                
            }
        }

        void project(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, float far, float near)
        {
            if(this->renderer == S3DL::ENGINE_RENDERER_SDL2)
            {
                std::sort(trisToRender.begin(), trisToRender.end(), [](triangle &t1, triangle &t2)
                {
                    float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                    float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                    return z1 < z2;
                });

                for(auto& triProjected : trisToRender)
                {
                    vector3 p[3] = {{triProjected.p[0].x * 200 + (width * 0.39f), triProjected.p[0].y * 200 + (height * 0.37f)}, 
                                    {triProjected.p[1].x * 200 + (width * 0.39f), triProjected.p[1].y * 200 + (height * 0.37f)}, 
                                    {triProjected.p[2].x * 200 + (width * 0.39f), triProjected.p[2].y * 200 + (height * 0.37f)}};
                                
                    

                    S3DL::fillTriangle(p, triProjected.col, renderer, texture);
                    if(showLines)
                        S3DL::drawTriangle(p[0], p[1], p[2], {0, 0, 0, 255}, renderer);
                }
                trisToRender.clear();
            }
            else
            {
                LOG("[FATAL ERROR]: More params required for rendering using Opengl");
                throw;
            }
        }

        void project(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, float far, float near, GLuint vao, GLuint vbo, GLuint shaderProgram)
        {
            if(this->renderer == S3DL::ENGINE_RENDERER_SDL2)
            {
                std::sort(trisToRender.begin(), trisToRender.end(), [](triangle &t1, triangle &t2)
                {
                    float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                    float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                    return z1 < z2;
                });

                for(auto& triProjected : trisToRender)
                {
                    vector3 p[3] = {{triProjected.p[0].x * 200 + (width * 0.39f), triProjected.p[0].y * 200 + (height * 0.37f)}, 
                                    {triProjected.p[1].x * 200 + (width * 0.39f), triProjected.p[1].y * 200 + (height * 0.37f)}, 
                                    {triProjected.p[2].x * 200 + (width * 0.39f), triProjected.p[2].y * 200 + (height * 0.37f)}};
                    
                // S3DL::fillTriangleBatch(vecTrisToRender, renderer, texture);
                    if(showLines)
                        S3DL::drawTriangle(p[0], p[1], p[2], {0, 0, 0, 255}, renderer);
                    S3DL::fillTriangle(p, triProjected.col, renderer, texture);
                }
                trisToRender.clear();
            }
            else
            {
                for(mesh m : meshesToRender)
                    S3DL::fillMeshGL(m, width, height, far, near, vao, vbo, shaderProgram); 
                meshesToRender.clear();
            }
        }

        // for showing fps
        void displayFPS(const int& toggle, SDL_Window* window)
        {
            if(toggle == S3DL::ENGINE_SHOW)
                showFPS = true;

            if(toggle == S3DL::ENGINE_HIDE)
            {
                showFPS = false;
                SDL_SetWindowTitle(window, winTitle);
            }
            else
                std::cout << "[ERROR] Invalid window toggle entered\n";
        }

        int getFPS()
        {
            return ticks;
        }

        // IMPORTANT!! For engine use only, regular programmers should not use this function.
        // Function adds triangles to render list 
        void __ADD_TO_RENDER_LIST(const std::vector<triangle>& __i, bool __sl, const mesh& __i2) 
        {
            if(renderer == S3DL::ENGINE_RENDERER_SDL2)
            {
                for(int j = 0; j < __i.size(); j++)
                    trisToRender.push_back(__i[j]);
            }
            else
            {
                meshesToRender.push_back(__i2);
            }

        }

        void command_box_toggle(const int& toggle)
        {
            if(useCommands)
            {
                if(toggle == S3DL::ENGINE_COMMANDS_ACTIVE)
                    commandsActive = true;
                else if(toggle == S3DL::ENGINE_COMMANDS_CLOSE)
                    commandsActive = false;
                else
                    std::cout << "[ERROR]: Invalid command toggle entered\n";
            }
            else
                std::cout << "Cannot activate/deactivate commands on a non-command active engineState\n";
        }
    };
}


// main classes

class camera
{
protected: 
    float fYaw;
    float fPitch;

public:
    float fFov;
    bool showLines = false;
    bool safeMode = true;
    vector3 cameraPos = {0, 0, 0}; 

    // using function sinse all movements apart from y require math, however
    // cameraPos is still public in the case that a programmer wnats to
    // implement their own movement algorithm 
    void move(const int& direction, const float& step)
    {
        switch(direction)
        {
        case S3DL::ENGINE_FOWARD:
            // dividing movement into 4 different quadrants each depending
            // on size of fYaw, and using basic trigonomety to figure out
            // how far to travel on each axis to give illusion of foward
            // movement
            if
            (fYaw < 360 && fYaw > 270)
            {
                // checking if sinf/cosf result is negative, in which case 
                // a inversion is needed
                triLens tri_formed = S3DL::tri_findWidthHeight(fYaw, step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;                
            }
            else if
            (fYaw < 90 && fYaw > 0)
            {
                triLens tri_formed = S3DL::tri_findWidthHeight(fYaw, step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;
            }
            else if
            (fYaw < 180 && fYaw > 90)
            {
                triLens tri_formed = S3DL::tri_findWidthHeight(fYaw, step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;
                
            }
            else if
            (fYaw < 270 && fYaw > 180)
            {
                triLens tri_formed = S3DL::tri_findWidthHeight(fYaw, step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;
                
            }
            // all thats left is the cases where fYaw = 90, 180, 270 and 360/0
            else
            {
                // using switch for cases and setting fYaw to int because we
                // know that it must be either 90, 180, 270 or 360/0, all of
                // which are ints
                switch((int)fYaw)
                {
                case 0:
                    cameraPos.z += step; // normal z movement
                    
                    break;
                case 90:
                    cameraPos.x += step; // normal x movement
                    
                    break;
                case 180:
                    cameraPos.z -= step; // reverse z movement
                    
                    break;
                case 270:
                    cameraPos.x -= step; // reverse x movement
                    
                    break;
                }
            }
            break;
        case S3DL::ENGINE_STRAFE:
            // exact same algorithm as before however rotated 90 degrees
            if
            (fYaw < 360 && fYaw > 270)
            {
                triLens tri_formed = S3DL::tri_findWidthHeight(S3DL::add_degrees(fYaw, 90.0f), step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;
                
            }
            else if
            (fYaw < 90 && fYaw > 0)
            {
                triLens tri_formed = S3DL::tri_findWidthHeight(S3DL::add_degrees(fYaw, 90.0f), step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;
                
            }
            else if
            (fYaw < 180 && fYaw > 90)
            {
                triLens tri_formed = S3DL::tri_findWidthHeight(S3DL::add_degrees(fYaw, 90.0f), step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;
                
            }
            else if
            (fYaw < 270 && fYaw > 180)
            {
                triLens tri_formed = S3DL::tri_findWidthHeight(S3DL::add_degrees(fYaw, 90.0f), step);
                cameraPos.x += step * tri_formed.h * 100.0f;
                cameraPos.z -= step * tri_formed.w * 100.0f;
                
            }
            // all thats left is the cases where fYaw = 90, 180, 270 and 360/0
            else
            {
                // using switch for cases and setting fYaw to int because we
                // know that it must be either 90, 180, 270 or 360/0, all of
                // which are ints
                switch((int)fYaw)
                {
                case 90:
                    cameraPos.z += step; // normal z movement
                    
                    break;
                case 180:
                    cameraPos.x += step; // normal x movement
                    
                    break;
                case 270:
                    cameraPos.z -= step; // reverse z movement
                    
                    break;
                case 0:
                    cameraPos.x -= step; // normal x movement
                    
                    break;
                }
            }
            break;
        case S3DL::ENGINE_VERTICAL:
            cameraPos.y += step;
            break;
        }
    }

    void rotate(const int& type, const float& degrees)
    {
        switch(type)
        {
        case S3DL::ENGINE_YAW:
            // checking to see whether degrees change is still in an 
            // acceptable range (i.e 0 <= x <= 360)
            // if not this algorithm finds the left overs and aplies
            // calculation
            if(fYaw + degrees > 360)
            {
                // getting how much is left
                float x = 360.0f - fYaw;
                // accounting for since we know x will be less than degrees
                // and then setting fYaw to value
                fYaw = degrees - x;
            }
            else if(fYaw + degrees < 0)
            {
                // same thing but multiply the degrees by -1 to get absouloute
                // value and then do standard calulations
                // keep in mind the '+' operator is still required since we do
                // not want to minus a negative (because it would produce a
                // possitive result)
                float x = fYaw + degrees;
                fYaw = 360.0f + degrees;
            }
            else
            {
                // no problem here because fyaw + degrees < 360
                fYaw += degrees;
            }
            break;
        case S3DL::ENGINE_PITCH:

            // no wrapping because pitch does not require wrapping
            if((fPitch + degrees) < -90)
                fPitch = -90.0f;
            else if((fPitch + degrees) > 90)
                fPitch = 90.0f;
            else
                fPitch += degrees;
            break;

        default:
            std::cout << "[ERROR]: Invalid camera rotation type imputed";
            break;
        }
    }

    // returns fYaw/fPitch because they are private
    float fgetYaw()    { return fYaw; }

    float fgetPitch()  { return fPitch; }

    void resetPitch()  { fPitch = 0;  }

    void resetYaw()    { fYaw = 0;    }

    void setYaw(float degrees)
    {
        if(degrees > 360 || degrees < 0)
        {
            switch(safeMode)
            {
            case true:
                std::cout << "[FATAL ERROR]: Camera yaw setting is out of bounds!\n";
                throw;
                break;
            default:
                std::cout << "[ERROR]: Camera yaw setting is out of bounds!\n";
                break;
            }
        }
        else
        {
            fYaw = degrees;
        }
    }

    
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


    void init(const mesh& objectImput, const SDL_Color& objectColourInput, const char* objectName, const char* filePath)
    {
        objectColour = objectColourInput;
        name = objectName;
        degreesX = 0.0f;
        degreesY = 0.0f;
        degreesZ = 0.0f;
        rotateSpeed = 0.0f;
        int load1 = 0;
        float load2 = 0;
        int progress = 0;
        int tick = 0;
        std::ifstream file(filePath);

        matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

        if(filePath == nullptr)
        {
            m_object = objectImput;
        }
        if(filePath != nullptr && file.fail())
        {
            std::cout << "[ERROR]: "<< objectName << ": file imputed is invalid\n";
        }
        if(filePath != nullptr && !file.fail())
        {
            char c[10];
            std::stringstream s;

            file.getline(c, 10);
            if(c[0] == 'n')
            {
                char junk;
                s << c;
                s >> junk >> load1;
            }
            else
                std::cout << "[WARNING]: No size number (i.e \"n 324\") at top of file, file loading but no progress reports available\n";

            std::vector<vector3> verts;

            while(!file.eof())
            {                
                char line[128];
                char junk;
                file.getline(line, 128);
                std::stringstream str;

                str << line;

                if(line[0] == 'v')
                {
                    vector3 v;
                    str >> junk >> v.x >> v.y >> v.z;
                    verts.push_back(v);
                }
                if(line[0] == 'f')
                {
                    int f[3];
                    str >> junk >> f[0] >> f[1] >> f[2];
                    m_object.tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});
                }

                if(load1 != 0)
                {
                    if(tick == 10)
                        std::cout << "[INFO]: Loading file " << "\"" << filePath << "\"" << ", progress: " << load2 * (100.0f / load1) << "%\n";

                    load2 ++;

                    if(tick > 10)
                        tick = 0;
                    else
                        tick ++;
                }
            }
        }

        std::cout << "[INFO]: " << objectName << ": Init complete, rendering...\n\n";
    }

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
     void addToRenderList(int height, int width, float fFar, float fNear, camera engineCam, const std::vector<vector3>& lightDirs, S3DL::engineState& engineState)
    {
        if(engineState.get_renderer() == S3DL::ENGINE_RENDERER_SDL2)
        {
            // create point at matrix
            vector3 vUp = {0, 1, 0};
            vector3 vTarget = {0, 0, 1};

            mat4x4 camRotY = S3DL::matrix_MakeRotationX(S3DL::degreeToRad(engineCam.fgetPitch()));
            mat4x4 camRotX = S3DL::matrix_MakeRotationY(S3DL::degreeToRad(engineCam.fgetYaw()));
            
            mat4x4 matCameraRot = S3DL::matrix_multiplyMatrix(camRotY, camRotX);

            vector3 vlookDir = S3DL::matrix_MultiplyVector(vTarget, matCameraRot);
            vTarget = S3DL::vectorAdd(engineCam.cameraPos, vlookDir);

            mat4x4 matCamera = S3DL::matrix_PointAt(engineCam.cameraPos, vTarget, vUp);
            mat4x4 matView = S3DL::matrix_QuickInverse(matCamera);
            
            // applying translation/rotation to pre-rastorised triangles

            if(rotateR.dir == 'x' && rotateR.rot)
            {
                degreesX += rotateSpeed;
            }
            if(rotateR.dir == 'z' && rotateR.rot)
            {
                degreesZ += rotateSpeed;
            }
            if(rotateR.dir == 'y' && rotateR.rot)
            {
                degreesY += rotateSpeed;
            }
            if(rotateR.dir == 'u' && rotateR.rot)
            {
                degreesZ += rotateSpeed; 
                degreesX += rotateSpeed;
                degreesY += rotateSpeed;
            }
            fThetaX = degreesX * (pi / 180.0f);
            fThetaZ = degreesZ * (pi / 180.0f);
            fThetaY = degreesY * (pi / 180.0f);
            matRotZ = S3DL::matrix_MakeRotationZ(fThetaZ);
            matRotX = S3DL::matrix_MakeRotationX(fThetaX);
            matRotY = S3DL::matrix_MakeRotationY(fThetaY);

            mat4x4 matTrans, matWorld;   
            matTrans = S3DL::matrix_MakeTranslation(pos.x, pos.y, pos.z);
            matWorld = S3DL::matrix_MakeIdentity();
            matWorld = S3DL::matrix_multiplyMatrix(matRotY, matRotX);
            matWorld = S3DL::matrix_multiplyMatrix(matWorld, matRotZ);
            matWorld = S3DL::matrix_multiplyMatrix(matWorld, matTrans);
            matProj = S3DL::Matrix_MakeProjection(engineCam.fFov, 1.0f, fNear, fFar);
            // vector of triangles for rasterization
            std::vector<triangle> vecTrisToRender;
            // rastorising triangles
            for(auto tri : m_object.tris)
            {
                triangle triProjected, triTransformed, triViewed;
                triTransformed.p[0] = S3DL::matrix_MultiplyVector(tri.p[0], matWorld);
                triTransformed.p[1] = S3DL::matrix_MultiplyVector(tri.p[1], matWorld);
                triTransformed.p[2] = S3DL::matrix_MultiplyVector(tri.p[2], matWorld);
                normal = S3DL::calculateNormal(triTransformed);
                vector3 vCameraRay = S3DL::vectorSub(triTransformed.p[0], engineCam.cameraPos);

                if(S3DL::dotProduct(normal, vCameraRay) < 0.0f)
                {
                    float dp;
                    std::vector<float> dps;
                    // shading calculations
                    for(auto t : lightDirs)
                    {
                        t = S3DL::vectorNormalise(t);

                        dps.push_back(S3DL::dotProduct(t, normal));
                    }
                    dp = S3DL::fFind_Max_Vector(dps);
                    triProjected.col = {(uint8_t)(objectColour.a * dp), (uint8_t)(objectColour.b * dp), (uint8_t)(objectColour.g * dp), (uint8_t)(objectColour.r * dp)};

                    // applying camera matrix 

                    triViewed.p[0] = S3DL::matrix_MultiplyVector(triTransformed.p[0], matView);
                    triViewed.p[1] = S3DL::matrix_MultiplyVector(triTransformed.p[1], matView);
                    triViewed.p[2] = S3DL::matrix_MultiplyVector(triTransformed.p[2], matView);

                    // clip viewed triangle against near plane

                    int nClippedTriangles = 0;
                    triangle clipped[2];
                    nClippedTriangles = S3DL::triangle_ClipAgainstPlane({ 0.0f, 0.0f, fNear }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

                    for (int n = 0; n < nClippedTriangles; n++)
                    {
                        // Project triangles from 3D --> 2D
                        triProjected.p[0] = S3DL::matrix_MultiplyVector(clipped[n].p[0], matProj);
                        triProjected.p[1] = S3DL::matrix_MultiplyVector(clipped[n].p[1], matProj);
                        triProjected.p[2] = S3DL::matrix_MultiplyVector(clipped[n].p[2], matProj);

                        // Scale into view, we moved the normalising into cartesian space

                        triProjected.p[0] = S3DL::vectorDiv(triProjected.p[0], triProjected.p[0].w);
                        triProjected.p[1] = S3DL::vectorDiv(triProjected.p[1], triProjected.p[1].w);
                        triProjected.p[2] = S3DL::vectorDiv(triProjected.p[2], triProjected.p[2].w);

                        // Offset verts into visible normalised space
                        vector3 vOffsetView = {1, 1, 0};
                        triProjected.p[0] = S3DL::vectorAdd(triProjected.p[0], vOffsetView);
                        triProjected.p[1] = S3DL::vectorAdd(triProjected.p[1], vOffsetView);
                        triProjected.p[2] = S3DL::vectorAdd(triProjected.p[2], vOffsetView);

                        // Store triangle for sorting
                        vecTrisToRender.push_back(triProjected);
                    }
                }
                // vecTrisToRender = S3DL::reverseVector(vecTrisToRender);

            }
            engineState.__ADD_TO_RENDER_LIST(vecTrisToRender, engineCam.showLines, {{0, 0, 0}});
        }
        else
        {
            m_object.r.rot = rotateR.rot;
            m_object.r.dir = rotateR.dir;
            m_object.r.step = rotateR.step;
            m_object.pos = pos;

            engineState.__ADD_TO_RENDER_LIST({{0, 0, 0}}, engineCam.showLines, m_object);
        }

        rotateR.rot = false;
    }

    
    /*
    * Transforms object in differernt ways,
    * \param type Type of transformation, `flags` include:
    * - `S3DL::ENGINE_ROTATE` For rotational translations,
    * - `S3DL::ENGINE_MOVE` For cardinal/normal translations.
    * \param axis Axis for desired translation.
    * - Can be 'x', 'y', or 'z'.
    * \param degrees Degree of translation, such as degree
    * of rotation or degree of movement.
    */
    void transform(int type, char axis, float degrees)
    {   
        if(type == S3DL::ENGINE_ROTATE)
        {
            switch(axis)
            {
            case 'x':
                rotateSpeed = degrees;
                rotateR.rot = true;
                rotateR.dir = 'x';
                break;
            case 'z':
                rotateSpeed = degrees;
                rotateR.rot = true;
                rotateR.dir = 'z';
                break;
            case 'y':   
                rotateSpeed = degrees;
                rotateR.rot = true;
                rotateR.dir = 'y';
                break;
            case 'u':
                rotateSpeed = degrees;
                rotateR.rot = true;
                rotateR.dir = 'u';
                break;
            default:
                std::cout << "[ERROR]: Failed to transform object: " << name << ", Error: Invalid character imputed\n";
                break;
            }
        }
        else if(type == S3DL::ENGINE_MOVE)
        {
            switch(axis)
            {
            case 'x':
                pos.x += degrees;
                break;
            case 'y':
                pos.y += degrees;
                break;
            case 'z':
                pos.z += degrees;
                break;
            default:
                std::cout << "[ERROR]: Failed to transform object: " << name << ", Error: Invalid character imputed\n";
                break;
            }
        }
    }

};

template<int typeT>
class actor
{
private:
    bool valid;
    int type = typeT;

public:
    vector3 pos;

    actor(vector3 pos)
    {
        this->pos = pos;
    }

    actor operator+(const actor& other)
    {
        if(other.type == type)
        {
            return {{0, 0, 0}};
        }
        else
            throw "[FATAL ERROR]: Cannot add two actors of different types!";
    }
};