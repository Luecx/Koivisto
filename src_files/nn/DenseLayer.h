//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_DENSELAYER_H
#define KOIVISTO_DENSELAYER_H


#include <string>
#include "../Bitboard.h"
#include <immintrin.h>

class DenseLayer {

        
        int inputSize;
        int outputSize;
        
        float* input;
        float* output;
        
        float* errorSignal;
        
        float* weights;
        float* bias;
        
    
    public:
        DenseLayer(int inputSize, int outputSize);
        
        void initWeights();
        
        void compute(){
            
            
            for (int row = 0; row < outputSize; row += 4) {
                
                __m128 biasV = _mm_load_ps(&bias[row]);
    
                __m128 acc0 = _mm_setzero_ps();
                __m128 acc1 = _mm_setzero_ps();
                __m128 acc2 = _mm_setzero_ps();
                __m128 acc3 = _mm_setzero_ps();
                for (int col = 0; col < inputSize; col += 4) {
                    __m128 vec = _mm_load_ps(&input[col]);
                    __m128 mat0 = _mm_load_ps(&weights[col + inputSize * row]);
                    __m128 mat1 = _mm_load_ps(&weights[col + inputSize * (row + 1)]);
                    __m128 mat2 = _mm_load_ps(&weights[col + inputSize * (row + 2)]);
                    __m128 mat3 = _mm_load_ps(&weights[col + inputSize * (row + 3)]);
                    acc0 = _mm_add_ps(acc0, _mm_mul_ps(mat0, vec));
                    acc1 = _mm_add_ps(acc1, _mm_mul_ps(mat1, vec));
                    acc2 = _mm_add_ps(acc2, _mm_mul_ps(mat2, vec));
                    acc3 = _mm_add_ps(acc3, _mm_mul_ps(mat3, vec));
                }
                acc0 = _mm_hadd_ps(acc0, acc1);
                acc2 = _mm_hadd_ps(acc2, acc3);
                acc0 = _mm_hadd_ps(acc0, acc2);
                acc0 = _mm_add_ps(acc0, biasV);
                _mm_store_ps(&output[row], acc0);
                output[row]   = output[row]   < 0 ? output[row]  *0.1f:output[row];
                output[row+1] = output[row+1] < 0 ? output[row+1]*0.1f:output[row+1];
                output[row+2] = output[row+2] < 0 ? output[row+2]*0.1f:output[row+2];
                output[row+3] = output[row+3] < 0 ? output[row+3]*0.1f:output[row+3];
            }
            
        }
        
        void backprop(float* prevError, float eta){
            for(int i = 0; i <inputSize; i++){
                
                float sum = 0;
                for(int n = 0; n < outputSize; n++){
                    //summing up the error
                    sum += errorSignal[n] * weights[i + inputSize * n];
                    
                    //updating weights
                    weights[i + inputSize * n] -= errorSignal[n] * input[i] * eta;;
                }
                prevError[i] = sum * (input[i] <= 0 ? 0.1:1);
            }
            
            //updating bias
            for(int n = 0; n <  outputSize; n++){
                bias[n] -= errorSignal[n] * eta;
            }
            
        }
        
        void setInput(float *input);
        
        float *getInput() const;
        
        float *getOutput() const;
        
        int getInputSize() const;
        
        int getOutputSize() const;
        
        float *getWeights() const;
        
        float *getBias() const;
        
        float *getErrorSignal() const;
};


#endif //KOIVISTO_DENSELAYER_H
