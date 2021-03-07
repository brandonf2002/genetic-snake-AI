#pragma once
#include <SDL2/SDL.h> 
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_ttf.h>

#include "snakeGame.h"

struct renderWindow {
	SDL_Window* window; 			//The physical window that everything is displayed to
	SDL_Renderer* renderer;			//The renderer that renders pixels to the window above
	SDL_Surface* surface;			//A surface to use, used for text in this program
	SDL_Texture* texture;			//A texture to use, used for text in this program 
	TTF_Font* font;					//A true text font to be used
};
typedef struct renderWindow renderWindow;

int initiliseSDL();
void destroySDL();
int initiliseWindow(renderWindow*, const char*, int, int);
void drawText(renderWindow*, const char*, int, int);
void renderBoard(renderWindow*, snake*, board*);
void destroyWindow(renderWindow*);