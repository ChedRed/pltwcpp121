#ifdef _WIN32
#include <Windows.h>
#endif
#include <fstream>
#include <sstream>
#define _USE_MATH_DEFINES
#include "TextHelp.h"


#ifdef _WIN32
std::string rpath = "./";
#elif __APPLE__
std::string rpath = "../Resources/";
#endif


#ifdef _WIN32
#define SDL_MODKEY SDL_SCANCODE_LCTRL
#elifdef __APPLE__
#define SDL_MODKEY SDL_SCANCODE_LGUI
#endif


#define trusizeof(x) (sizeof(x)/sizeof(x[0]))


struct Vector2i {int x; int y;};


/* Convenience functions */
void SDL_SetRenderDrawColor(SDL_Renderer * renderer, SDL_Color color) { SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); }
float limit(float value, float min, float max) { return (value<min)?min:((value>max)?max:value); }


/* Main variables */
bool loop = true;
bool endgame = false;
bool rendscorbord = false;


/* SDL variables */
SDL_Event e;
SDL_Window * window;
SDL_Renderer * renderer;
Vector2i ScreenSpace = {800, 600};
Uint32 lastime;
float deltime;
Uint32 mousebitmask;
const bool * keystates = SDL_GetKeyboardState(NULL);
float fade = 0;
float scorefade = 0;


/* 'Turtle' variables */
float radius = 20;
Vector2 mouse = {0, 0};
Vector2 pos = {400, 300};
Vector2 target = pos;
SDL_FRect rectpos = {400-(radius/2), 300-(radius/2), radius, radius};


/* Initial variables */
int Score = 0;
float TimeLeft = 30;


// int SDLCALL WindowEventFilter(void * userdata, SDL_Event * e){
//     return 0;
// }


// Bubble sort bc standard sort can't change names array as well
void bubbleSort(int arr[], std::string stringray[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] < arr[j+1]) {
                std::swap(arr[j], arr[j+1]);
                std::swap(stringray[j], stringray[j+1]);
            }
        }
    }
}


