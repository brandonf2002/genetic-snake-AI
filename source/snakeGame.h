#pragma once
#define HEIGHT 600
#define WIDTH 600

#define GRID_SIZE 20

struct snake {
	int* x;
	int* y;
	int score;
	int time;
	int alive;
	int direction; 		//0-right, 1-down, 2-left, 3-up
	int move;			//0-forward, 1-turn right, -1-turn left
	int hasAte;
};
typedef struct snake snake;

struct board {
	int height;
	int width;
	int foodX;
	int foodY;
};
typedef struct board board;

void initiliseSnakeAndBoard(snake*, board*);
void placeFood(board*, snake*);
int snakeCollison(snake*, board*);
int snakeFoodCollsion(snake*, board*);
void updateSnake(snake*, board*);