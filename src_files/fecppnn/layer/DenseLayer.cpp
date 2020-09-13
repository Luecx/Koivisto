


#include "DenseLayer.h"


DenseLayer::DenseLayer(Layer* previousLayer, int size) : Layer(size) {
    weights = new Data(previousLayer->getOutput()->getSize(), size, true);
    bias    = new Data(size, true);
    
    float wBound = 1 / sqrt(previousLayer->getOutput()->getSize());
    
    weights->randomise(-wBound, +wBound);
    bias->randomise(-wBound, +wBound);
    
    this->connect(previousLayer);
}

void DenseLayer::compute() {
    
    Data* output = getOutput();
    Data* input  = getInput();
    
    int inputSize = input->getSize();
    
    float* outputValues = output->getValues();
    float* inputValues  = getInput()->getValues();
    float* bias         = this->bias->getValues();
    float* weights      = this->weights->getValues();
    
    for (int row = 0; row < output->getSize(); row += 8) {
        
        __m256 biasV = _mm256_load_ps(&(bias[row]));
        
        __m256 acc0 = _mm256_setzero_ps();
        __m256 acc1 = _mm256_setzero_ps();
        __m256 acc2 = _mm256_setzero_ps();
        __m256 acc3 = _mm256_setzero_ps();
        __m256 acc4 = _mm256_setzero_ps();
        __m256 acc5 = _mm256_setzero_ps();
        __m256 acc6 = _mm256_setzero_ps();
        __m256 acc7 = _mm256_setzero_ps();
        for (int col = 0; col < inputSize; col += 8) {
            __m256 vec = _mm256_load_ps(&inputValues[col]);
            
            __m256 mat0 = _mm256_load_ps(&weights[col + inputSize * row]);
            __m256 mat1 = _mm256_load_ps(&weights[col + inputSize * (row + 1)]);
            __m256 mat2 = _mm256_load_ps(&weights[col + inputSize * (row + 2)]);
            __m256 mat3 = _mm256_load_ps(&weights[col + inputSize * (row + 3)]);
            __m256 mat4 = _mm256_load_ps(&weights[col + inputSize * (row + 4)]);
            __m256 mat5 = _mm256_load_ps(&weights[col + inputSize * (row + 5)]);
            __m256 mat6 = _mm256_load_ps(&weights[col + inputSize * (row + 6)]);
            __m256 mat7 = _mm256_load_ps(&weights[col + inputSize * (row + 7)]);
            
            acc0 = _mm256_add_ps(acc0, _mm256_mul_ps(mat0, vec));
            acc1 = _mm256_add_ps(acc1, _mm256_mul_ps(mat1, vec));
            acc2 = _mm256_add_ps(acc2, _mm256_mul_ps(mat2, vec));
            acc3 = _mm256_add_ps(acc3, _mm256_mul_ps(mat3, vec));
            acc4 = _mm256_add_ps(acc4, _mm256_mul_ps(mat4, vec));
            acc5 = _mm256_add_ps(acc5, _mm256_mul_ps(mat5, vec));
            acc6 = _mm256_add_ps(acc6, _mm256_mul_ps(mat6, vec));
            acc7 = _mm256_add_ps(acc7, _mm256_mul_ps(mat7, vec));
        }
        
        acc0 = _mm256_hadd_ps(acc0, acc1);
        acc2 = _mm256_hadd_ps(acc2, acc3);
        
        acc4 = _mm256_hadd_ps(acc4, acc5);
        acc6 = _mm256_hadd_ps(acc6, acc7);
        
        acc0 = _mm256_hadd_ps(acc0, acc2);
        acc4 = _mm256_hadd_ps(acc4, acc6);
        
        __m128 sumabcd1 = _mm256_extractf128_ps(acc0, 0);
        __m128 sumabcd2 = _mm256_extractf128_ps(acc0, 1);
        __m128 sumefgh1 = _mm256_extractf128_ps(acc4, 0);
        __m128 sumefgh2 = _mm256_extractf128_ps(acc4, 1);
        
        sumabcd1 = _mm_add_ps(sumabcd1, sumabcd2);
        sumefgh1 = _mm_add_ps(sumefgh1, sumefgh2);
        
        acc0 = _mm256_insertf128_ps(_mm256_castps128_ps256(sumabcd1), sumefgh1, 1);
        
        acc0 = _mm256_add_ps(biasV, acc0);
        
        _mm256_store_ps(&outputValues[row], acc0);
    }
}
void DenseLayer::backprop() {
    Data* output = getOutput();
    Data* input  = getInput();
    
    int inputSize = input->getSize();
    
    if(input->hasGradient()){
        float* outputGrads  = output->getGradient()->getValues();
        float* inputGrads   = getInput()->getGradient()->getValues();
        float* inputValues  = getInput()->getValues();
        float* weights      = this->weights->getValues();
        float* weightsGrads = this->weights->getGradient()->getValues();
        float* biasGrads    = this->bias->getGradient()->getValues();
        
        for (int o = 0; o < output->getSize(); o++) {
            
            __m256 outputGrad = _mm256_set1_ps(outputGrads[o]);
            biasGrads[o] += outputGrads[o];
            
            for (int i = 0; i < inputSize; i += 8) {
                __m256 mat = _mm256_load_ps(&weights[i + inputSize * o]);
                _mm256_store_ps(&inputGrads[i], _mm256_mul_ps(mat, outputGrad));
                
                __m256 weightGrads = _mm256_load_ps(&weightsGrads[i + inputSize * o]);
                __m256 inputs      = _mm256_load_ps(&inputValues[i]);
                
                weightGrads = _mm256_add_ps(weightGrads, _mm256_mul_ps(outputGrad, inputs));
                _mm256_store_ps(&weightsGrads[i + inputSize * o], weightGrads);
            }
        }
    }else{
        float* outputGrads  = output->getGradient()->getValues();
        float* inputValues  = getInput()->getValues();
        float* weightsGrads = this->weights->getGradient()->getValues();
        float* biasGrads    = this->bias->getGradient()->getValues();
        
        for (int o = 0; o < output->getSize(); o++) {
            
            __m256 outputGrad = _mm256_set1_ps(outputGrads[o]);
            biasGrads[o] += outputGrads[o];
            
            for (int i = 0; i < inputSize; i += 8) {
                __m256 weightGrads = _mm256_load_ps(&weightsGrads[i + inputSize * o]);
                __m256 inputs      = _mm256_load_ps(&inputValues[i]);
                
                weightGrads = _mm256_add_ps(weightGrads, _mm256_mul_ps(outputGrad, inputs));
                _mm256_store_ps(&weightsGrads[i + inputSize * o], weightGrads);
            }
        }
    }
    
    
}
Data* DenseLayer::getWeights() const { return weights; }
void  DenseLayer::setWeights(Data* weights) { DenseLayer::weights = weights; }
Data* DenseLayer::getBias() const { return bias; }
void  DenseLayer::setBias(Data* bias) { DenseLayer::bias = bias; }

