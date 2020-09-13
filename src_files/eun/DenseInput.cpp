//
// Created by finne on 7/22/2020.
//

#include "DenseInput.h"

#include "../Util.h"
#include "DenseNetwork.h"

#include <cmath>

nn::DenseInput::DenseInput(int inputSize, int outputSize) : inputSize(inputSize), outputSize(outputSize) {

    input  = new (std::align_val_t(32)) float[inputSize] {};
    output = new (std::align_val_t(32)) float[outputSize] {};
    sums   = new (std::align_val_t(32)) float[outputSize] {};

    weights = new Weight(outputSize * inputSize);
    bias    = new Weight(outputSize);

    initWeights();
}

void nn::DenseInput::initWeights() {

    float bound = 1.0f / sqrt(inputSize);

    for (int i = 0; i < outputSize; i++) {
        bias->value[i] = bb::randDouble(-bound, bound);
        for (int n = 0; n < inputSize; n++) {
            weights->value[i * inputSize + n] = bb::randDouble(-bound, bound);
        }
    }
}

float* nn::DenseInput::getOutput() const { return output; }

Weight* nn::DenseInput::getWeights() const { return weights; }

int nn::DenseInput::getInputSize() const { return inputSize; }

int nn::DenseInput::getOutputSize() const { return outputSize; }

Weight* nn::DenseInput::getBias() const { return bias; }

void nn::DenseInput::adjustInput(int index, float newValue) {

    if (newValue == 0) {
        inputTracker.remove(index);
    } else {
        inputTracker.put(index);
    }

    // only do the upper part for the upper neurons
    if (index < inputSize / 2) {

        float valDif = newValue - input[index];
        input[index] = newValue;

        for (int o = 0; o < outputSize / 2; o++) {
            sums[o] += weights->value[index * outputSize + o] * valDif;
        }

        //        __m256 dif = _mm256_set1_ps(valDif);
        //
        //
        //        for (int o = 0; o < outputSize/2; o+=8) {
        //            __m256 we = _mm256_load_ps(&weights->value[index*outputSize + o]);
        //            __m256 outV = _mm256_load_ps(&sums[o]);
        //            __m256 difV = _mm256_mul_ps(we, dif);
        //
        //            __m256 newV = _mm256_add_ps(outV,difV);
        //            _mm256_store_ps(&sums[o], newV);
        //
        //        }
    } else {
        float valDif = newValue - input[index];
        input[index] = newValue;

        for (int o = outputSize / 2; o < outputSize; o++) {
            sums[o] += weights->value[index * outputSize + o] * valDif;
        }

        //        __m256 dif = _mm256_set1_ps(valDif);
        //
        //
        //        for (int o = outputSize/2; o < outputSize; o+=8) {
        //            __m256 we = _mm256_load_ps(&weights->value[index*outputSize + o]);
        //            __m256 outV = _mm256_load_ps(&sums[o]);
        //            __m256 difV = _mm256_mul_ps(we, dif);
        //
        //            __m256 newV = _mm256_add_ps(outV,difV);
        //            _mm256_store_ps(&sums[o], newV);
        //        }
    }
}

float* nn::DenseInput::getInput() const { return input; }

float* nn::DenseInput::getSums() const { return sums; }

void nn::DenseInput::clearInput() {
    memset(sums, 0, sizeof(float) * outputSize);

    for (int k = 0; k < inputTracker.count(); k++) {
        int i    = inputTracker.at(k);
        input[i] = 0;
    }
    inputTracker.clear();
}

int nn::DenseInput::getNonZeroCount() const { return inputTracker.count(); }

const nn::InputTracker& nn::DenseInput::getNonZeroIndices() const { return inputTracker; }

nn::DenseInput::~DenseInput() {
    delete weights;
    delete bias;
}

void nn::DenseInput::setWeights(Weight* weights) { DenseInput::weights = weights; }

void nn::DenseInput::setBias(Weight* bias) { DenseInput::bias = bias; }
