#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#undef main  //Needed for windows.

bool running = true;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* renderer)
{
    // Load from file
    SDL_Surface* surface = IMG_Load(file.c_str());
    if (!surface)
    {
        SDL_Log("Failed to load texture file %s", file.c_str());
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* result = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!result)
    {
        SDL_Log("Failed to convert surface to texture for %s", file.c_str());
        return nullptr;
    }

    return result;
}

void mainloop()
{
    if (!running)
    {
#ifdef __EMSCRIPTEN__
        // Terminate SDL
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
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
    SDL_SetRenderDrawColor(renderer,
                           0,    // R
                           0,    // G
                           255,  // B
                           255   // A
    );

    // clear back buffer
    SDL_RenderClear(renderer);

    // Draw texture
    int x = SDL_GetTicks() % 1024;
    SDL_Rect rect {x, 100, 100, 100};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

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

    window = SDL_CreateWindow("Hello world !!",         // window title
                              SDL_WINDOWPOS_UNDEFINED,  // Top left x-coordinate of window
                              SDL_WINDOWPOS_UNDEFINED,  // Top left y-coordinate of window
                              1024,                     // width of window
                              768,                      // height of window
                              0                         // flags (0 for no flags set)
    );

    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window,  // window to create renderer for
                                  -1,      // usually -1
                                  0);

    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    texture = loadTexture("resources/example.png", renderer);

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
    IMG_Quit();
    SDL_Quit();
#endif

    return EXIT_SUCCESS;
}
