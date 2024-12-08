#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>
#include "Vector2.h"
#include "MoreMaths.h"
#define elif else if

typedef int Alignment;
#define Top 0
#define Left 0
#define Right 2
#define Bottom 2
#define Center 1


// Character cache
class TextCharacters{
public:
TextCharacters(SDL_Renderer * renderer, TTF_Font * font, std::string characters);
SDL_Renderer * GetRenderer();
SDL_Texture * GetCharacter(std::string character);
float GetTotalLength(std::string characters);
TTF_Font * GetFont();
private:
TTF_Font * Font;
std::string Charin;
SDL_Renderer * Renderer;
std::vector<SDL_Texture *> Characters;
};


inline TextCharacters::TextCharacters(SDL_Renderer * renderer, TTF_Font * font, std::string characters){
    SDL_Surface * tempsurf;
    Font = font;
    Renderer = renderer;
    Charin = characters;

    for (int i = 0; i < characters.length(); i++){
        tempsurf = TTF_RenderText_Blended(Font, (std::string() + characters[i]).c_str(), 1, {255, 255, 255, 255});
        Characters.push_back(SDL_CreateTextureFromSurface(Renderer, tempsurf));
        SDL_DestroySurface(tempsurf);
    }
}


inline SDL_Texture * TextCharacters::GetCharacter(std::string character){
    return Characters[Charin.find(character)];
}


inline float TextCharacters::GetTotalLength(std::string characters){
    float value = 0;
    float addvalue = 0;
    int kerning;
    for (int i = 0; i < characters.length(); i++){
        SDL_GetTextureSize(Characters[Charin.find(characters[i])], &addvalue, nullptr);
        TTF_GetGlyphKerning(GetFont(), (int)characters[i], (int)characters[i+1], &kerning);
        value += addvalue + kerning;
    }
    return value;
}


inline SDL_Renderer * TextCharacters::GetRenderer(){
    return Renderer;
}


inline TTF_Font * TextCharacters::GetFont(){
    return Font;
}


// Actual object to render text
class TextObject{
public:
TextObject(std::string text, Alignment horizontal, Alignment vertical, Vector2 position, SDL_Color mod, bool editable, bool visibleWhenEmpty = true, int editStart = 0, int editEnd = 0);
void Render(SDL_Renderer * renderer, TextCharacters Characters);
void MoveCursor(bool Shift, bool Control, bool MoveLeft);
void Edit(std::string InputChars, bool Modifier);
void Delete(bool Reverse);
void Destroy();
void TrySelect(Vector2 CursorPosition, bool Shift, TextCharacters Characters);
void ConTrySelect(Vector2 CursorPosition, TextCharacters Characters);
void ChangeHorizontalAlignment(Alignment horizontal, TextCharacters Characters);
void ChangeVerticalAlignment(Alignment vertical, TextCharacters Characters);
bool Editable;
int EditStart = 0;
int EditEnd = 0;
bool VisibleWhenEmpty;
bool Selected = false;
std::string Text;
Alignment Horizontal;
Alignment Vertical;
Vector2 Position;
SDL_Color Mod;
private:
int Cursor;
int Selection;
};


inline TextObject::TextObject(std::string text, Alignment horizontal, Alignment vertical, Vector2 position, SDL_Color mod, bool editable, bool visibleWhenEmpty, int editStart, int editEnd){
    Text = text;
    Horizontal = horizontal;
    Vertical = vertical;
    Position = position;
    Editable = editable;
    EditStart = editStart;
    EditEnd = editEnd;
    VisibleWhenEmpty = visibleWhenEmpty;
    Mod = mod;
    Cursor = -1;
    Selection = 0;
}


