#include <stdlib.h>
#include <math.h>

#include "neuralNetworkShell.h"
#include "geneticNeuralNetwork.h"
#include "snakeGame.h"
#include "main.h"

/**
 * @brief This function allocates the memory for the training data that we need
 * @param population - the collection of networks in each population
 * @param fitness - the fitnesses of each member of the population
 * @return nothing
 */
void initiliseTrainingData(neuralNetwork** population) {
	for (int i = 0; i < populationSize; i++) {
		population[i] = calloc(1, sizeof(neuralNetwork));
		initialiseNetworkBrain(population[i]);
	}
}

/**
 * @brief This function frees the memory for the training data that we need
 * @param population - the collection of networks in each population
 * @param fitness - the fitnesses of each member of the population
 * @return nothing
 */
void destoryTrainingData(neuralNetwork** population) {
	for (int i = 0; i < populationSize; i++) {
		destroyBrainData(population[i]);
		population[i] = calloc(1, sizeof(neuralNetwork));
	}
	free(population);
}

/**
 * @brief This function generates a new, random population group.
 * @param population - the double pointer to store all of the individuals
 * @return nothing
*/
void randomisePopulation(neuralNetwork** population) {
	for (int i = 0; i < populationSize; i++) {
		randomiseNetwork(population[i]);
	}
}

/**
 * @brief This function gets the fitness score for an individual neural network.
 * @param nn - a pointer to the neural network
 * @return the fitness of the network
 */
long double getFitness(neuralNetwork* nn, int gen) {
	long double scores = 0;
	snake* s = malloc(sizeof(snake));
	board* b = malloc(sizeof(board));
	initiliseSnakeAndBoard(s, b);

	for (int i = 0; i < 3; i++) { 
		playCompTrain(nn, s, b);
		//scores = s->score;
		scores += pow(2, s->score) * ((double)(s->time)/150);
		//scores += s->score;
		//scores += log10(((double)s->time) * pow(2, s->score));
	}

	free(s);
	free(b);
	return scores/3;
}

/**
 * @brief This function generates and stores the fitness of the entire population.
 * @param nn - the entire generation of neural networks
 * @param fitness - the fitness scores of the neural networks
 * @return the average score of the whole generation
 */
double getGenerationFitness(neuralNetwork** nn, double* fitness, int gen) {
	long double averageScore = 0;
	for (int i = 0; i < populationSize; i++) {
        fitness[i] = getFitness(nn[i], gen);
		averageScore += fitness[i];
	}
	return (averageScore/(double)populationSize);
}

/**
 * @brief This function creates a child by splicing the "genome" of 2 parents
 * @param parent1 - the first of the selected parents
 * @param parent2 - the other parent whos genes will be spliced
 * @param child - the outputed network of this algorithm
 * @return nothing
 */
void mate(neuralNetwork* parent1, neuralNetwork* parent2, neuralNetwork* child) {
	int totalNetworkDataPoints = 0;
	for (int i = 0; i < parent1->networkSize-1; i++) 
		totalNetworkDataPoints += (parent1->networkLayout[i] * parent1->networkLayout[i+1]) + parent1->networkLayout[i+1];

    int splitPoint1 = rand()%totalNetworkDataPoints;
    int splitPoint2 = rand()%(totalNetworkDataPoints - splitPoint1) + splitPoint1 + 1;

	int currPoint = 0;
	neuralNetwork* currentParent = parent1;
	for (int i = 0; i < parent1->networkSize - 1; i++) {
		for (int j = 0; j < parent1->networkLayout[i+1]; j++) {
			for (int k = 0; k < parent1->networkLayout[i]; k++) {
				child->weights[i][k][j] = currentParent->weights[i][k][j];
				currPoint++;
				currentParent = (i >= splitPoint1 && i <= splitPoint2) ? parent2 : parent1;
			}

			child->biases[i][j] = currentParent->biases[i][j];
			currPoint++;
			currentParent = (i >= splitPoint1 && i <= splitPoint2) ? parent2 : parent1;
		}
	}
}

/**
 * @brief This function will mutate a random weight or bias, adding/subrating a small value from it.
 * @param nn - the neural network to mutate
 * @return nothing
 */
void mutate(neuralNetwork* nn, int generation) {
	for (int i = 0; i < nn->networkSize - 1; i++) {
		for (int j = 0; j < nn->networkLayout[i+1]; j++) {
			for (int k = 0; k < nn->networkLayout[i]; k++) {
				if (rand() < mutationRate) 
					nn->weights[i][k][j] *= (((double)rand() / RAND_MAX) * 0.2) + 0.9;
					//nn->weights[i][j][k] = (((double)rand() / RAND_MAX) * 4) - 2;
			}

			if (rand() < mutationRate) 
				nn->biases[i][j] *= (((double)rand() / RAND_MAX) * 0.2) + 0.9;
				//nn->biases[i][j] = (((double)rand() / RAND_MAX) * 4) - 2;
		}
	}
}

/**
 * @brief This function selects a parent using tournament selection
 * @param tournamentSize - the number of candidates to choose from
 * @param fitness - the fitnesses of all of the networks
 * @return the best network from the randomly selected few
 */
int selectParent(neuralNetwork** candidates, int tournamentSize, double* fitness) {
	int bestIndex = rand()%populationSize;
	double bestFitness = fitness[bestIndex];

	for (int i = 0; i < tournamentSize - 1; i++) {
		int randomIndex = rand()%populationSize;
		double randomFitness = fitness[randomIndex];
		if (randomFitness > bestFitness) {
			bestIndex = randomIndex;
			bestFitness = randomFitness;	
		}
	}

	return bestIndex;
}

