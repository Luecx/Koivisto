//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_DENSEINPUT_H
#define KOIVISTO_DENSEINPUT_H


#include <string>
#include "../Bitboard.h"
#include <immintrin.h>
#include <memory.h>

class DenseInput {

        
        int inputSize;
        int outputSize;
        

        float* input;
        float* sums;
        float* output;
        
        float* errorSignal;
        
        
        float* weights;
        float* bias;
    
    public:
        DenseInput(int inputSize, int outputSize);
        
        void initWeights();
        
        void adjustInput(int index, float newValue);
        
        template<bool recompute>
        void compute() {
            
            if(recompute){
                memset(sums, 0, outputSize*sizeof(float));
                for(int i = 0; i < inputSize; i++){
                    if(input[i] != 0){
                        for(int n = 0; n < outputSize; n++){
                            sums[n] += input[i] * weights[i*outputSize + n];
                        }
                    }
                }
            }
    
            for(int i = 0; i < outputSize; i++){
                output[i] = sums[i]+bias[i] < 0 ? 0:(sums[i]+bias[i]);
            }
         
        }
        
        void backprop(float eta){
            for(int i = 0; i <inputSize; i++){
                
                float sum = 0;
                for(int n = 0; n < outputSize; n++){
                    
                    //updating weights
                    weights[n + outputSize * i] -= errorSignal[n] * input[i] * eta;
                }
            }
            
            //updating bias
            for(int n = 0; n <  outputSize; n++){
                bias[n] -= errorSignal[n] * eta;
            }
            
        }
        
        float *getInput() const;
        
        float *getOutput() const;
        
        float *getWeights() const;
        
        int getInputSize() const;
        
        int getOutputSize() const;
        
        float *getSums() const;
        
        float *getBias() const;
        
        float *getErrorSignal() const;
};


#endif //KOIVISTO_DENSELAYER_H
