//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_DENSEOUTPUT_H
#define KOIVISTO_DENSEOUTPUT_H


#include <string>
#include <immintrin.h>
#include "nntools.h"
#include "data/Weight.h"

namespace nn{


class DenseOutput {

        
        int inputSize;
        
        float* input;
        float  output;
        
        Weight* bias;
        Weight* weights;
    
        
        
        
    public:
        DenseOutput(int inputSize);
        
        virtual ~DenseOutput();
        
        void initWeights();
        
        void compute(){
            __m128 acc = _mm_setzero_ps();
            for (int col = 0; col < inputSize; col += 4) {
                __m128 vec = _mm_load_ps(&input[col]);
                __m128 mat = _mm_load_ps(&weights->value[col]);
                acc = _mm_add_ps(acc, _mm_mul_ps(mat, vec));
            }
    
    
            acc = _mm_hadd_ps(acc, acc);
            acc = _mm_hadd_ps(acc, acc);
    
            output = acc[0] + bias->value[0];
//            output = sigmoid(output);
        }
        
        void setInput(float *input);
        
        float *getInput() const;
        
        float getOutput() const;
        
        int getInputSize() const;
        
        int getOutputSize() const;
        
        Weight *getWeights() const;
        
        Weight* getBias() const;
        
        void setBias(Weight *bias);
        
        void setWeights(Weight *weights);
    
};

}

#endif //KOIVISTO_DENSEOUTPUT_H
