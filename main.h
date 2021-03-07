#pragma once

#include "snakeGame.h"
#include "snakeGraphics.h"
#include "neuralNetworkShell.h"
#include "neuralNetworkData.h"

int playHuman(renderWindow*, snake*, board*);
int playCompTrain(neuralNetwork*, snake*, board*);
int playCompTest(renderWindow*, neuralNetwork*, snake*, board*);