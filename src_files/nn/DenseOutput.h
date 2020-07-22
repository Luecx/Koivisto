//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_DENSEOUTPUT_H
#define KOIVISTO_DENSEOUTPUT_H


#include <string>
#include "../Bitboard.h"
#include <immintrin.h>

class DenseOutput {

        
        int inputSize;
        
        float* input;
        
        float  errorSignal;
        float  output;
        float  bias;
        
        float* weights;
    
        
        
        
    public:
        DenseOutput(int inputSize);
        
        void initWeights();
        
        void compute(){
    
            __m128 acc = _mm_setzero_ps();
            for (int col = 0; col < inputSize; col += 4) {
                __m128 vec = _mm_load_ps(&input[col]);
                __m128 mat = _mm_load_ps(&weights[col]);
                acc = _mm_add_ps(acc, _mm_mul_ps(mat, vec));
            }
            acc = _mm_hadd_ps(acc, acc);
            acc = _mm_hadd_ps(acc, acc);
            
            output = acc[0] + bias;
        }
        
        float backprop(float* prevError, float eta, float target){
    
    
            //the derivative of the output is 1 because no activation function is applied
            errorSignal = (output-target) * 1;
            
            for(int i = 0; i <inputSize; i++){
                
                //dont parse it back if that specific neuron has no output
                if(input[i] <= 0){
                    prevError[i] = 0;
                    continue;
                }
                
                
                prevError[i] = errorSignal * weights[i];
                
                //updating weights
                weights[i] -= eta * input[i] * errorSignal;
            }
            
            //updating bias
            bias -= eta * errorSignal;
            return errorSignal;
        }
        
        
        
        
        void setInput(float *input);
        
        float *getInput() const;
        
        float getOutput() const;
        
        int getInputSize() const;
        
        int getOutputSize() const;
        
        float *getWeights() const;
        
        float getErrorSignal() const;
        
        float getBias() const;
        
        void setBias(float bias);
};


#endif //KOIVISTO_DENSEOUTPUT_H
