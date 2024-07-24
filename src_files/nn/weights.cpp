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
, -50
, 47
, -96
, -128
, -32
, -69
, 55
, 81
, 195
, -32
, -528
, 63
, 82
, 58
, 83
, 106
, 192
, -47
, -66
, -146
, -68
, -114
, -43
, -97
, 51
, -124
, -463
, -108
, 45
, -153
, -70
, -75
, 69
, 45
, -73
, -102
, -167
, 218
, -108
, -72
, 81
, 111
, 95
, 74
, 68
, 67
, 85
, -159
, -86
, -66
, 49
, 55
, -95
, -52
, -62
, -55
, -49
, -55
, -38
, 63
, -260
, -47
, -445
, -56
, 77
, -43
, -74
, -53
, 88
, 55
, 82
, -71
, -147
, -83
, -124
, -42
, -59
, -185
, -50
, -72
, -49
, -67
, -1519
, 112
, -98
, 44
, 91
, -63
, -296
, -170
, 73
, 99
, -45
, 48
, 45
, 111
, 69
, -53
, -75
, -48
, -59
, -89
, -187
, 64
, 51
, 129
, 134
, -71
, 470
, -186
, -56
, 55
, -86
, 133
, -44
, 59
, -97
, -54
, 61
, 64
, 38
, -47
, -113
, 48
, 95
, -181
, -82
, 63
, 57
, -85
, 79
, 56
, 66
, -68
, -63
, -67
, 78
, 100
, -256
, -174
, -106
, 117
, 47
, -49
, -90
, -38
, -119
, -132
, -294
, -73
, -27
, 49
, 82
, 99
, 71
, -45
, -128
, -446
, 51
, -50
, 106
, -69
, -271
, -49
, -157
, -65
, 49
, 48
, -1558
, 61
, 46
, 64
, -66
, 73
, 49
, -74
, 59
, 78
, -50
, -238
, 65
, -71
, 81
, -131
, -128
, 83
, 143
, -164
, 47
, 75
, -110
, 87
, -118
, -63
, -95
, -81
, 46
, -52
, -66
, -51
, -53
, -134
, -268
, -64
, 63
, -68
, 52
, -325
, 47
, 76
, 106
, 46
, -84
, -72
, -377
, -252
, 58
, 51
, -79
, -41
, -109
, 46
, -57
, 54
, -67
, -50
, 74
, 76
, -52
, 95
, 58
, 43
, 39
, 65
, 43
, 90
, -42
, -149
, -64
, 75
, 44
, 59
, 95
, -116
, -79
, -460
, 76
, -239
, 60
, 120
, -40
, 49
, 69
, 66
, -155
};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


