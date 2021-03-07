#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "neuralNetworkShell.h"

/**
 * @brief This function sets the structure of the neural network struct passed in.
 * @param nn - the neural network whos structure will be set
 * @param networkSize - the number of layers to be input, including input and output layers, 
 * 						must be at least 2.
 * @param ... - the size of each individual layer as integers, including the input and output layers.
 * @return nothing.
 */
void setNetworkLayout(neuralNetwork* nn, int networkSize, ...) {
	nn->networkSize = networkSize;
	nn->networkLayout = calloc(networkSize, sizeof(int));

	va_list valist;

	va_start(valist, networkSize);
	for (int i = 0; i < networkSize; i++) 
		nn->networkLayout[i] = va_arg(valist, int);

	va_end(valist);
}

/**
 * @brief This function creates, empty, a new network "brain"
 * @param nn - this stores the neural networks data points
 * @return nothing
 */
void initialiseNetworkBrain(neuralNetwork* nn) {
	setNetworkLayout(nn, 3, 16, 9, 3);

	nn->weights = calloc(nn->networkSize - 1, sizeof(double**));
    nn->biases = calloc(nn->networkSize - 1, sizeof(double*));

    for (int i = 0; i < nn->networkSize - 1; i++) {
        nn->weights[i] = calloc(nn->networkLayout[i], sizeof(double*));
        nn->biases[i] = calloc(nn->networkLayout[i+1], sizeof(double));

        for (int j = 0; j < nn->networkLayout[i]; j++) 
            nn->weights[i][j] = calloc(nn->networkLayout[i+1], sizeof(double));
    }

	nn->outputs = calloc(nn->networkSize, sizeof(double*));
	for (int i = 0; i < nn->networkSize; i++) 
		nn->outputs[i] = calloc(nn->networkLayout[i], sizeof(double));
}

/**
 * @brief This function frees the memory of the weights, biases and outputs
 * @param nn - this stores the networks data points
 * @return nothing
 */
void destroyBrainData(neuralNetwork* nn) {
    for (int i = 0; i < nn->networkSize - 1; i++) {
        for (int j = 0; j < nn->networkLayout[i]; j++) 
            free(nn->weights[i][j]);
        free(nn->weights[i]);
        free(nn->biases[i]);
    }
    free(nn->weights);
    free(nn->biases);

	for (int i = 0; i < nn->networkSize; i++) 
        free(nn->outputs[i]);
    free(nn->outputs);

	free(nn->networkLayout);
}

/**
 * @brief This function randomises all of the values in the neural network
 * @param nn - the network to be randomised
 * @return nothing
 */
void randomiseNetwork(neuralNetwork* nn) {
    for (int i = 0; i < nn->networkSize - 1; i++) {
        for (int j = 0; j < nn->networkLayout[i]; j++) 
            for (int k = 0; k < nn->networkLayout[i+1]; k++) 
                nn->weights[i][j][k] = (((double)rand() / RAND_MAX) * 4) - 2;

        for (int j = 0; j < nn->networkLayout[i+1]; j++) 
            nn->biases[i][j] = (((double)rand() / RAND_MAX) * 4) - 2;
    }
}

/**
 * @brief This function copies each data point from source to the neural net at destination
 * @param source - the network to be copied
 * @param destination - the network to be copied to
 * @return nothing
 */
void deepCopy(neuralNetwork* source, neuralNetwork* destination) {
    for (int i = 0; i < source->networkSize - 1; i++) {
        for (int j = 0; j < source->networkLayout[i]; j++) 
            for (int k = 0; k < source->networkLayout[i+1]; k++) 
                destination->weights[i][j][k] = source->weights[i][j][k];

        for (int j = 0; j < source->networkLayout[i+1]; j++) 
			destination->biases[i][j] = source->biases[i][j];
    }
	for (int i = 0; i < source->networkSize; i++) 
		for (int j = 0; j < source->networkLayout[i]; j++)
			destination->outputs[i][j] = source->outputs[i][j];
}

/**
 * @brief This function copys an input array to the neural network to act as the inputs for the network
 * @param nn - this stores all of the networks data points
 * @param inputs - the inputs that should be passes into the neural network
 * @return nothing
 */
