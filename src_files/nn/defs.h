/****************************************************************************************************
*                                                                                                  *
*                                     Koivisto UCI Chess engine                                    *
*                                   by. Kim Kahre and Finn Eggers                                  *
*                                                                                                  *
*                 Koivisto is free software: you can redistribute it and/or modify                 *
*               it under the terms of the GNU General Public License as published by               *
*                 the Free Software Foundation, either version 3 of the License, or                *
*                                (at your option) any later version.                               *
*                    Koivisto is distributed in the hope that it will be useful,                   *
*                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
*                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
*                           GNU General Public License for more details.                           *
*                 You should have received a copy of the GNU General Public License                *
*                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
*                                                                                                  *
****************************************************************************************************/

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
#define CHUNK_UNROLL_SIZE (BIT_ALIGNMENT)
#define REG_COUNT (16)


#define INPUT_WEIGHT_MULTIPLIER  (32)
#define HIDDEN_WEIGHT_MULTIPLIER (128)

#if defined(__AVX512F__)
using avx_register_type_16 = __m512i;
using avx_register_type_32 = __m512i;
#define avx_madd_epi16 _mm512_madd_epi16
#define avx_add_epi32  _mm512_add_epi32
#define avx_sub_epi32  _mm512_sub_epi32
#define avx_add_epi16  _mm512_add_epi16
#define avx_sub_epi16  _mm512_sub_epi16
#define avx_max_epi16  _mm512_max_epi16
#define avx_load_reg   _mm512_load_si512
#define avx_store_reg  _mm512_store_si512
#elif defined(__AVX2__) || defined(__AVX__)
using avx_register_type_16 = __m256i;
using avx_register_type_32 = __m256i;
#define avx_madd_epi16 _mm256_madd_epi16
#define avx_load_reg   _mm256_load_si256
#define avx_store_reg  _mm256_store_si256
#define avx_add_epi32  _mm256_add_epi32
#define avx_sub_epi32  _mm256_sub_epi32
#define avx_add_epi16  _mm256_add_epi16
#define avx_sub_epi16  _mm256_sub_epi16
#define avx_max_epi16  _mm256_max_epi16
#elif defined(__SSE2__)
using avx_register_type_16 = __m128i;
using avx_register_type_32 = __m128i;
#define avx_madd_epi16 _mm_madd_epi16
#define avx_add_epi32  _mm_add_epi32
#define avx_sub_epi32  _mm_sub_epi32
#define avx_add_epi16  _mm_add_epi16
#define avx_sub_epi16  _mm_sub_epi16
#define avx_max_epi16  _mm_max_epi16
#define avx_load_reg   _mm_load_si128
#define avx_store_reg  _mm_store_si128
#elif defined(__ARM_NEON)
using avx_register_type_16 = int16x8_t;
using avx_register_type_32 = int32x4_t;
#define avx_madd_epi16(a, b)                                                                         \
    (vpaddq_s32(vmull_s16(vget_low_s16(a), vget_low_s16(b)), vmull_high_s16(a, b)))
#define avx_add_epi32 vaddq_s32
#define avx_sub_epi32 vsubq_s32
#define avx_add_epi16 vaddq_s16
#define avx_sub_epi16 vsubq_s16
#define avx_max_epi16 vmaxq_s16
#define avx_load_reg  vldrq_p128
#define avx_store_reg exit(-1)
#endif

class Board;
namespace nn{
    struct Evaluator;
}

#endif    // KOIVISTO_DEFS_H
