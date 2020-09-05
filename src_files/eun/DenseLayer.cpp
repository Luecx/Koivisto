//
// Created by finne on 7/22/2020.
//

#include <cmath>
#include "DenseLayer.h"
#include "data/Weight.h"


nn::DenseLayer::DenseLayer(int inputSize, int outputSize) : inputSize(inputSize), outputSize(outputSize) {
    
    
    input = new (std::align_val_t(32)) float[inputSize];
    output = new (std::align_val_t(32)) float[outputSize];
    
    weights = new Weight(outputSize*inputSize);
    bias  = new Weight(outputSize);
    
    initWeights();

}

void nn::DenseLayer::initWeights() {
        
        float bound = 1.0f / sqrt(inputSize);
        
        
        for(int i = 0; i < outputSize; i++){
            bias->value[i] = bb::randDouble(-bound,bound);
            for(int n = 0; n < inputSize; n++){
                weights->value[i * inputSize + n] = bb::randDouble(-bound, bound);
            }
        }
    
}

float *nn::DenseLayer::getInput() const {
    return input;
}

float *nn::DenseLayer::getOutput() const {
    return output;
}

int nn::DenseLayer::getInputSize() const {
    return inputSize;
}

int nn::DenseLayer::getOutputSize() const {
    return outputSize;
}

Weight *nn::DenseLayer::getWeights() const {
    return weights;
}

Weight *nn::DenseLayer::getBias() const {
    return bias;
}

void nn::DenseLayer::setInput(float *input) {
    //delete DenseLayer::input;
    
    
    DenseLayer::input = input;
}


void nn::DenseLayer::setWeights(Weight *weights) {
    DenseLayer::weights = weights;
}

void nn::DenseLayer::setBias(Weight *bias) {
    DenseLayer::bias = bias;
}

nn::DenseLayer::~DenseLayer() {
    delete weights;
    delete bias;
}