void assignInputs(neuralNetwork* nn, double* inputs) {
	for (int i = 0; i < nn->networkLayout[i]; i++) 
        nn->outputs[0][i] = inputs[i];
}

/**
 * @brief This function passes an input (the 0th element of the output arr) though the network,
 *        saving the outputs of all neurons
 * @param nn - this stores all of the networks data points
 * @param normilise - if 0, the outputs will not be normilsed, otherwise they will be
 * @return nothing
 */
void frontPropegation(neuralNetwork* nn, int normilise) {
	for (int i = 0; i < nn->networkSize - 1; i++) {
		for (int j = 0; j < nn->networkLayout[i+1]; j++) {
			nn->outputs[i+1][j] = 0.0;
			for (int k = 0; k < nn->networkLayout[i]; k++) 
				nn->outputs[i+1][j] += nn->outputs[i][k] * nn->weights[i][k][j];

			nn->outputs[i+1][j] += nn->biases[i][j];
			nn->outputs[i+1][j] = crelu(nn->outputs[i+1][j]);
		}

		//normilse the output data
        if (normilise) {
            double max = 1;
            for (int j = 0; j < nn->networkLayout[i+1]; j++) 
                if (nn->outputs[i+1][j] > max) 
                    max = nn->outputs[i+1][j];

            for (int j = 0; j < nn->networkLayout[i+1]; j++) 
                nn->outputs[i+1][j] /= max;
        }
	}
}

/**
 * @brief This function writes the network to a file for later use
 * @param nn - this stores the data points of the neural network
 * @param filePath - the file location you wish to save the file from
 * @return nothing
 */
void saveBrain(neuralNetwork* nn, const char* filePath) {
	FILE* f = fopen(filePath, "w");

	//writes the network structure to a file
	for (int i = 0; i < nn->networkSize - 1; i++) 
		fprintf(f, "%d,", nn->networkLayout[i]);
	fprintf(f, "%d\n", nn->networkLayout[nn->networkSize-1]);

	//writes the weight values to the file
	for (int i = 0; i < nn->networkSize - 1; i++) 
		for (int j = 0; j < nn->networkLayout[i]; j++) 
			for (int k = 0; k < nn->networkLayout[i+1]; k++) 
				fprintf(f, "%.50lf\n", nn->weights[i][j][k]);

	//writes the biases to the file
	for (int i = 0; i < nn->networkSize - 1; i++) 
		for (int j = 0; j < nn->networkLayout[i+1]; j++) 
			fprintf(f, "%.50lf\n", nn->biases[i][j]);

	fclose(f);
}

/**
 * @brief This function reads the network from a file
 * @param nn - this stores the data points of the neural network
 * @param filePath - the file location you wish to read the file from
 * @return nothing
 */
void loadBrain(neuralNetwork* nn, const char* filePath) {
	FILE* f = fopen(filePath, "r");
	printf("Hello world!");

	char tempStr[4096];
	const char delim[2] = ",";
	char* token;

	fscanf(f, "%s", tempStr);

	int i = 0;
	for (token = strtok(tempStr, delim); token && *token; token = strtok(NULL, delim)) {
		if (atoi(token) != nn->networkLayout[i]) {
			printf("Error loading network, dimensions do not match\n");
			exit(1);
		}
		i++;
	}


	char* e;
	//writes the weight values to the file
	for (int i = 0; i < nn->networkSize - 1; i++) {
		for (int j = 0; j < nn->networkLayout[i]; j++) {
			for (int k = 0; k < nn->networkLayout[i+1]; k++) {
				fscanf(f, "%s", tempStr);
				nn->weights[i][j][k] = strtod(tempStr, &e);
			}
		}
	}

	//writes the biases to the file
	for (int i = 0; i < nn->networkSize - 1; i++) {
		for (int j = 0; j < nn->networkLayout[i+1]; j++) {
			fscanf(f, "%s", tempStr);
			nn->biases[i][j] = strtod(tempStr, &e);
		}
	}

	fclose(f);
}