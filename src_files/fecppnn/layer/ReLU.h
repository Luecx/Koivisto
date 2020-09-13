

#ifndef KOIVISTO_RELU_H
#define KOIVISTO_RELU_H

#include "Layer.h"

#include <immintrin.h>

/**
 *                   .   /
 *                   .  /
 *                   . /
 *                   ./
 *       -------------....................
 *                   .
 *                   .
 *
 */
class ReLU : public Layer {
    public:
    ReLU(Layer* prevLayer) : Layer(prevLayer->getOutput()->getSize()) { this->connect(prevLayer); };

    void compute() override {
    
        static __m256 lower = _mm256_set1_ps(0);
        
        float* outputVals = getOutput()->getValues();
        float* inputVals  = getInput()->getValues();
    
        for(int i = 0; i < getOutput()->getSize(); i+=8){
        
            __m256 in  = _mm256_load_ps(&(inputVals[i]));
    
            __m256 out = _mm256_max_ps(in, lower);
        
            _mm256_store_ps(&(outputVals[i]), out);
        }
    }
    void backprop() override {

        float* outputGrads = getOutput()->getGradient()->getValues();
        float* outputVals  = getOutput()->getValues();
        float* inputGrads  = getInput()->getGradient()->getValues();

        for (int i = 0; i < getOutput()->getSize(); i++) {
            inputGrads[i] = outputVals[i] < 0 ? 0 : outputGrads[i];
        }
    }
};

#endif    // KOIVISTO_RELU_H
