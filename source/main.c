#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "snakeGame.h"
#include "snakeGraphics.h"
#include "geneticNeuralNetwork.h"
#include "main.h"

/**
 * @brief This function initilises the game vars and plays it.
 * @param win - the game window and the renderer
 * @param s - the snake
 * @param b - the board
 * @return -1 if the user quits, returns 0 if they die
 */
int playHuman(renderWindow* win, snake* s, board* b) {
    initiliseSnakeAndBoard(s, b);
	SDL_Event e;
	Uint32 time;
	int keyDown = 0;

	while (s->alive) {
		renderBoard(win, s, b);
		time = SDL_GetTicks();

		while (!SDL_TICKS_PASSED(SDL_GetTicks(), time+100) || SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT) {
				return -1;
			} else if (e.type == SDL_KEYUP) {
				keyDown = 0;
			} else if (e.type == SDL_KEYDOWN && !keyDown) {
				switch (e.key.keysym.sym) {
				case SDLK_RIGHT:
					s->move = 1;
					break;
				case SDLK_LEFT:
					s->move = -1;
					break;
				}
				keyDown = 1;
			}
		}
		updateSnake(s, b);
	}
	return 0;
}

/**
 * @brief This function initilises the game vars and plays it, used to train the nn
 * @param nn - the neural network to play the game
 * @param s - the snake
 * @param b - the board
 * @return nothing
 */
int playCompTrain(neuralNetwork* nn, snake* s, board* b) {
    initiliseSnakeAndBoard(s, b);
	int ticksSinceAteFood = 50;
	while (s->alive && ticksSinceAteFood > 0) {
		if (s->x[0] == b->foodX & s->y[0] == b->foodY) 
			ticksSinceAteFood += 150;

		getInputs(nn, s, b);
		frontPropegation(nn, 0);
		s->move = getOutput(nn) - 1;		//nn outputs 0 for left, 1 for forward, 2 for right, one more than the game;

		updateSnake(s, b);
		ticksSinceAteFood--;
	}
}

/**
 * @brief This function initilises the game vars and plays it, shows the outputs of the
 * 		  nn to the user.
 * @param win - the window that will be rendered
 * @param nn - the neural network to play the game
 * @param s - the snake
 * @param b - the board
 * @return nothing
 */
int playCompTest(renderWindow* win, neuralNetwork* nn, snake* s, board* b) {
    initiliseSnakeAndBoard(s, b);
	SDL_Event e;
	Uint32 time;

	int ticksSinceAteFood = 100;
	while (s->alive && ticksSinceAteFood > 0) {
		renderBoard(win, s, b);
		time = SDL_GetTicks();

		if (s->x[0] == b->foodX & s->y[0] == b->foodY) 
			ticksSinceAteFood += 150;

		while (!SDL_TICKS_PASSED(SDL_GetTicks(), time+20) || SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT) 
				return -1;
		}

		getInputs(nn, s, b);

		for (int i = 0; i < 16; i++) {
			printf("%d: %f\n", i, nn->outputs[0][i]);
		}
		printf("\n");

		frontPropegation(nn, 0);
		s->move = getOutput(nn) - 1;		//nn outputs 0 for left, 1 for forward, 2 for right, one more than the game;

		updateSnake(s, b);
        ticksSinceAteFood--;
	}
	return 0;
}

int main(int argc, char** argv) {
	srand(time(NULL));

	if (argc < 2) {
		printf("Enter a valid command:\n");
		printf("Play:\t\tplay\n");
		printf("Train:\t\ttrain\n");
		printf("Test:\t\ttest\n");
	}
	renderWindow* win = malloc(sizeof(renderWindow));
	snake* s = malloc(sizeof(snake));
	board* b = malloc(sizeof(board));

	if (!strcmp(argv[1], "train")) {
		neuralNetwork** population = calloc(populationSize, sizeof(neuralNetwork*));
		initiliseTrainingData(population);
		randomisePopulation(population);
		trainNetwork(population, 100);
		//save population
	}

	else if (!strcmp(argv[1], "play") || !strcmp(argv[1], "test")) {
		if (!initiliseSDL()) {
			printf("SDL Initilisation Failed");
			return 1;
		}

		if (!initiliseWindow(win, "snake", HEIGHT, WIDTH)) {
			printf("Window Initilisation Failed");
			return 1;
		}

		if (!strcmp(argv[1], "play")) {
			while(playHuman(win, s, b) != -1);
		}

		else if (!strcmp(argv[1], "test")) {
			neuralNetwork* nn = malloc(sizeof(neuralNetwork));
			initialiseNetworkBrain(nn);
			loadBrain(nn, "brains/Generation_132");
			while(playCompTest(win, nn, s, b) != -1);
		}
	}

	free(s);
	free(b);
	destroyWindow(win);
	destroySDL();
	
	return 0;
}