inline void TextObject::TrySelect(Vector2 CursorPosition, bool Shift, TextCharacters Characters){
    float FontHeight = (float)TTF_GetFontHeight(Characters.GetFont());
    float TotaLength = Characters.GetTotalLength(Text);
    float BasePos = (TotaLength*((float)Horizontal/2));
    SDL_FRect TestRange = {Position.x - BasePos-8, Position.y - (FontHeight*((float)Vertical/2)), TotaLength+16, FontHeight};
    if (Text.length() == 0) TestRange = {Position.x - 8, Position.y - 8, 16, 16};
    if (contained(CursorPosition, TestRange)){
        float distance = TotaLength;
        for (int i = 0; i < Text.length()+1; i++){
            if (CursorPosition.x > (Position.x - BasePos + Characters.GetTotalLength(slice(Text, 0, i)))){
                distance = abs(CursorPosition.x - (Position.x - BasePos + Characters.GetTotalLength(slice(Text, 0, i))));
            }
            else{
                if (Shift && Cursor != -1){
                    Selection = i-(distance < abs(CursorPosition.x - (Position.x - BasePos + Characters.GetTotalLength(slice(Text, 0, i)))))-Cursor;
                }
                else{
                    Cursor = i-(distance < abs(CursorPosition.x - (Position.x - BasePos + Characters.GetTotalLength(slice(Text, 0, i)))));
                    Selection = 0;
                }
                break;
            }
            if (i == Text.length()-1){
                if (Shift && Cursor != -1){
                    Selection = i+1-Cursor;
                }
                else{
                    Cursor = i+1;
                    Selection = 0;
                }
            }
        }
        Selected = inlimit(Cursor, EditStart, Text.length()-EditEnd+1);
    }
    else{
        Selected = false;
    }
}


inline void TextObject::ConTrySelect(Vector2 CursorPosition, TextCharacters Characters){
    float TotaLength = Characters.GetTotalLength(Text);
    float BasePos = (TotaLength*((float)Horizontal/2));
    float distance = TotaLength;
    for (int i = 0; i < Text.length()+1; i++){
        TotaLength = Characters.GetTotalLength(slice(Text, 0, i));
        if (CursorPosition.x > (Position.x - BasePos + TotaLength)){
            distance = abs(CursorPosition.x - (Position.x - BasePos + TotaLength));
        }
        else{
            Selection = i-(distance < abs(CursorPosition.x - (Position.x - BasePos + TotaLength))) - Cursor;
            break;
        }
        if (i == Text.length()-1) Selection = i+1 - Cursor;
    }
    if (Cursor+Selection < EditStart){
        Selection = EditStart-Cursor;
    }
    if (Cursor+Selection > Text.length()-EditEnd){
        Selection = Text.length()-EditEnd-Cursor;
    }
}


inline void TextObject::MoveCursor(bool Shift, bool Control, bool MoveLeft){
    if (Selected){
        if (Shift){
            Selection -= (MoveLeft-0.5)*2;
            Selection = limit(Selection, -Cursor, Text.length()-Cursor);
            if (Cursor+Selection < EditStart){
                Selection = EditStart-Cursor;
            }
            if (Cursor+Selection > Text.length()-EditEnd){
                Selection = Text.length()-EditEnd-Cursor;
            }
        }
        else{
            Cursor -= (MoveLeft-0.5)*2 - Selection;
            Cursor = limit(Cursor, EditStart, Text.length()-EditEnd);
            Selection = 0;
        }
    }
}


inline void TextObject::Edit(std::string InputChars, bool Modifier){
    if (Selected){
        if (Modifier){
            if (InputChars == "a"){
                Cursor = EditStart;
                Selection = Text.length()-EditEnd-Cursor;
            }
            elif (InputChars == "c"){
                SDL_SetClipboardText(slice(Text, (Selection<0)?Cursor+Selection:Cursor, (Selection>0)?Cursor+Selection:Cursor).c_str());
            }
            elif (InputChars == "x"){
                if (Selection != 0){
                    SDL_SetClipboardText(slice(Text, (Selection<0)?Cursor+Selection:Cursor, (Selection>0)?Cursor+Selection:Cursor).c_str());
                    Delete(false);
                }
            }
            elif (InputChars == "v"){
                Edit(SDL_GetClipboardText(), false);
            }
        }
        else{
            if (Selection == 0){
                Text = slice(Text, 0, Cursor)+InputChars+slice(Text, Cursor, Text.length());
                Cursor += InputChars.length();
            }
            else{
                Text = slice(Text, 0, (Selection<0)?Cursor+Selection:Cursor)+InputChars+slice(Text, (Selection>0)?Cursor+Selection:Cursor, Text.length());
                Cursor = (Selection<0)?Cursor+Selection+InputChars.length():Cursor+InputChars.length();
                Selection = 0;
            }
        }
    }
}


