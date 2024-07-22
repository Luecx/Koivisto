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
, -42
, -79
, 47
, 84
, 195
, -43
, -546
, 58
, 85
, 46
, 87
, 98
, 201
, -55
, -57
, -142
, -68
, -110
, -44
, -89
, 56
, -127
, -461
, -110
, 51
, -150
, -77
, -77
, 64
, 51
, -75
, -100
, -163
, 212
, -115
, -70
, 81
, 112
, 94
, 69
, 62
, 68
, 81
, -154
, -70
, -57
, 47
, 45
, -92
, -46
, -60
, -58
, -48
, -51
, -51
, 58
, -258
, -46
, -441
, -47
, 73
, -46
, -70
, -57
, 88
, 54
, 81
, -71
, -159
, -84
, -131
, -37
, -53
, -177
, -58
, -78
, -51
, -67
, -1476
, 103
, -92
, 45
, 77
, -62
, -292
, -156
, 77
, 98
, -46
, 47
, 49
, 112
, 59
, -65
, -69
, -41
, -57
, -76
, -187
, 66
, 43
, 132
, 136
, -68
, 461
, -182
, -58
, 53
, -81
, 134
, -42
, 60
, -102
, -49
, 55
, 58
, 35
, -41
, -108
, 54
, 89
, -168
, -90
, 59
, 53
, -83
, 77
, 61
, 60
, -60
, -69
, -64
, 77
, 93
, -261
, -161
, -104
, 109
, 43
, -42
, -90
, -43
, -115
, -130
, -297
, -74
, -36
, 42
, 78
, 90
, 63
, -51
, -127
, -452
, 48
, -46
, 103
, -61
, -254
, -47
, -164
, -63
, 44
, 47
, -1517
, 57
, 54
, 70
, -67
, 66
, 46
, -77
, 58
, 81
, -46
, -238
, 65
, -63
, 81
, -124
, -124
, 85
, 139
, -152
, 49
, 80
, -116
, 88
, -110
, -57
, -95
, -73
, 54
, -60
, -61
, -67
, -55
, -124
, -271
, -69
, 62
, -70
, 59
, -318
, 53
, 73
, 108
, 48
, -82
, -67
, -384
, -252
, 59
, 53
, -78
, -48
, -117
, 42
, -53
, 47
, -73
, -47
, 69
, 81
, -60
, 86
, 55
, 42
, 46
, 63
, 42
, 89
, -42
, -161
, -62
, 78
, 42
, 50
, 98
, -120
, -83
, -445
, 81
, -232
, 59
, 112
, -45
, 51
, 59
, 57
, -153};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


