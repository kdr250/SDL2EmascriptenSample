#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

bool running = true;
SDL_Window* window;
SDL_Renderer* renderer;

void mainloop()
{
    if (!running)
    {
#ifdef __EMSCRIPTEN__
        // Terminate SDL
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        emscripten_cancel_main_loop(); /* this should "kill" the app. */
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

    // set draw color blue
    SDL_SetRenderDrawColor(
        renderer,
        0,    // R
        0,    // G
        255,  // B
        255   // A
    );

    // clear back buffer
    SDL_RenderClear(renderer);

    // Draw rect
    int x = SDL_GetTicks() % 1024;
    SDL_Rect rect{ x, 100, 100, 100 };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    // exchange front buffer for back buffer
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(
        "Hello world !!",        // window title
        SDL_WINDOWPOS_UNDEFINED, // Top left x-coordinate of window
        SDL_WINDOWPOS_UNDEFINED, // Top left y-coordinate of window
        1024,                    // width of window
        768,                     // height of window
        0                        // flags (0 for no flags set)
    );

    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(
        window,  // window to create renderer for
        -1,      // usually -1
        0
    );

    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Main Loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    while (running)
    {
        mainloop();
    }

    // Terminate SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif

    return EXIT_SUCCESS;
}
