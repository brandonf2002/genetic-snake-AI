#include <SDL2/SDL.h> 
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h> 

#include "snakeGraphics.h"
#include "snakeGame.h"

/**
 * @brief This function initilises SDL.
 * @return 0 if there is a failure, 1 if success.
 */
int initiliseSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_failed");
		return 0;
	}

	if (TTF_Init() == -1) {
		printf("TTF_failed");
		return 0;
	}
    return 1;
}

/**
 * @brief This function unititilises all of the SDL data.
 * @return nothing
 */
void destroySDL() {
	SDL_Quit();
	TTF_Quit();
} 

/**
 * @brief This function creates a window and a rendering surface on that window.
 * @param win - pointer to a window pointer, is the main window that the game is diplayed to.
 * @param name - the name the window will display
 * @param height - the height of the window
 * @param width - the width of the window
 * @return 1 if all went well, 0 if something went wrong
 */
int initiliseWindow(renderWindow* win, const char* name, int height, int width) {
	win->window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (win->window ==  NULL)
		return 0;

	win->renderer = SDL_CreateRenderer(win->window, -1, SDL_RENDERER_ACCELERATED);
	if (win->renderer == NULL)
		return 0;

	win->font = TTF_OpenFont("./text.ttf", 24);
	if (win->font == NULL)
		return 0;

	return 1;
}

/**
 * @brief This function frees the memory allocated to the stucts containing window information.
 * @param win - the structure containing the window information.
 * @return nothing
 */
void destroyWindow(renderWindow* win) {
	SDL_DestroyRenderer(win->renderer);
	SDL_DestroyWindow(win->window);
	TTF_CloseFont(win->font);
	free(win);
}

/**
 * @brief This function draws text to the given window
 * @param win - A struct containing the window and rendering details
 * @param text - The text to be written to the renderer
 * @param x - the x cord of the text
 * @param y - the y cord of the text
 * @return nothing
 */
void drawText(renderWindow* win, const char* text, int x, int y) {
	SDL_Color color = {255, 255, 255, 128};

	win->surface = TTF_RenderText_Solid(win->font, text, color);
	win->texture = SDL_CreateTextureFromSurface(win->renderer, win->surface);

	SDL_Rect textBox;
	textBox.x = x;
	textBox.y = y;

	SDL_QueryTexture(win->texture, NULL, NULL, &textBox.w, &textBox.h);
	SDL_RenderCopy(win->renderer, win->texture, NULL, &textBox);

	SDL_DestroyTexture(win->texture);
	SDL_FreeSurface(win->surface);
}

/**
 * @brief This function renders each new frame to the window, connected through the rendered defined in the "initiliseWindow" function.
 * @param b - the game board.
 * @param s - the snake.
 * @param renderer - where all the graphics are rendered to, is linked to the window through the "initilseWindow" funtion
 * @param font - the font to be used.
 */
void renderBoard(renderWindow* win, snake* s, board* b) {
	SDL_SetRenderDrawColor(win->renderer, 0, 0, 0, 255);
	SDL_RenderClear(win->renderer);

	SDL_Rect foodImg = {(b->foodX*GRID_SIZE)-1, (b->foodY*GRID_SIZE)-1, GRID_SIZE-1, GRID_SIZE-1};
	SDL_SetRenderDrawColor(win->renderer, 0, 255, 0, 0);
	SDL_RenderFillRect(win->renderer, &foodImg);

	for (int i = 0; i < s->score - s->hasAte; i++) {
		SDL_Rect snakeImg = {(s->x[i]*GRID_SIZE)-1, (s->y[i]*GRID_SIZE)-1, GRID_SIZE-1, GRID_SIZE-1};
		SDL_SetRenderDrawColor(win->renderer, 255, 0, 0, 0);
		SDL_RenderFillRect(win->renderer, &snakeImg);
	}

	char temp[256];

	sprintf(temp, "Score: %d", s->score);
	drawText(win, temp, 10, 10);

	sprintf(temp, "Time: %d", s->time/10);
	drawText(win, temp, (b->width*GRID_SIZE)-90, 10);

	SDL_RenderPresent(win->renderer);
}