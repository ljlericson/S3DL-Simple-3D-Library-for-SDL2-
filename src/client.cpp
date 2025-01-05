
// engine.h loads all relevent headers and defines SDL stuff

#include "engine.h"
#include <random>


int main(int argv, char** args)
{
    S3DL::initSDL();

    int height, width;
    int tick = 0;

    char lineWH[20];
    const char* windowTitle;
    std::string convert;

    std::stringstream sWH;
    std::ifstream widthHeight("windowSize.size");
    std::ifstream objToRender("objs.info");

    // Mix_Chunk* testAudio = S3DL::loadWAV()

    widthHeight.getline(lineWH, 20);
    sWH << lineWH;
    sWH >> width >> height;
    bool windowFocus = false;
    bool useCam = false;
    bool heheheha = false;
    mesh initialiser;
    initialiser.tris = {{0}, {0}, {0}};
    
    SDL_Window* window1 = SDL_CreateWindow("3D Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer1 = SDL_CreateRenderer(window1, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event e;
    // SDL_GLContext glContext = SDL_GL_CreateContext(window1);
    // if (!glContext) 
    // {
    //     std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
    //     return -1;
    // }
    // S3DL::initGl();

    // glewExperimental = GL_TRUE; // Ensure GLEW uses modern OpenGL
    // if (glewInit() != GLEW_OK) 
    // {
    //     std::cerr << "Failed to initialize GLEW" << std::endl;
    //     return -1;
    // }

    // GLuint vao, vbo;
    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);

    // S3DL::init(vao, vbo);

    // std::string vs_src = S3DL::get_vs_src();
    // std::string fs_src = S3DL::get_fs_src();

    // GLuint shad_program = S3DL::create_shader(vs_src, fs_src);
    // glUseProgram(shad_program);
    // SDL_GL_SetSwapInterval(1);

    const Uint8* state;
    float volume = 100.0f;

    object test, test2, cube1, cube2, cube3, cube4, sheet;
    cube1.init(initialiser, {255, 255, 255, 255}, "cube1", "./assets/obj/cube.obj");
    cube2.init(initialiser, {255, 255, 255, 255}, "cube2", "./assets/obj/cube.obj");
    cube3.init(initialiser, {255, 255, 255, 255}, "cube3", "./assets/obj/cube.obj");
    cube4.init(initialiser, {255, 255, 255, 255}, "cube4", "./assets/obj/cube.obj");
    test2.init(initialiser, {255, 255, 255, 255}, "test2", "./assets/obj/cool_gun.obj");
    test.init(initialiser, {255, 255, 0, 255}, "test", "./assets/obj/VideoShip.obj");
    sheet.init(initialiser, {255, 0, 255, 0}, "sheet", "./assets/obj/mountains.obj");

    cube1.transform(S3DL::ENGINE_MOVE, 'z', 8.0f);
    cube1.transform(S3DL::ENGINE_MOVE, 'x', 3.0f);

    cube2.transform(S3DL::ENGINE_MOVE, 'z', 8.0f);
    cube2.transform(S3DL::ENGINE_MOVE, 'x', -3.0f);

    cube3.transform(S3DL::ENGINE_MOVE, 'z', 8.0f);

    cube4.transform(S3DL::ENGINE_MOVE, 'z', 8.0f);
    cube4.transform(S3DL::ENGINE_MOVE, 'x', 6.0f);

    test.transform(S3DL::ENGINE_ROTATE, 'x', 180.0f);
    test2.transform(S3DL::ENGINE_MOVE, 'x', 10.0f);
    test2.transform(S3DL::ENGINE_MOVE, 'y', 20.0f);

    // if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)
    // {
    //     std::cout << "Could not open audio, Error: " << Mix_GetError();
    // }

    // Mix_Chunk* music = SDLFunc::loadWAV("music.mp3");
    // SDLFunc::playSound(music, volume, 1, -1);

    camera mainCamera, testCam;
    
    S3DL::engineState engState("3D Engine", true, "./assets/fonts/font.ttf", window1);
    engState.displayFPS(S3DL::ENGINE_SHOW, window1);
    engState.set_renderer(S3DL::ENGINE_RENDERER_SDL2);

    testCam.fFov = 80;
    mainCamera.fFov = 80;
    mainCamera.rotate(S3DL::ENGINE_YAW, 180.0f);
    testCam.rotate(S3DL::ENGINE_YAW, 180.0f);

    std::vector<vector3> lights;
    lights.push_back({0.0f, 0.0f, -1.0f});
    lights.push_back({5, 0, 0});
    lights.push_back({0, 0, 5});

    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glEnable(GL_DEPTH_TEST);  // Enable depth testing
    // glDisable(GL_CULL_FACE);

    while(engState.running)
    {
        engState.update(window1, "3D Engine", e, renderer1);

        if(engState.windowFocus)
        {
            if(tick > 10)
            {
                SDL_WarpMouseInWindow(window1, width * 0.5, height * 0.5);
                tick = 0;
            }
            else
                tick ++;
            
            SDL_ShowCursor(SDL_DISABLE);
        }
        else
            SDL_ShowCursor(SDL_ENABLE);


        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                SDL_DestroyRenderer(renderer1);
                // SDL_GL_DeleteContext(glContext);
                SDL_DestroyWindow(window1);
                engState.quit();
            }
            if(e.type == SDL_MOUSEMOTION && engState.windowFocus)
            {
                mainCamera.rotate(S3DL::ENGINE_YAW, e.motion.xrel * -0.1f);
                mainCamera.rotate(S3DL::ENGINE_PITCH, e.motion.yrel * -0.1f);
            }
            if(e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    engState.windowFocus = false;
                    break;
                case SDLK_c:
                    mainCamera.fFov += 10;
                    break;
                case SDLK_v:
                    mainCamera.fFov -= 10;
                    break;
                case SDLK_F3:
                    mainCamera.showLines = false;
                    useCam = false;
                    // engState.displayFPS(S3DL::ENGINE_HIDE, window1);
                    break;
                case SDLK_F4:
                    // mainCamer.showLines = true;
                    useCam = true;
                    // engState.displayFPS(S3DL::ENGINE_SHOW, window1);
                    break;
                case SDLK_t:
                    mainCamera.cameraPos = {0, 0, 0};
                    test.pos.x = 0.0f;
                    test.pos.y = 0.0f;
                    test.pos.z = 0.0f;
                    heheheha = false;
                    mainCamera.setYaw(180.0f);
                    break;
                case SDLK_o:
                    engState.command_box_toggle(S3DL::ENGINE_COMMANDS_ACTIVE);
                    break;
                case SDLK_y:
                    if(heheheha)
                        break;
                    else
                        heheheha = true;
                    break;
                }
            }
            if(e.type == SDL_MOUSEBUTTONDOWN)
                engState.windowFocus = true;
        }

        state = SDL_GetKeyboardState(NULL);

        if(state[SDL_SCANCODE_SPACE])
            mainCamera.move(S3DL::ENGINE_VERTICAL, (state[SDL_SCANCODE_LSHIFT]) ? 0.03f : 0.01f);
        if(state[SDL_SCANCODE_LCTRL])
            mainCamera.move(S3DL::ENGINE_VERTICAL, (state[SDL_SCANCODE_LSHIFT]) ? -0.03f : -0.01f);
        if(state[SDL_SCANCODE_W])
            mainCamera.move(S3DL::ENGINE_FOWARD, (state[SDL_SCANCODE_LSHIFT]) ? -0.03f : -0.01f);
        if(state[SDL_SCANCODE_S])
           mainCamera.move(S3DL::ENGINE_FOWARD, (state[SDL_SCANCODE_LSHIFT]) ? 0.03f : 0.01f);
        if(state[SDL_SCANCODE_D])
            mainCamera.move(S3DL::ENGINE_STRAFE, (state[SDL_SCANCODE_LSHIFT]) ? 0.03f : 0.01f);
        if(state[SDL_SCANCODE_A])
            mainCamera.move(S3DL::ENGINE_STRAFE, (state[SDL_SCANCODE_LSHIFT]) ? -0.03f : -0.01f);
        if(state[SDL_SCANCODE_M])
            mainCamera.rotate(S3DL::ENGINE_PITCH, (state[SDL_SCANCODE_LSHIFT]) ? 0.03f : 0.01f);
        if(state[SDL_SCANCODE_J])
            mainCamera.rotate(S3DL::ENGINE_PITCH, (state[SDL_SCANCODE_LSHIFT]) ? -0.03f : -0.01f);
        if(state[SDL_SCANCODE_RIGHT])
            test.transform(S3DL::ENGINE_ROTATE, 'z', 0.01f);
        if(state[SDL_SCANCODE_LEFT])
            test.transform(S3DL::ENGINE_ROTATE, 'z', -0.01f);

        SDL_GetWindowSize(window1, &width, &height);
        SDL_RenderClear(renderer1);
        // glViewport(0, 0, width, height);

        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(useCam)
        {
            test2.addToRenderList(height, width, 100.0f, 0.1f, testCam, lights, engState);
            // test.addToRenderList(height, width, 100.0f, 0.1f, testCam, lights, engState);
            // cube1.addToRenderList(height, width, 100.0f, 0.1f, testCam, lights, engState);
            // cube2.addToRenderList(height, width, 100.0f, 0.1f, testCam, lights, engState);
            // cube3.addToRenderList(height, width, 100.0f, 0.1f, testCam, lights, engState);
            // cube4.addToRenderList(height, width, 100.0f, 0.1f, testCam, lights, engState);
            sheet.addToRenderList(height, width, 100.0f, 0.1f, testCam, lights, engState);
            engState.project(renderer1, nullptr, width, height, 100.0f, 0.1f);
        }
        else
        {
            test2.addToRenderList(height, width, 100.0f, 0.1f, mainCamera, lights, engState);
            // test.addToRenderList(height, width, 100.0f, 0.1f, mainCamera, lights, engState);
            // cube1.addToRenderList(height, width, 100.0f, 0.1f, mainCamera, lights, engState);
            // cube2.addToRenderList(height, width, 100.0f, 0.1f, mainCamera, lights, engState);
            // cube3.addToRenderList(height, width, 100.0f, 0.1f, mainCamera, lights, engState);
            // cube4.addToRenderList(height, width, 100.0f, 0.1f, mainCamera, lights, engState);
            sheet.addToRenderList(height, width, 100.0f, 0.1f, mainCamera, lights, engState);
            engState.project(renderer1, nullptr, width, height, 100.0f, 0.1f);
        } 
        cube1.transform(S3DL::ENGINE_ROTATE, 'z', 0.1f);
        cube2.transform(S3DL::ENGINE_ROTATE, 'x', 0.1f);
        cube3.transform(S3DL::ENGINE_ROTATE, 'y', 0.1f);

        if(heheheha)
        {
            test.transform(S3DL::ENGINE_ROTATE, 'u', 0.1f);
            test.transform(S3DL::ENGINE_MOVE, 'z', 0.01f);
            test.transform(S3DL::ENGINE_MOVE, 'y', 0.01f);
        }

        // SDL_SetRenderDrawColor(renderer1, 66, 202, 255, 255);
        SDL_SetRenderDrawColor(renderer1, 0, 0, 0, 255);
        SDL_RenderPresent(renderer1);
        // SDL_GL_SwapWindow(window1);
    }
    return 0;
}