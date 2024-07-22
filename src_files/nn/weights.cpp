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
    int tw [256] = {-45
, -51
, 41
, -104
, -132
, -43
, -80
, 48
, 83
, 196
, -43
, -531
, 58
, 86
, 46
, 86
, 98
, 198
, -55
, -57
, -142
, -68
, -109
, -44
, -88
, 56
, -125
, -453
, -109
, 51
, -152
, -76
, -78
, 64
, 51
, -75
, -99
, -164
, 211
, -114
, -71
, 81
, 113
, 96
, 70
, 62
, 69
, 81
, -153
, -69
, -57
, 47
, 45
, -91
, -46
, -59
, -58
, -49
, -51
, -51
, 58
, -258
, -46
, -440
, -47
, 73
, -46
, -71
, -59
, 87
, 54
, 81
, -72
, -159
, -82
, -131
, -37
, -54
, -177
, -57
, -76
, -51
, -68
, -1470
, 103
, -91
, 45
, 76
, -63
, -296
, -160
, 79
, 96
, -46
, 47
, 48
, 112
, 59
, -65
, -69
, -40
, -56
, -77
, -186
, 65
, 44
, 134
, 136
, -67
, 463
, -181
, -59
, 53
, -81
, 136
, -42
, 59
, -102
, -49
, 54
, 59
, 34
, -42
, -108
, 53
, 90
, -166
, -90
, 58
, 52
, -85
, 76
, 61
, 60
, -59
, -69
, -64
, 77
, 93
, -264
, -161
, -106
, 110
, 43
, -42
, -90
, -43
, -116
, -131
, -290
, -74
, -36
, 43
, 79
, 90
, 62
, -51
, -128
, -452
, 48
, -47
, 103
, -61
, -258
, -46
, -163
, -63
, 45
, 47
, -1531
, 57
, 55
, 70
, -67
, 66
, 46
, -78
, 57
, 81
, -47
, -237
, 63
, -63
, 81
, -125
, -123
, 86
, 137
, -154
, 49
, 81
, -114
, 88
, -111
, -56
, -95
, -74
, 53
, -60
, -60
, -68
, -56
, -125
, -272
, -68
, 62
, -69
, 57
, -325
, 53
, 73
, 107
, 47
, -82
, -66
, -380
, -250
, 58
, 53
, -78
, -48
, -117
, 42
, -52
, 48
, -74
, -48
, 69
, 81
, -61
, 87
, 56
, 42
, 45
, 63
, 42
, 89
, -42
, -162
, -61
, 78
, 41
, 50
, 99
, -118
, -82
, -448
, 81
, -234
, 59
, 114
, -45
, 51
, 59
, 55
, -155
    };
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


