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
    int tw [256] = {-48
, -51
, 43
, -96
, -147
, -45
, -85
, 47
, 83
, 203
, -38
, -561
, 59
, 86
, 48
, 86
, 97
, 208
, -53
, -62
, -142
, -67
, -116
, -42
, -85
, 51
, -132
, -445
, -106
, 53
, -142
, -67
, -83
, 67
, 46
, -79
, -102
, -166
, 216
, -108
, -69
, 76
, 103
, 87
, 71
, 64
, 70
, 84
, -153
, -72
, -53
, 46
, 44
, -88
, -46
, -54
, -61
, -51
, -47
, -45
, 68
, -271
, -48
, -438
, -50
, 71
, -47
, -74
, -64
, 95
, 55
, 88
, -69
, -158
, -85
, -149
, -34
, -53
, -194
, -59
, -73
, -53
, -68
, -1335
, 107
, -88
, 40
, 75
, -60
, -300
, -163
, 75
, 96
, -45
, 46
, 45
, 114
, 61
, -61
, -67
, -41
, -54
, -82
, -177
, 71
, 51
, 140
, 143
, -71
, 442
, -180
, -56
, 53
, -81
, 133
, -39
, 62
, -95
, -51
, 51
, 60
, 34
, -40
, -105
, 55
, 92
, -168
, -86
, 62
, 49
, -80
, 67
, 58
, 59
, -55
, -75
, -61
, 77
, 95
, -249
, -161
, -104
, 112
, 43
, -38
, -96
, -41
, -125
, -124
, -281
, -75
, -36
, 43
, 76
, 99
, 67
, -49
, -115
, -416
, 46
, -46
, 92
, -58
, -270
, -46
, -175
, -63
, 45
, 46
, -1422
, 53
, 50
, 73
, -66
, 63
, 50
, -74
, 55
, 87
, -44
, -249
, 68
, -63
, 84
, -133
, -121
, 80
, 144
, -155
, 47
, 81
, -110
, 94
, -110
, -51
, -101
, -76
, 51
, -60
, -70
, -68
, -54
, -118
, -269
, -72
, 68
, -70
, 56
, -338
, 51
, 68
, 109
, 51
, -81
, -64
, -387
, -244
, 56
, 53
, -76
, -49
, -107
, 43
, -52
, 52
, -83
, -45
, 73
, 88
, -54
, 92
, 56
, 43
, 45
, 69
, 39
, 86
, -42
, -144
, -61
, 79
, 37
, 51
, 89
, -115
, -76
, -476
, 78
, -241
, 66
, 113
, -44
, 57
, 61
, 57
, -151
};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


