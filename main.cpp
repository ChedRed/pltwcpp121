#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "sdlas.hpp"


#define trusizeof(x) (sizeof(x)/sizeof(x[0]))


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


/* Score variables */
int Score = 0;
SDL_FRect FullScore = {8, 0, 0, 0};
SDL_Texture * ScoreNum[10];
SDL_FRect ScoreNumRect[trusizeof(ScoreNum)];


int SDLCALL WindowEventFilter(void * userdata, SDL_Event * e){
    return 0;
}


int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    TTF_Font * font = TTF_OpenFont(((std::string)SDL_GetBasePath()+"../Resources/FreeSans.ttf").c_str(), 32);
    std::cout << SDL_GetError() << std::endl;
    SDL_CreateWindowAndRenderer("PLTW 1.2.1 (C++)", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_AddEventWatch(WindowEventFilter, NULL);
    SDL_SetWindowMinimumSize(window, 800, 600);
    SDL_SetRenderVSync(renderer, 1);


    /* Setup score stuffs */
    SDL_Surface * tempsurf = TTF_RenderText_Blended(font, "Score: ", 7, {255, 255, 255, 255});
    SDL_Texture * ScoreLabel = SDL_CreateTextureFromSurface(renderer, tempsurf);
    SDL_GetTextureSize(ScoreLabel, &FullScore.w, &FullScore.h);
    for (int i = 0; i < trusizeof(ScoreNum); i++){
        SDL_DestroySurface(tempsurf);
        tempsurf = TTF_RenderText_Blended(font, std::to_string(i).c_str(), 1, {255, 255, 255, 255});
        ScoreNum[i] = SDL_CreateTextureFromSurface(renderer, tempsurf);
        SDL_GetTextureSize(ScoreNum[i], &ScoreNumRect[i].w, &ScoreNumRect[i].h);
    }


    /* Setup circle */
    SDL_Texture * tutel = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, radius * 2, radius * 2);
    SDL_SetRenderTarget(renderer, tutel);
    SDL_SetRenderDrawColor(renderer, SDL_Color{0, 0, 0, 0});
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, SDL_Color{255, 255, 255, 255});
    for (int i = 0; i < (pow(radius * 2, 2)); i++){
        Vector2f center = {(float)radius, (float)radius};
        Vector2f tempos = {(float)(i % (radius * 2)), (float)(i / (radius * 2))};
        SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, limit((float)radius-(tempos-center).magnitude(), 0, 1));
        SDL_RenderPoint(renderer, tempos.x, tempos.y);
    }


    /* Main loop */
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
                            target = Vector2f{(float)(SDL_rand(screenspace.x-(radius * 4)) + (radius * 2)), (float)(SDL_rand(screenspace.y-(radius * 4) - FullScore.h) + (radius * 2) + FullScore.h)};
                            Score++;
                            std::cout << Score << std::endl;
                        }
                    }
                    break;


                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    target = Vector2f{(float)(SDL_rand(screenspace.x-(radius * 4)) + (radius * 2)), (float)(SDL_rand(screenspace.y-(radius * 4) - FullScore.h) + (radius * 2) + FullScore.h)};
                    std::cout << "CHANGED" << std::endl;
                    break;


                case SDL_EVENT_QUIT:
                    loop = false;
                    break;
            }
        }


        pos = target + ((pos-target) * pow(.5, 12. * deltime));
        rectpos = {pos.x - radius, pos.y - radius, (float)radius * 2, (float)radius * 2};


        /* Render score */
        float AAAAA = FullScore.w + FullScore.x;
        for (int i = 0; i < std::to_string(Score).length(); i++){
            ScoreNumRect[((std::to_string(Score))[i])-'0'].x = AAAAA;
            SDL_RenderTexture(renderer, ScoreNum[((std::to_string(Score))[i])-'0'], NULL, &ScoreNumRect[((std::to_string(Score))[i])-'0']);
            AAAAA += ScoreNumRect[((std::to_string(Score))[i])-'0'].w;
        }

        SDL_RenderTexture(renderer, ScoreLabel, NULL, &FullScore);
        SDL_RenderTexture(renderer, tutel, NULL, &rectpos);
        SDL_RenderPresent(renderer);
        std::cout << std::endl;
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}


#ifdef _WIN32
/* Windows window subsystem :( */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    return main(__argc, __argv);
}
#endif