/**
 * @brief This function get the index of the neural network with the highest fitness
 * 		  Used for elitism.
 * @param fitness - the fitness of all of the members of the population
 * @return The index of the "smartest" brain
 */
int getBestBrain(double* fitness) {
	int bestIndex = 0;

	for (int i = 1; i < populationSize; i++)
		if (fitness[i] >= fitness[bestIndex]) 
			bestIndex = i;

	return bestIndex;
}

/**
 * @brief This function trains the neural network for a given number of generations
 * @param population - generation 1 of the training session
 * @param generations - the number of generations to produce
 * @return nothing
 */
void trainNetwork(neuralNetwork** population, int generations) {
	neuralNetwork** nextPopulation = calloc(populationSize, sizeof(neuralNetwork*));
	double* fitness = calloc(populationSize, sizeof(double));

	initiliseTrainingData(nextPopulation);
	randomisePopulation(population);
	
	for (int i = 0; i < generations; i++) {
		double averageFitness = getGenerationFitness(population, fitness, i);
		//if ((i+1)%20 == 0)
			printf("Average fitness for Generation%d: %lf\n", i+1, averageFitness);
		
		for (int j = 0; j < populationSize-1; j++) {
			mate(
				population[selectParent(population, 15, fitness)], 
				population[selectParent(population, 15, fitness)], 
				nextPopulation[j]
			);

			mutate(nextPopulation[j], i);
		}

		int bestBrain = getBestBrain(fitness);
		char temp[4096];
		sprintf(temp, "brains/Generation_%d", i+1);
		saveBrain(population[bestBrain], temp);

		deepCopy(population[bestBrain], population[0]);
		for (int j = 0; j < populationSize-1; j++) 
			deepCopy(nextPopulation[j], population[j+1]);
	}

	destoryTrainingData(nextPopulation);
	free(fitness);
}

/**
 * @brief This function parses the snake game data to nn inputs.
 * @param nn - the neural network that the stores the parced input
 * @param s - the snake struct, passes not as a refrence because it gets modified, end snake should not
 * 	      change in theory but better safe than sorry.
 * @param b - the board struct, has the position of the food and the board dims
 */
void getInputs(neuralNetwork* nn, snake* s, board* b) {
	double distance;
	double maxDistance[] = {
		(b->width - s->x[0]) + 1,
		(b->height - s->y[0]) + 1,
		b->width - (b->width - s->x[0]) + 1,
		b->height - (b->height- s->y[0]) + 1
	};
	int hasCollided;
	snake* newSnake;
	newSnake = malloc(sizeof(snake));
	newSnake->score = s->score;
	newSnake->hasAte = s->hasAte;
	newSnake->x = calloc(newSnake->score, sizeof(int));
	newSnake->y = calloc(newSnake->score, sizeof(int));

	/*
		These loops and switches are kinda fugly, should be updated at some point
	*/
	for (int i = s->direction; i < s->direction + 8; i++) {
		distance = 0;
		hasCollided = 0;

		//reset the temp snakes position
		for (int j = 0; j < newSnake->score; j++) {
			newSnake->x[j] = s->x[j];
			newSnake->y[j] = s->y[j];
		}

		while(!hasCollided && distance < maxDistance[i%4]) {
			switch (i%8) {
			case 0:
				newSnake->x[0]++;
				break;
			case 1:
				newSnake->y[0]++;
				break;
			case 2:
				newSnake->x[0]--;
				break;
			case 3:
				newSnake->y[0]--;
				break;
			case 4:
				newSnake->x[0]++;
				newSnake->y[0]++;
				break;
			case 5:
				newSnake->y[0]++;
				newSnake->x[0]--;
				break;
			case 6:
				newSnake->x[0]--;
				newSnake->y[0]++;
				break;
			case 7:
				newSnake->y[0]--;
				newSnake->x[0]++;
				break;
			}

			hasCollided = snakeFoodCollsion(newSnake, b);
			distance++;
		}

		nn->outputs[0][0 + (i - s->direction)] = distance < maxDistance[i%4] ? distance : 0;
	}

	for (int i = s->direction; i < s->direction + 8; i++) {
		distance = 0;
		hasCollided = 0;

		//reset the temp snakes position
		for (int j = 0; j < newSnake->score; j++) {
			newSnake->x[j] = s->x[j];
			newSnake->y[j] = s->y[j];
		}

		while(!hasCollided && distance < maxDistance[i%4]) {
			switch (i%8) {
			case 0:
				newSnake->x[0]++;
				break;
			case 1:
				newSnake->y[0]++;
				break;
			case 2:
				newSnake->x[0]--;
				break;
			case 3:
				newSnake->y[0]--;
				break;
			case 4:
				newSnake->x[0]++;
				newSnake->y[0]++;
				break;
			case 5:
				newSnake->y[0]++;
				newSnake->x[0]--;
				break;
			case 6:
				newSnake->x[0]--;
				newSnake->y[0]++;
				break;
			case 7:
				newSnake->y[0]--;
				newSnake->x[0]++;
				break;
			}

			hasCollided = snakeCollison(newSnake, b);
			distance++;
		}

		nn->outputs[0][8 + (i - s->direction)] = 1/distance;
	}

	free(newSnake->x);
	free(newSnake->y);
	free(newSnake);
}

/**
 * @brief This function get the index of most active output neuron
 * @param nn - the neural network, assumed to have been propagated through
 * @return the index of the most active output.
 */
int getOutput(neuralNetwork* nn) {
		int max = nn->outputs[nn->networkSize-1][0];
		int maxIndex = 0;
		for (int i = 1; i < nn->networkLayout[nn->networkSize - 1]; i++) {
			if (nn->outputs[nn->networkSize-1][i] > max) {
				maxIndex = i;
				max = nn->outputs[nn->networkSize-1][i];
			}
		}
		return maxIndex;
}