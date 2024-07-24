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
    int tw [256] = {-43
, -55
, 40
, -101
, -134
, -42
, -81
, 46
, 88
, 195
, -42
, -527
, 56
, 81
, 49
, 86
, 96
, 198
, -53
, -63
, -139
, -69
, -109
, -39
, -88
, 56
, -124
, -457
, -108
, 53
, -155
, -69
, -80
, 63
, 51
, -77
, -97
, -161
, 210
, -115
, -72
, 87
, 107
, 94
, 68
, 63
, 68
, 83
, -156
, -65
, -57
, 50
, 43
, -85
, -46
, -58
, -65
, -50
, -47
, -47
, 64
, -268
, -44
, -442
, -48
, 74
, -45
, -64
, -62
, 89
, 57
, 84
, -73
, -158
, -83
, -136
, -33
, -52
, -181
, -56
, -73
, -47
, -64
, -1522
, 105
, -93
, 41
, 78
, -61
, -298
, -160
, 74
, 99
, -41
, 49
, 49
, 110
, 56
, -63
, -70
, -43
, -56
, -81
, -190
, 66
, 46
, 131
, 140
, -70
, 478
, -180
, -57
, 53
, -89
, 136
, -40
, 63
, -95
, -45
, 54
, 60
, 36
, -38
, -108
, 49
, 89
, -171
, -94
, 58
, 51
, -82
, 80
, 62
, 66
, -57
, -67
, -68
, 81
, 94
, -261
, -165
, -109
, 118
, 42
, -37
, -93
, -38
, -124
, -134
, -296
, -78
, -34
, 47
, 81
, 90
, 66
, -46
, -125
, -446
, 49
, -47
, 112
, -62
, -263
, -50
, -166
, -65
, 45
, 42
, -1562
, 60
, 56
, 70
, -68
, 71
, 49
, -80
, 56
, 81
, -49
, -246
, 68
, -60
, 87
, -124
, -119
, 87
, 133
, -157
, 53
, 80
, -111
, 91
, -117
, -53
, -99
, -76
, 51
, -55
, -62
, -64
, -52
, -118
, -259
, -71
, 60
, -66
, 54
, -337
, 54
, 76
, 108
, 53
, -85
, -70
, -374
, -252
, 61
, 56
, -84
, -55
, -119
, 41
, -53
, 48
, -71
, -41
, 69
, 76
, -60
, 93
, 58
, 42
, 52
, 64
, 44
, 90
, -45
, -161
, -60
, 76
, 39
, 54
, 98
, -124
, -87
, -455
, 77
, -240
, 60
, 116
, -46
, 53
, 63
, 57
, -155
};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


