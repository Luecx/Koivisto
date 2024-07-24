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
    int tw [256] = {-42
, -59
, 54
, -108
, -166
, -66
, -77
, 65
, 82
, 211
, -38
, -543
, 53
, 77
, 67
, 83
, 107
, 185
, -51
, -80
, -139
, -41
, -126
, -54
, -95
, 65
, -135
, -443
, -119
, 78
, -135
, -74
, -78
, 64
, 50
, -77
, -65
, -185
, 226
, -127
, -63
, 95
, 115
, 84
, 92
, 61
, 69
, 108
, -146
, -68
, -89
, 58
, 51
, -80
, -61
, -58
, -64
, -46
, -52
, -56
, 66
, -266
, -61
, -448
, -67
, 69
, -45
, -75
, -57
, 96
, 63
, 81
, -64
, -173
, -94
, -130
, -49
, -40
, -205
, -57
, -80
, -67
, -88
, -1520
, 120
, -103
, 42
, 83
, -73
, -305
, -178
, 67
, 97
, -34
, 52
, 50
, 116
, 57
, -53
, -83
, -52
, -71
, -74
, -191
, 78
, 48
, 115
, 124
, -71
, 500
, -179
, -73
, 85
, -57
, 122
, -54
, 65
, -102
, -52
, 59
, 67
, 50
, -47
, -89
, 59
, 100
, -178
, -104
, 64
, 68
, -88
, 87
, 44
, 77
, -65
, -68
, -79
, 77
, 111
, -273
, -177
, -109
, 118
, 69
, -56
, -81
, -74
, -156
, -130
, -301
, -87
, -53
, 51
, 92
, 112
, 81
, -44
, -131
, -452
, 85
, -46
, 91
, -54
, -288
, -74
, -184
, -63
, 42
, 68
, -1571
, 71
, 57
, 77
, -98
, 69
, 66
, -80
, 79
, 87
, -61
, -252
, 73
, -72
, 109
, -145
, -131
, 82
, 134
, -162
, 73
, 95
, -95
, 114
, -119
, -43
, -121
, -87
, 54
, -60
, -80
, -75
, -80
, -135
, -252
, -74
, 45
, -79
, 65
, -328
, 39
, 85
, 125
, 44
, -86
, -56
, -379
, -264
, 68
, 66
, -93
, -49
, -101
, 54
, -65
, 48
, -88
, -59
, 90
, 75
, -41
, 103
, 60
, 43
, 47
, 75
, 76
, 91
, -54
, -181
, -56
, 75
, 67
, 56
, 103
, -130
, -87
, -420
, 80
, -233
, 57
, 94
, -48
, 65
, 77
, 71
, -163
};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


