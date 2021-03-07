#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "snakeGame.h"
#include "snakeGraphics.h"
#include "geneticNeuralNetwork.h"

/**
 * @brief This function resets the boards and snake values.
 * @param s - the snake
 * @param b - the board
 * @return nothing
 */
void initiliseSnakeAndBoard(snake* s, board* b) {
    s->x = calloc((1) + 2, sizeof(int));	//x-cords
    s->y = calloc((1) + 2, sizeof(int)); 	//y-cords
    s->score = 1; 				    	//score
    s->time = 0; 						//time
    s->alive = 1; 						//alive
    s->direction = 0; 						//direction
    s->hasAte = 0;						//hasAte

	b->height = HEIGHT/GRID_SIZE; 
    b->width = WIDTH/GRID_SIZE;
    placeFood(b, s);

	s->x[0] = b->width/(2);
	s->y[0] = b->height/(2);
}

/**
 * @brief This function places food on the board, but not on the snake
 * @param b - the board, where the food is placed to.
 * @param s - the snake, used to get all of the snakes current positions.
 * @return nothing
 */
void placeFood(board* b, snake* s) {
	int isInSnake;
	do {
		isInSnake = 0;
		b->foodX = rand() % (b->width);
		b->foodY = rand() % (b->height);
		for (int i = 0; i < s->score - s->hasAte; i++) {
			if (b->foodX == s->x[i] && b->foodY == s->y[i]) {
				isInSnake = 1;
				break;
			}
		}
	} while (isInSnake);
}

/**
 * @brief This function checks to see if the snake is touching one its tail.
 * @param s - a snake struct, stores all the relivent information about the snake
 * @return 1 if the snake has collided with itself, 0 otherwise
 */
int snakeCollison(snake* s, board* b) {
	for (int i = 1; i < s->score - s->hasAte; i++) 
		if (s->x[0] == s->x[i] & s->y[0] == s->y[i])
			return 1;

	if (s->x[0] < 0 | s->x[0] >= b->width | s->y[0] < 0 | s->y[0] >= b->height) 
		return 1;

	return 0;
}

int snakeFoodCollsion(snake* s, board* b) {
    if (s->x[0] == b->foodX & s->y[0] == b->foodY) {
		return 1;
    }

	return 0;
}

/*
 * @brief This function updates the snakes position once per "tick", moves each array index one position up, simulating movement
 * @param s - the snake struct
 * @return nothing
 */
void updateSnake(snake* s, board* b) {
	if (snakeCollison(s, b)) 
		s->alive = 0;

	else if (snakeFoodCollsion(s, b)){
		s->hasAte += 4;
        s->score += 4;
        s->x = realloc(s->x, s->score*sizeof(int));
        s->y = realloc(s->y, s->score*sizeof(int));
        placeFood(b, s);
	}

	s->direction = (4 + s->direction + s->move) % 4;
	s->move = 0;

	for (int i = s->score - s->hasAte; i > 0; i--) {
		s->x[i] = s->x[i-1];
		s->y[i] = s->y[i-1];
	}
	
	switch(s->direction) {
	case 3:
		s->y[0]--;
		break;
	case 1:
		s->y[0]++;
		break;
	case 2:
		s->x[0]--;
		break;
	case 0:
		s->x[0]++;
		break;
	}

	if (s->hasAte) s->hasAte--;
	s->time++;
}