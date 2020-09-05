//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_DENSEINPUT_H
#define KOIVISTO_DENSEINPUT_H


#include <string>
#include "../Bitboard.h"
#include "data/CompactInput.h"
#include <immintrin.h>
#include <memory.h>
#include "nntools.h"
#include "data/Weight.h"

namespace nn{

/**
 * tracks the indices in the input which are set
 */
struct InputTracker{
    private:
        int indices[64];
        
        int size = 0;
    public:
        void put(int index) {
            indices[size++] = index;
        }
        
        void remove(int index){
            for(int i = 0; i < size; i++){
                if (indices[i] == index){
                    indices[i] = indices[(size--)-1];
                    return;
                }
            }
        }
        
        bool contains(int index){
            for(int i = 0; i < size; i++){
                if (indices[i] == index){
                    return true;
                }
            }
            return false;
        }
        
        void clear() {
            size = 0;
        }
        
        int count() const{
            return size;
        }

        int at(int index){
            return indices[index];
        }
};

class DenseInput {
        
        InputTracker inputTracker{};
        
        int inputSize;
        int outputSize;
        
        float* input;
        
        float* sums;
        float* output;
        
        Weight* weights;
        Weight* bias;
    
    public:
        DenseInput(int inputSize, int outputSize);
        
        virtual ~DenseInput();
        
        void initWeights();
        
        /**
         * adjusting a single input will change all the "sums".
         * activations wont be applied in here
         * @param index
         * @param newValue
         */
        void adjustInput(int index, float newValue);
        
        /**
         * the idea of recomputing is that updating the input incrementally a lot of times could lead to
         * rounding issues. recompute will force the input to be summed up over the weights. otherwise only the activation
         * function will be applied
         * @tparam recompute
         */
        template<bool recompute>
        void compute() {
            
            if(recompute){
                memset(sums, 0, outputSize*sizeof(float));
                for(int k = 0; k < inputTracker.count(); k++){
                    int i = inputTracker.at(k);
                    
                    
                    
                    if(i < inputSize/2){
                        for (int n = 0; n < outputSize/2; n++) {
                            sums[n] += input[i] * weights->value[i * outputSize + n];
                        }
                    }else{
                        for (int n = outputSize/2; n < outputSize; n++) {
                            sums[n] += input[i] * weights->value[i * outputSize + n];
                        }
                    }
                    
                    
                }
            }

            for(int i = 0; i < outputSize; i++){
                
                output[i] = activation(sums[i]+bias->value[i]);
                
            }
         
        }
        
        float *getInput() const;
        
        float *getOutput() const;
        
        Weight *getWeights() const;
        
        int getInputSize() const;
        
        int getOutputSize() const;
        
        float *getSums() const;
        
        Weight *getBias() const;
        
        void setWeights(Weight *weights);
        
        void setBias(Weight *bias);
        
        void clearInput();
        
        const InputTracker &getNonZeroIndices() const;
        
        int getNonZeroCount() const;
        
};


}

#endif //KOIVISTO_DENSELAYER_H
