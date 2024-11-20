#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string>

#ifdef __EMSCRIPTEN__
    #include <SDL2/SDL_opengles2.h>
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

bool running = true;
SDL_Window* window;
SDL_GLContext context;

bool loadShaders()
{
    // TODO
    return true;
}

bool loadTexture(const std::string& file, unsigned int& textureId)
{
    // TODO
    return true;
}

void terminate()
{
    // Terminate SDL
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void mainloop()
{
    if (!running)
    {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop(); /* this should "kill" the app. */
        terminate();
        return;
#endif
    }

    // Wait for close
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        running = false;
    }

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);  // set the clear color to blue
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    // swap the buffers
    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[])
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

// Set OpenGL attributes
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);      // Use the core OpenGL profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);  // Specify version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);  // Request a color buffer with 8-bits per RGBA channel
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);        // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);  // Force OpenGL to use hardware acceleration

    window = SDL_CreateWindow("Hello world !!",         // window title
                              SDL_WINDOWPOS_UNDEFINED,  // Top left x-coordinate of window
                              SDL_WINDOWPOS_UNDEFINED,  // Top left y-coordinate of window
                              1024,                     // width of window
                              768,                      // height of window
                              SDL_WINDOW_OPENGL);

    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create an OpenGL context
    context = SDL_GL_CreateContext(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return EXIT_FAILURE;
    }

    glGetError();  // On some platforms, GLEW will emit a benign error code, so clear it

    // Main Loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    while (running)
    {
        mainloop();
    }
    terminate();
#endif

    return EXIT_SUCCESS;
}
