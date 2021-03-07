#pragma once
#include "neuralNetworkShell.h"
#include "snakeGame.h"
#include "main.h"

#define mutationRate 0.25
#define populationSize 10000

#define max(a,b) (a>b)?a:b

void initiliseTrainingData(neuralNetwork**);
void destoryTrainingData(neuralNetwork**);
void randomisePopulation(neuralNetwork**);
long double getFitness(neuralNetwork*, int);
double getGenerationFitness(neuralNetwork**, double*, int);
void mate(neuralNetwork*, neuralNetwork*, neuralNetwork*);
void mutate(neuralNetwork*, int);
int selectParent(neuralNetwork**, int, double*);
int getBestBrain(double*);
void trainNetwork(neuralNetwork**, int);
void getInputs(neuralNetwork*, snake*, board*);
int getOutput(neuralNetwork*);