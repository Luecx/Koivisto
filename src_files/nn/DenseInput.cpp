//
// Created by finne on 7/22/2020.
//

#include <cmath>
#include "DenseInput.h"
#include "DenseNetwork.h"
#include "../Util.h"


DenseInput::DenseInput(int inputSize, int outputSize) : inputSize(inputSize), outputSize(outputSize) {
    
    input = new float[inputSize];
    output = new float[outputSize];
    sums = new float[outputSize];
    weights = new float[outputSize*inputSize];
    bias = new float[outputSize];
    errorSignal = new float[outputSize];
    
    
    initWeights();
    
}

void DenseInput::initWeights() {
    
    float bound = 1.0f / sqrt(inputSize);
    
    for (int i = 0; i < outputSize; i++) {
        bias[i] = bb::randDouble(-bound, bound);
        for (int n = 0; n < inputSize; n++) {
            weights[i * inputSize + n] = bb::randDouble(-bound, bound);
        }
    }
    
}


float *DenseInput::getOutput() const {
    return output;
}



float *DenseInput::getWeights() const {
    return weights;
}

int DenseInput::getInputSize() const {
    return inputSize;
}

int DenseInput::getOutputSize() const {
    return outputSize;
}

float *DenseInput::getBias() const {
    return bias;
}

void DenseInput::adjustInput(int index, float newValue) {
    float valDif = newValue - input[index];
    input[index] = newValue;
    
    __m128 dif = _mm_set1_ps(valDif);
    
    
    for (int o = 0; o < outputSize; o+=4) {
        __m128 we = _mm_load_ps(&weights[index*outputSize + o]);
        __m128 outV = _mm_load_ps(&sums[o]);
        __m128 difV = _mm_mul_ps(we, dif);
        
        __m128 newV = _mm_add_ps(outV,difV);
        _mm_store_ps(&sums[o], newV);
    }
    
//    printArray(sums, outputSize);
}

float *DenseInput::getInput() const {
    return input;
}

float *DenseInput::getSums() const {
    return sums;
}

float *DenseInput::getErrorSignal() const {
    return errorSignal;
}

CompactTrainEntry *DenseInput::getLastInput() const {
    return lastInput;
}

void DenseInput::setLastInput(CompactTrainEntry *lastInput) {
    DenseInput::lastInput = lastInput;
}


