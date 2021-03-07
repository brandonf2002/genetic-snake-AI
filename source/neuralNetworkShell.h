#pragma once

#include <stdarg.h>

#define crelu(x)(x>0.0?x:x*0.1)
#define creluPrime(x)(x>0.0?1.0:0.1)

struct neuralNetwork {
    double*** weights;
    double** biases;
    double** outputs;
    int* networkLayout;
    int networkSize;
};
typedef struct neuralNetwork neuralNetwork;

void setNetworkLayout(neuralNetwork*, int, ...);
void initialiseNetworkBrain(neuralNetwork*);
void destroyBrainData(neuralNetwork*);
void randomiseNetwork(neuralNetwork*);
void deepCopy(neuralNetwork*, neuralNetwork*);
void assignInputs(neuralNetwork*, double*);
void frontPropegation(neuralNetwork*, int);
void saveBrain(neuralNetwork*, const char*);
void loadBrain(neuralNetwork*, const char*);