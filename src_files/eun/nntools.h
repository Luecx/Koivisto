//
// Created by finne on 7/23/2020.
//

#ifndef KOIVISTO_NNTOOLS_H
#define KOIVISTO_NNTOOLS_H


#define whiteInputIndex(activePlayer, piece, square, opponentKing)    activePlayer  * (6 * 64 * 64) +              opponentKing  * (6*64) + piece   * 64 +              square
#define blackInputIndex(activePlayer, piece, square, opponentKing) (1-activePlayer) * (6 * 64 * 64) + mirrorSquare(opponentKing) * (6*64) + piece%6 * 64 + mirrorSquare(square)

#include <immintrin.h>
#include <cmath>

namespace nn{

inline float activation(float x){
    if (x < 0) return x*0.1;
//    if (x > 1) return 1;
    return x;
}

inline float derivativeFromOutput(float x){
//    if (x <= 0 || x >= 1) return 0;
    if(x <= 0) return 0.05;
    return 1;
}


inline float inverseSigmoid(float y){
    return -log(1/y-1);
}

inline float sigmoid(float x){
    
    
    return 1.0f/(1+pow(10,-x/4));
    
    return x;
}

inline float sigmoidPrime(float x) {
    
    return sigmoid(x) * (1 - sigmoid(x));
    
}

inline float randFloat(float min, float max) {
    float f = (float)rand() / RAND_MAX;
    return min + f * (max - min);
}


static inline float _mm256_reduce_add_ps(__m256 x) {
    /* ( x3+x7, x2+x6, x1+x5, x0+x4 ) */
    const __m128 x128 = _mm_add_ps(_mm256_extractf128_ps(x, 1), _mm256_castps256_ps128(x));
    /* ( -, -, x1+x3+x5+x7, x0+x2+x4+x6 ) */
    const __m128 x64 = _mm_add_ps(x128, _mm_movehl_ps(x128, x128));
    /* ( -, -, -, x0+x1+x2+x3+x4+x5+x6+x7 ) */
    const __m128 x32 = _mm_add_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
    /* Conversion to float is a no-op on x86-64 */
    return _mm_cvtss_f32(x32);
}
}

#endif //KOIVISTO_NNTOOLS_H
