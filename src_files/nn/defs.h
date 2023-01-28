//
// Created by Luecx on 28.01.2023.
//

#ifndef KOIVISTO_DEFS_H
#define KOIVISTO_DEFS_H

#include "../bitboard.h"
#include "../uciassert.h"

#include <cstdint>
#include <cstring>
#if defined(__ARM_NEON)
#include <arm_neon.h>
#else
#include <immintrin.h>
#endif
#include <vector>
#include <memory>

#define INPUT_SIZE     (bb::N_PIECE_TYPES * bb::N_SQUARES * 2 * 16)
#define HIDDEN_SIZE    (512)
#define HIDDEN_DSIZE   (HIDDEN_SIZE * 2)
#define OUTPUT_SIZE    (1)

#if defined(__AVX512F__)
#define BIT_ALIGNMENT  (512)
#elif defined(__AVX2__) || defined(__AVX__)
#define BIT_ALIGNMENT  (256)
#elif defined(__SSE2__) || defined(__ARM_NEON)
#define BIT_ALIGNMENT  (128)
#endif
#define STRIDE_16_BIT  (BIT_ALIGNMENT / 16)
#define BYTE_ALIGNMENT (BIT_ALIGNMENT / 8)
#define ALIGNMENT      (BYTE_ALIGNMENT)



#define INPUT_WEIGHT_MULTIPLIER  (32)
#define HIDDEN_WEIGHT_MULTIPLIER (128)

#if defined(__AVX512F__)
using avx_register_type_16 = __m512i;
using avx_register_type_32 = __m512i;
#define avx_madd_epi16(a, b) (_mm512_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm512_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm512_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm512_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm512_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm512_max_epi16(a, b))
#elif defined(__AVX2__) || defined(__AVX__)
using avx_register_type_16 = __m256i;
using avx_register_type_32 = __m256i;
#define avx_madd_epi16(a, b) (_mm256_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm256_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm256_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm256_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm256_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm256_max_epi16(a, b))
#elif defined(__SSE2__)
using avx_register_type_16 = __m128i;
using avx_register_type_32 = __m128i;
#define avx_madd_epi16(a, b) (_mm_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm_max_epi16(a, b))
#elif defined(__ARM_NEON)
using avx_register_type_16 = int16x8_t;
using avx_register_type_32 = int32x4_t;
#define avx_madd_epi16(a, b)                                                                         \
    (vpaddq_s32(vmull_s16(vget_low_s16(a), vget_low_s16(b)), vmull_high_s16(a, b)))
#define avx_add_epi32(a, b) (vaddq_s32(a, b))
#define avx_sub_epi32(a, b) (vsubq_s32(a, b))
#define avx_add_epi16(a, b) (vaddq_s16(a, b))
#define avx_sub_epi16(a, b) (vsubq_s16(a, b))
#define avx_max_epi16(a, b) (vmaxq_s16(a, b))
#endif

class Board;
namespace nn{
    struct Evaluator;
}

#endif    // KOIVISTO_DEFS_H
