#include <SDL2/SDL.h>

int main(int argc, char* args[])
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    auto window = SDL_CreateWindow("Hello world !!",  // window title
                                   100,               // Top left x-coordinate of window
                                   100,               // Top left y-coordinate of window
                                   1024,              // width of window
                                   768,               // height of window
                                   0                  // flags (0 for no flags set)
    );

    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    auto renderer = SDL_CreateRenderer(window,  // window to create renderer for
                                       -1,      // usually -1
                                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    bool isRunning = true;

    while (isRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                // If we get an SDL_QUIT event, end loop
                case SDL_QUIT:
                    isRunning = false;
                    break;
            }
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

        // exchange front buffer for back buffer
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
