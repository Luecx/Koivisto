//
// Created by Luecx on 13.09.2022.
//

#ifndef KOIVISTO_DENSENETWORK_H
#define KOIVISTO_DENSENETWORK_H

#include "eval.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
namespace dense_network {


// compute amount of params from templates
template<int S1, int S2>
constexpr int count_params(){
    return S1 * S2 + S2;
}
template<int S1, int S2, int S3, int... S>
constexpr int count_params(){
    return S1 * S2 + S2 + count_params<S2, S3, S...>();
}

// compute maximum amount of weights per layer from templates
template<int S1, int S2>
constexpr int max_weights_per_layer(){
    return S1 * S2;
}
template<int S1, int S2, int S3, int... S>
constexpr int max_weights_per_layer(){
    return std::max(S1 * S2, max_weights_per_layer<S2, S3, S...>());
}

// compute input size from templates
template<int S1, int... S>
constexpr int count_input_neurons(){
    return S1;
}

// compute output size from templates
template<int S>
constexpr int count_output_neurons(){
    return S;
}
template<int S1, int S2, int... S>
constexpr int count_output_neurons(){
    return count_output_neurons<S2, S...>();
}

// compute layer count
template<int S>
constexpr int count_layers(){
    return 1;
}
template<int S1, int S2, int... S>
constexpr int count_layers(){
    return 1 + count_layers<S2, S...>();
}

// compute maximum layer size
template<int S1>
constexpr int max_layer_size(){
    return S1;
}
template<int S1, int S2, int... S>
constexpr int max_layer_size(){
    return std::max(S1, max_layer_size<S2, S...>());
}


// compute layer size by given index
template<int S1>
constexpr int layer_size(int idx){
    return S1;
}

template<int S1, int S2, int... S>
constexpr int layer_size(int idx){
    if(idx == 0) return S1;
    return layer_size<S2, S...>(idx-1);
}

inline float _mm256_reduce_add_ps(__m256 x) {
    /* ( x3+x7, x2+x6, x1+x5, x0+x4 ) */
    const __m128 x128 = _mm_add_ps(_mm256_extractf128_ps(x, 1), _mm256_castps256_ps128(x));
    /* ( -, -, x1+x3+x5+x7, x0+x2+x4+x6 ) */
    const __m128 x64 = _mm_add_ps(x128, _mm_movehl_ps(x128, x128));
    /* ( -, -, -, x0+x1+x2+x3+x4+x5+x6+x7 ) */
    const __m128 x32 = _mm_add_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
    /* Conversion to float is a no-op on x86-64 */
    return _mm_cvtss_f32(x32);
}

template<int... S>
class Network{
    
    alignas(ALIGNMENT) float weights[count_layers<S...>()][max_weights_per_layer<S...>()]{};
    alignas(ALIGNMENT) float bias   [count_layers<S...>()][max_layer_size       <S...>()]{};
    alignas(ALIGNMENT) float output [count_layers<S...>()][max_layer_size       <S...>()]{};
    alignas(ALIGNMENT) int   size   [count_layers<S...>()];
    
    public:
    Network(const float (&input) [count_params<S...>()]){
        int mem_ptr = 0;
        for(int i = 0; i < count_layers<S...>(); i++){
            size[i] = layer_size<S...>(i);
            if(i > 0){
                std::memcpy(weights[i], &input[mem_ptr], sizeof(float) * size[i] * size[i-1]);
                mem_ptr += size[i] * size[i-1];
                std::memcpy(bias   [i], &input[mem_ptr], sizeof(float) * size[i]);
                mem_ptr += size[i];
            }
        }
    }
    const float* feed(const float (&input) [count_input_neurons<S...>()] ){
        // copy to the output of the first layer
        for(int i = 0; i < size[0]; i++){
            output[0][i] = input[i];
        }
        
        // compute the layers
        // go through each layer
        for(int i = 1; i < count_layers<S...>(); i++){
            // go through each node and compute dot product
            for(int n = 0; n < size[i]; n++){
                __m256* wgt = (__m256*) &weights[i][n * size[i-1]];
                __m256* inp = (__m256*) output [i-1];
                
                __m256 sum{};
                
                // assume size of prev layer is a multiple of 8
                // only works on avx2 systems and higher
                for (int j = 0; j < size[i - 1] / 8; j++) {
                    sum = _mm256_fmadd_ps(wgt[j], inp[j], sum);
                }

                output[i][n] = std::max(0.0f,_mm256_reduce_add_ps(sum) + bias[i][n]);
            }
        }
        return output[ count_layers<S...>()-1];
    }
};
}

#endif    // KOIVISTO_DENSENETWORK_H
