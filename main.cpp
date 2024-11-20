#include <SDL3/SDL_timer.h>
#include <iostream>
#include <SDL3/SDL.h>
#include "sdlas.hpp"


/* Convenience functions */
void SDL_SetRenderDrawColor(SDL_Renderer * renderer, SDL_Color color) { SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); }
float limit(float value, float min, float max) { return (value<min)?min:((value>max)?max:value); }


/* Main variables */
bool loop = true;


/* SDL variables */
SDL_Event e;
SDL_Window * window;
SDL_Renderer * renderer;
Vector2 screenspace;
Uint32 lastime;
float deltime;


/* 'Turtle' variables */
int radius = 20;
Vector2f mouse = {0, 0};
Vector2f pos = {400, 300};
Vector2f target = pos;
SDL_FRect rectpos = {390, 290, (float)radius, (float)radius};


int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("PLTW 1.2.1 (C++)", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_Texture * tutel = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, radius * 2, radius * 2);
    SDL_SetRenderTarget(renderer, tutel);
    SDL_SetRenderDrawColor(renderer, SDL_Color{0, 0, 0, 0});
    SDL_RenderClear(renderer);
    SDL_SetRenderVSync(renderer, -1);
    SDL_SetRenderDrawColor(renderer, SDL_Color{255, 255, 255, 255});
    for (int i = 0; i < (pow(radius * 2, 2)); i++){
        Vector2f center = {(float)radius, (float)radius};
        Vector2f tempos = {(float)(i % (radius * 2)), (float)(i / (radius * 2))};
        SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, limit((float)radius-(tempos-center).magnitude(), 0, 1));
        SDL_RenderPoint(renderer, tempos.x, tempos.y);
    }


    SDL_SetRenderTarget(renderer, NULL);
    while (loop){
        deltime = (SDL_GetTicks() - lastime) / 1000.;
        lastime = SDL_GetTicks();
        SDL_SetRenderDrawColor(renderer, SDL_Color{0, 0, 0, 255});
        SDL_RenderClear(renderer);
        SDL_GetWindowSizeInPixels(window, &screenspace.x, &screenspace.y);
        SDL_GetMouseState(&mouse.x, &mouse.y);


        while (SDL_PollEvent(&e)){
            switch (e.type){
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (e.button.button == SDL_BUTTON_LEFT){
                        if ((mouse-pos).magnitude() <= radius) {
                            target = Vector2f{(float)(SDL_rand(screenspace.x-(radius * 4)) + (radius * 2)), (float)(SDL_rand(screenspace.y-(radius * 4)) + (radius * 2))};
                        }
                    }
                    break;
                case SDL_EVENT_QUIT:
                    loop = false;
                    break;
            }
        }


        pos = target + ((pos-target) * pow(.5, 12. * deltime));
        rectpos = {pos.x - radius, pos.y - radius, (float)radius * 2, (float)radius * 2};


        SDL_RenderTexture(renderer, tutel, NULL, &rectpos);
        SDL_RenderPresent(renderer);
        SDL_Delay(5);
    }


    SDL_Quit();
}
