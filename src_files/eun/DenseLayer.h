//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_DENSELAYER_H
#define KOIVISTO_DENSELAYER_H


#include <string>
#include "../Bitboard.h"
#include "nntools.h"
#include "data/Weight.h"
#include <immintrin.h>

namespace nn{


class DenseLayer {
        
        
        int inputSize;
        int outputSize;
        
        float *input;
        float *output;
        
        Weight *weights;
        Weight *bias;
    
    
    public:
        DenseLayer(int inputSize, int outputSize);
        
        virtual ~DenseLayer();
        
        void initWeights();
        
        void compute() {
    

//            for (int row = 0; row < outputSize; row += 4) {
//
//                __m128 biasV = _mm_load_ps(&(bias->value[row]));
//
//                __m128 acc0 = _mm_setzero_ps();
//                __m128 acc1 = _mm_setzero_ps();
//                __m128 acc2 = _mm_setzero_ps();
//                __m128 acc3 = _mm_setzero_ps();
//                for (int col = 0; col < inputSize; col += 4) {
//                    __m128 vec = _mm_load_ps(&input[col]);
//                    __m128 mat0 = _mm_load_ps(&weights->value[col + inputSize * row]);
//                    __m128 mat1 = _mm_load_ps(&weights->value[col + inputSize * (row + 1)]);
//                    __m128 mat2 = _mm_load_ps(&weights->value[col + inputSize * (row + 2)]);
//                    __m128 mat3 = _mm_load_ps(&weights->value[col + inputSize * (row + 3)]);
//                    acc0 = _mm_add_ps(acc0, _mm_mul_ps(mat0, vec));
//                    acc1 = _mm_add_ps(acc1, _mm_mul_ps(mat1, vec));
//                    acc2 = _mm_add_ps(acc2, _mm_mul_ps(mat2, vec));
//                    acc3 = _mm_add_ps(acc3, _mm_mul_ps(mat3, vec));
//                }
//
//
//                acc0 = _mm_hadd_ps(acc0, acc1);
//                acc2 = _mm_hadd_ps(acc2, acc3);
//                acc0 = _mm_hadd_ps(acc0, acc2);
//
//                acc0 = _mm_add_ps(acc0, biasV);
//                _mm_store_ps(&output[row], acc0);
//
//                output[row] = nn::activation(output[row]);
//                output[row + 1] = nn::activation(output[row + 1]);
//                output[row + 2] = nn::activation(output[row + 2]);
//                output[row + 3] = nn::activation(output[row + 3]);
//
//            }
    
            for (int row = 0; row < outputSize; row += 8) {



                __m256 biasV = _mm256_load_ps(&(bias->value[row]));

                __m256 acc0 = _mm256_setzero_ps();
                __m256 acc1 = _mm256_setzero_ps();
                __m256 acc2 = _mm256_setzero_ps();
                __m256 acc3 = _mm256_setzero_ps();
                __m256 acc4 = _mm256_setzero_ps();
                __m256 acc5 = _mm256_setzero_ps();
                __m256 acc6 = _mm256_setzero_ps();
                __m256 acc7 = _mm256_setzero_ps();
                for (int col = 0; col < inputSize; col += 8) {
                    __m256 vec = _mm256_load_ps(&input[col]);


                    __m256 mat0 = _mm256_load_ps(&weights->value[col + inputSize * row]);
                    __m256 mat1 = _mm256_load_ps(&weights->value[col + inputSize * (row + 1)]);
                    __m256 mat2 = _mm256_load_ps(&weights->value[col + inputSize * (row + 2)]);
                    __m256 mat3 = _mm256_load_ps(&weights->value[col + inputSize * (row + 3)]);
                    __m256 mat4 = _mm256_load_ps(&weights->value[col + inputSize * (row + 4)]);
                    __m256 mat5 = _mm256_load_ps(&weights->value[col + inputSize * (row + 5)]);
                    __m256 mat6 = _mm256_load_ps(&weights->value[col + inputSize * (row + 6)]);
                    __m256 mat7 = _mm256_load_ps(&weights->value[col + inputSize * (row + 7)]);
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


                acc0 =_mm256_insertf128_ps(_mm256_castps128_ps256(sumabcd1), sumefgh1, 1);


                acc0 = _mm256_add_ps(biasV, acc0);
                _mm256_store_ps(&output[row], acc0);



                output[row] = nn::activation(output[row]);
                output[row + 1] = nn::activation(output[row + 1]);
                output[row + 2] = nn::activation(output[row + 2]);
                output[row + 3] = nn::activation(output[row + 3]);
                output[row + 4] = nn::activation(output[row + 4]);
                output[row + 5] = nn::activation(output[row + 5]);
                output[row + 6] = nn::activation(output[row + 6]);
                output[row + 7] = nn::activation(output[row + 7]);

            }
        
        }
        
        void setInput(float *input);
        
        float *getInput() const;
        
        float *getOutput() const;
        
        int getInputSize() const;
        
        int getOutputSize() const;
        
        Weight *getWeights() const;
        
        Weight *getBias() const;
        
        void setWeights(Weight *weights);
        
        void setBias(Weight *bias);
        
};

}
#endif //KOIVISTO_DENSELAYER_H
