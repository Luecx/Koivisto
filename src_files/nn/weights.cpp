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
#include "weights.h"

#define INCBIN_STYLE INCBIN_STYLE_CAMEL
#include "../incbin/incbin.h"
INCBIN(Eval, EVALFILE);

// clang-format off
alignas(ALIGNMENT) int16_t nn::inputWeights [INPUT_SIZE ][HIDDEN_SIZE ];
alignas(ALIGNMENT) int16_t nn::hiddenWeights[OUTPUT_SIZE][HIDDEN_DSIZE];
alignas(ALIGNMENT) int16_t nn::inputBias    [HIDDEN_SIZE];
alignas(ALIGNMENT) int32_t nn::hiddenBias   [OUTPUT_SIZE];
// clang-format on

void nn::init() {
    int memoryIndex = 0;
    std::memcpy(inputWeights, &gEvalData[memoryIndex], INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex += INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t);
    std::memcpy(inputBias, &gEvalData[memoryIndex], HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex += HIDDEN_SIZE * sizeof(int16_t);
    
    std::memcpy(hiddenWeights, &gEvalData[memoryIndex], HIDDEN_DSIZE * OUTPUT_SIZE * sizeof(int16_t));
    memoryIndex += HIDDEN_DSIZE * OUTPUT_SIZE * sizeof(int16_t);
    int tw [256] = {-47
, -51
, 41
, -102
, -133
, -43
, -79
, 48
, 84
, 195
, -43
, -522
, 57
, 87
, 46
, 84
, 97
, 194
, -55
, -58
, -143
, -69
, -110
, -45
, -88
, 56
, -125
, -460
, -109
, 52
, -151
, -76
, -78
, 63
, 50
, -75
, -96
, -165
, 209
, -115
, -69
, 81
, 111
, 96
, 71
, 62
, 69
, 81
, -153
, -69
, -56
, 47
, 44
, -93
, -47
, -60
, -58
, -49
, -50
, -52
, 58
, -265
, -47
, -445
, -48
, 74
, -46
, -70
, -61
, 87
, 55
, 81
, -72
, -155
, -85
, -131
, -37
, -53
, -182
, -58
, -75
, -51
, -69
, -1511
, 105
, -92
, 44
, 76
, -63
, -304
, -164
, 79
, 95
, -46
, 47
, 47
, 110
, 60
, -66
, -72
, -40
, -56
, -77
, -185
, 65
, 44
, 130
, 137
, -68
, 477
, -184
, -59
, 53
, -82
, 137
, -41
, 58
, -101
, -49
, 54
, 58
, 35
, -42
, -109
, 53
, 88
, -171
, -89
, 57
, 50
, -84
, 77
, 63
, 58
, -59
, -70
, -65
, 76
, 93
, -261
, -159
, -106
, 111
, 42
, -42
, -93
, -42
, -116
, -130
, -295
, -74
, -36
, 42
, 79
, 91
, 63
, -51
, -129
, -449
, 48
, -47
, 102
, -63
, -262
, -47
, -166
, -64
, 45
, 47
, -1583
, 58
, 54
, 69
, -66
, 65
, 47
, -78
, 57
, 81
, -47
, -244
, 64
, -62
, 83
, -126
, -120
, 86
, 137
, -158
, 48
, 82
, -112
, 87
, -112
, -54
, -95
, -73
, 53
, -62
, -61
, -66
, -56
, -124
, -265
, -68
, 64
, -68
, 56
, -328
, 53
, 73
, 104
, 48
, -80
, -67
, -372
, -251
, 57
, 52
, -80
, -47
, -117
, 42
, -51
, 48
, -73
, -48
, 68
, 81
, -59
, 88
, 56
, 42
, 45
, 61
, 43
, 88
, -42
, -158
, -63
, 78
, 41
, 50
, 96
, -117
, -82
, -457
, 81
, -242
, 60
, 113
, -45
, 52
, 59
, 56
, -154};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


