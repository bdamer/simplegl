#define USE_SDL

#include <GL/glew.h>

#ifdef USE_SDL
#include <SDL2/SDL.h>
#else
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#endif

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

static auto window_width = 1280;
static auto window_height = 720;
static auto fullscreen = false;
static auto is_done = false;
static auto vsync = true;

#ifdef USE_SDL
static SDL_Window* window = nullptr;
static SDL_GLContext context;
#else
static GLFWwindow* window = nullptr;
#endif 

int init(void)
{
#ifdef USE_SDL

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    SDL_Log("SDL version %d.%d.%d (%d.%d.%d)", 
        static_cast<int>(compiled.major), static_cast<int>(compiled.minor), static_cast<int>(compiled.patch),
        static_cast<int>(linked.major), static_cast<int>(linked.minor), static_cast<int>(linked.patch));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to init: %s", SDL_GetError());
        return 1;
    }

    // Create OpenGL context with desired profile version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    if (window_width == -1 || window_height == -1)
    {
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);
        window_width = display_mode.w;
        window_height = display_mode.h;
    }

    Uint32 window_flags;
    window_flags = SDL_WINDOW_OPENGL;
    if (fullscreen)
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    else
        window_flags |= SDL_WINDOW_OPENGL;

    // Create the window with the requested resolution
    window = SDL_CreateWindow("Simple SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        window_width, window_height, window_flags);
    if (window == nullptr)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return 1;
    }

    context = SDL_GL_CreateContext(window);
    if (context == nullptr)
    {
        SDL_Log("Failed to create context: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetSwapInterval(vsync);
#else
    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( window_width, window_height, "Simple GLFW", NULL, NULL);
    if( window == nullptr ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

#endif
    return 0;
}

void poll_events()
{
#ifdef USE_SDL
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT:
                is_done = true;
                break;
        }
    }
#else
    glfwPollEvents();
    is_done = glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS || glfwWindowShouldClose(window) != 0;
#endif
}

void render()
{

#ifdef USE_SDL
    SDL_GL_SwapWindow(window);
#else
    glfwSwapBuffers(window);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glFinish();
}

void release(void)
{
#ifdef USE_SDL
    SDL_Quit();
#endif
}

int main(int argc, char** argv)
{
    if (init())
        return 1;

    uint64_t t_start, t_middle, t_end;
    uint64_t last_update = SDL_GetPerformanceCounter() - 1;
    uint64_t render_ticks = 0u;  

    while (!is_done)
    {
        t_start = SDL_GetPerformanceCounter();

        poll_events();

        t_middle = SDL_GetPerformanceCounter();
        render();

        t_end = SDL_GetPerformanceCounter();

        const auto frequency = static_cast<double>(SDL_GetPerformanceFrequency());
        auto update_time = static_cast<double>(t_middle - t_start) / frequency;
        auto render_time = static_cast<double>(t_end - t_middle) / frequency;
        auto total_time = update_time + render_time;

        if (total_time > 0.020)
            SDL_Log("Slow frame: %f [ %f %f ]", total_time, update_time, render_time);
    }

    release();
    return 0;
}