int main(int argc, char* argv[]){
    // Setup scoreboard (no json parser ;-;)
    std::ifstream Scores(SDL_GetBasePath()+rpath+"Scores");
    std::string line;
    std::string Names[6];
    std::string ScoreString[6];
    int TopScores[6];
    int CurrentRead = 0;
    int realsize = 0;
    if (Scores.is_open()){
        std::getline(Scores, line);
        bool ReadingName = true;
        for (int i = 1; i < line.length()-1; i++){
            if (ReadingName){
                if (std::string() + line[i]=="|"){
                    ReadingName = false;
                }
                else{
                    Names[CurrentRead] += line[i];
                }
            }
            else{
                if (std::string() + line[i]==","){
                    TopScores[CurrentRead] = std::stoi(ScoreString[CurrentRead]);
                    ReadingName = true;
                    CurrentRead++;
                }
                else{
                    ScoreString[CurrentRead] += line[i];
                }
            }
        }
        TopScores[CurrentRead] = std::stoi(ScoreString[CurrentRead]);
        for (int i = 0; i < 6; i++){
            if (Names[i] != "") realsize++;
        }
        bubbleSort(TopScores, Names, realsize);
    }
    Scores.close();


    // Setup SDL3
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("PLTW 1.2.2 (C++)", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer);
    // SDL_AddEventWatch(WindowEventFilter, NULL);
    SDL_StartTextInput(window);
    SDL_SetWindowMinimumSize(window, 800, 600);
    SDL_SetRenderVSync(renderer, 1);


    TTF_Init();
    TTF_Font * font = TTF_OpenFont((SDL_GetBasePath()+rpath+"FreeSans.ttf").c_str(), 32);
    TextCharacters Characters = {renderer, font, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890.~!@#$%^&*()_+-=:;\"'?[]/ "};
    float FontHeight = TTF_GetFontHeight(font);


    /* Setup score stuffs */
    TextObject ScoreText = {"Score: 0", Left, Top, Vector2(8, 0), {255, 255, 255, 255}, false, true};
    TextObject CountdownText = {std::to_string(TimeLeft) + " Seconds Left", Right, Top, Vector2(ScreenSpace.x-8, 0), {255, 255, 255, 255}, false};
    TextObject Name = {"Name: ", Center, Top, Vector2(ScreenSpace.x/2, ScreenSpace.y/2), {255, 255, 255, 0}, false, true, 6, 0};
    TextObject BoardRender[5] = {{"] ", Left, Top, Vector2(8, 0), {255, 255, 255, 0}, false},
        {"] ", Left, Top, Vector2(8, FontHeight), {255, 255, 255, 0}, false},
        {"] ", Left, Top, Vector2(8, FontHeight*2), {255, 255, 255, 0}, false},
        {"] ", Left, Top, Vector2(8, FontHeight*3), {255, 255, 255, 0}, false},
        {"] ", Left, Top, Vector2(8, FontHeight*4), {255, 255, 255, 0}, false}
        };


    /* Setup circle */
    SDL_Texture * tutel = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, radius * 2, radius * 2);
    SDL_SetRenderTarget(renderer, tutel);
    SDL_SetRenderDrawColor(renderer, SDL_Color{0, 0, 0, 0});
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, SDL_Color{255, 255, 255, 255});
    for (int i = 0; i < (pow(radius * 2, 2)); i++){
        Vector2 center = {radius, radius};
        Vector2 tempos = {(float)(i % ((int)radius * 2)), (float)(int)(i / (radius * 2))};
        SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, limit(radius-(tempos-center).Magnitude(), 0, 1));
        SDL_RenderPoint(renderer, tempos.x, tempos.y);
    }


    /* Main loop */
    SDL_SetRenderTarget(renderer, NULL);
    while (loop){
        deltime = (SDL_GetTicks() - lastime) / 1000.;
        lastime = SDL_GetTicks();
        TimeLeft -= limit((Score > 0 && TimeLeft > 0)?deltime:0, 0, TimeLeft);


        SDL_SetRenderDrawColor(renderer, {0, 0, 0, 255});
        SDL_RenderClear(renderer);
        mousebitmask = SDL_GetMouseState(&mouse.x, &mouse.y);


        while (SDL_PollEvent(&e)){
            switch (e.type){
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    Name.TrySelect(mouse, false, Characters);
                    if (e.button.button == SDL_BUTTON_LEFT){
                        if ((mouse-pos).Magnitude() <= radius && floor(TimeLeft) > 0) {
                            float newrotation = SDL_randf()*2*M_PI;
                            Vector2 temptarget = target;
                            while (true){
                                target = temptarget + Vector2(std::sin(newrotation)*300, std::cos(newrotation)*300);
                                if (contained(target, {radius, TTF_GetFontHeight(font)+radius, ScreenSpace.x-(radius*2), ScreenSpace.y-(radius*2)-TTF_GetFontHeight(font)})) break;
                                newrotation = SDL_randf()*2*M_PI;
                            }
                            Score++;
                        }
                    }
                    break;


                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    SDL_GetWindowSizeInPixels(window, &ScreenSpace.x, &ScreenSpace.y);
                    CountdownText.Position.x = ScreenSpace.x-8;
                    break;


                case SDL_EVENT_QUIT:
                    loop = false;
                    break;


                case SDL_EVENT_KEY_DOWN:
                    if (e.key.key == SDLK_DELETE || e.key.key == SDLK_BACKSPACE){
                        Name.Delete(e.key.key == SDLK_DELETE);
                    }
                    if (e.key.key == SDLK_LEFT){
                        Name.MoveCursor(keystates[SDL_SCANCODE_LSHIFT] || keystates[SDL_SCANCODE_RSHIFT], keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL], true);
                    }
                    elif (e.key.key == SDLK_RIGHT){
                        Name.MoveCursor(keystates[SDL_SCANCODE_LSHIFT] || keystates[SDL_SCANCODE_RSHIFT], keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL], false);
                    }
                    if (keystates[SDL_MODKEY]){
                        if (e.key.key == SDLK_A){
                            Name.Edit("a", true);
                        }
                        elif (e.key.key == SDLK_X){
                            Name.Edit("x", true);
                        }
                        elif (e.key.key == SDLK_C){
                            Name.Edit("c", true);
                        }
                        elif (e.key.key == SDLK_V){
                            Name.Edit("v", true);
                        }
                    }
                    if (e.key.key == SDLK_RETURN){
                        if (Name.Selected){
                            Name.Editable = false;
                        }
                    }
                    break;


                case SDL_EVENT_TEXT_INPUT:
                    Name.Edit(e.text.text, false);
                    break;
            }
        }


        if (mousebitmask & SDL_BUTTON_LMASK){
            Name.ConTrySelect(mouse, Characters);
        }


        pos = target + ((pos-target) * pow(.5, 12. * deltime));
        rectpos = {pos.x - radius, pos.y - radius, radius * 2, radius * 2};


        if (TimeLeft < 1) {
            if (!endgame){
                endgame = true;
                Name.Editable = true;
                ScoreText.ChangeHorizontalAlignment(Center, Characters);
                ScoreText.ChangeVerticalAlignment(Bottom, Characters);
                CountdownText.Text = "0 Seconds Left";
            }
        }
        else {
            ScoreText.Text = "Score: " + std::to_string(Score);
            CountdownText.Text = std::to_string((int)TimeLeft) + " Seconds Left";
        }


        if (endgame){
            fade += deltime;
            Name.Mod.a = (Uint8)limit(fade*255, 0, 255);
            CountdownText.Mod.a = (Uint8)(255-limit(fade*255, 0, 255));
            SDL_SetTextureAlphaMod(tutel, 255-(Uint8)limit(fade*255, 0, 255));
            ScoreText.Position.x = (ScreenSpace.x/2) + ((ScoreText.Position.x - (ScreenSpace.x/2)) * pow(.5, 12. * deltime));
            ScoreText.Position.y = (ScreenSpace.y/2) + ((ScoreText.Position.y - (ScreenSpace.y/2)) * pow(.5, 12. * deltime));
            Name.Position.x = (ScreenSpace.x/2);
            Name.Position.y = (ScreenSpace.y/2);
            if (!Name.Editable){
                if (rendscorbord){
                    scorefade += deltime;
                    ScoreText.Mod.a = (Uint8)(255-limit(scorefade*255, 0, 255));
                    Name.Mod.a = (Uint8)(255-limit(scorefade*255, 0, 255));
                    for (int i = 0; i < 5; i++){
                        BoardRender[i].Render(renderer, Characters);
                        BoardRender[i].Mod.a = (Uint8)limit(scorefade*255, 0, 255);
                    }
                }
                else{
                    rendscorbord = true;
                    int index = -1;
                    for (int i = 0; i < realsize; i++){
                        if (Names[i] == slice(Name.Text, 6, Name.Text.length())){
                            index = i;
                        }
                    }
                    if (index == -1){
                        TopScores[realsize] = Score;
                        Names[realsize] = slice(Name.Text, 6, Name.Text.length());
                        realsize++;
                    }
                    else{
                        if (TopScores[index] < Score){
                            Names[index] = slice(Name.Text, 6, Name.Text.length());
                            TopScores[index] = Score;
                        }
                    }
                    bubbleSort(TopScores, Names, realsize);
                    for (int i = 0; i < 5; i++){
                        if (i < realsize){
                            BoardRender[i].Text += Names[i] + ": " + std::to_string(TopScores[i]);
                        }
                    }

                    std::string returnv = "{";
                    for (int i = 0; i < limit(realsize, 0, 5); i++){
                        returnv += Names[i] + "|" + std::to_string(TopScores[i]);
                        if (i < limit(realsize, 0, 5)-1){
                            returnv += ",";
                        }
                    }
                    returnv += "}";
                    std::ofstream ScoreFile(SDL_GetBasePath()+rpath+"Scores");
                    ScoreFile << returnv << std::endl;
                    ScoreFile.close();
                }
            }
        }


        /* Render score and countdown */
        ScoreText.Render(renderer, Characters);
        CountdownText.Render(renderer, Characters);
        Name.Render(renderer, Characters);


        SDL_RenderTexture(renderer, tutel, nullptr, &rectpos);
        SDL_RenderPresent(renderer);
    }


    SDL_StopTextInput(window);
    SDL_DestroyTexture(tutel);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}


#ifdef _WIN32
/* Windows window subsystem :( */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    return main(__argc, __argv);
}
#endif