inline void TextObject::Delete(bool Reverse){
    if (Selected){
        if (Selection == 0){
            if (Reverse){
                if (Cursor<Text.length()-EditEnd) Text = slice(Text, 0, Cursor)+slice(Text, Cursor+1, Text.length());
            }
            else{
                if (Cursor>EditStart){
                    Text = slice(Text, 0, Cursor-1)+slice(Text, Cursor, Text.length());
                    Cursor--;
                }
            }
        }
        else{
            Text = slice(Text, 0, (Selection<0)?Cursor+Selection:Cursor)+slice(Text, (Selection>0)?Cursor+Selection:Cursor, Text.length());
            Cursor = (Selection<0)?Cursor+Selection:Cursor;
            Selection = 0;
        }
    }
}


inline void TextObject::ChangeHorizontalAlignment(Alignment horizontal, TextCharacters Characters){
    float FontWidth = Characters.GetTotalLength(Text);
    Position.x += (FontWidth*((float)horizontal/2)) - (FontWidth*((float)Horizontal/2));
    Horizontal = horizontal;
}


inline void TextObject::ChangeVerticalAlignment(Alignment vertical, TextCharacters Characters){
    float FontHeight = (float)TTF_GetFontHeight(Characters.GetFont());
    Position.y += ((FontHeight/2) + (FontHeight*((float)vertical/2))) - ((FontHeight/2) + (FontHeight*((float)Vertical/2)));
    Vertical = vertical;
}


inline void TextObject::Render(SDL_Renderer * renderer, TextCharacters Characters){
    float FontHeight = (float)TTF_GetFontHeight(Characters.GetFont());
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Texture * curchar;
    SDL_FRect charect;
    charect.x = Position.x - (Characters.GetTotalLength(Text)*((float)Horizontal/2));
    charect.y = Position.y - (FontHeight*((float)Vertical/2));
    // Render editing stuffs like text cursor
    if (Selected && Editable){
        Cursor = limit(Cursor, 0, Text.length());
        if (Text.length() == 0){
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderLine(renderer, Position.x, Position.y - (FontHeight/4), Position.x, Position.y + (FontHeight/4));
        }
        else{
            if (Selection != 0){
                SDL_SetRenderDrawColor(renderer, 50, 50, 128, 128);
                SDL_FRect SelecRect = {charect.x+Characters.GetTotalLength(slice(Text, 0, Cursor + Selection)), charect.y, ((Selection>0)?-1:1)*Characters.GetTotalLength(slice(Text, ((Selection<0)?Cursor+Selection:Cursor), ((Selection>0)?Cursor+Selection:Cursor))), FontHeight+1};
                SDL_RenderFillRect(renderer, &SelecRect);
            }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderLine(renderer, charect.x+Characters.GetTotalLength(slice(Text, 0, Cursor + Selection)), charect.y, charect.x+Characters.GetTotalLength(slice(Text, 0, Cursor + Selection)), charect.y+FontHeight);
        }
    }
    else{
        if (Text.length() == 0 && VisibleWhenEmpty){
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 64);
            SDL_FRect TempRectForFillWhyDoesItNeedAPointerThatsSoSillyAndStupidButIGuessItMakesSenseConsideringYouDontWantToConstantlyCopyDataLikeIDoAllTheTimeInMyCPPProgramsOopsiesLol = {Position.x - (FontHeight/4), Position.y - (FontHeight/4), FontHeight/2, FontHeight/2};
            SDL_RenderFillRect(renderer, &TempRectForFillWhyDoesItNeedAPointerThatsSoSillyAndStupidButIGuessItMakesSenseConsideringYouDontWantToConstantlyCopyDataLikeIDoAllTheTimeInMyCPPProgramsOopsiesLol);
        }
        Cursor = -1;
        Selection = 0;
    }

    // Render text!1!
    int kerning;
    for (int i = 0; i < Text.length(); i++){
        curchar = Characters.GetCharacter(std::string() + Text[i]);
        SDL_SetTextureColorMod(curchar, Mod.r, Mod.g, Mod.b);
        SDL_SetTextureAlphaMod(curchar, Mod.a);
        SDL_GetTextureSize(curchar, &charect.w, &charect.h);
        SDL_RenderTexture(renderer, curchar, NULL, &charect);
        TTF_GetGlyphKerning(Characters.GetFont(), (int)Text[i], (int)Text[i+1], &kerning);
        charect.x += charect.w + kerning;
    }
}
