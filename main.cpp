#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <SDL3/SDL.h>


/* Main variables */
bool loop = true;


/* SDL variables */
SDL_Event e;
SDL_Window * window;
SDL_Renderer * renderer;


int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("PLTW 1.2.1 (C++)", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer);


    while (loop){
        while (SDL_PollEvent(&e)){
            switch (e.type){
                case SDL_EVENT_QUIT:
                    loop = false;
            }
        }
    }


    SDL_Quit();
}
