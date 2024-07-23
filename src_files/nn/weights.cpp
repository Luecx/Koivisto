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
, -52
, 41
, -103
, -134
, -45
, -78
, 46
, 86
, 196
, -40
, -558
, 61
, 90
, 47
, 87
, 99
, 198
, -55
, -57
, -147
, -71
, -110
, -45
, -87
, 55
, -130
, -479
, -113
, 50
, -150
, -77
, -78
, 64
, 51
, -74
, -100
, -166
, 219
, -115
, -71
, 83
, 117
, 95
, 72
, 60
, 68
, 80
, -147
, -70
, -57
, 47
, 44
, -91
, -46
, -55
, -59
, -49
, -48
, -53
, 57
, -267
, -48
, -428
, -47
, 75
, -46
, -67
, -61
, 91
, 57
, 82
, -70
, -155
, -83
, -134
, -37
, -52
, -179
, -58
, -72
, -50
, -70
, -1470
, 101
, -93
, 44
, 75
, -63
, -296
, -157
, 78
, 96
, -45
, 47
, 45
, 113
, 62
, -64
, -71
, -40
, -55
, -77
, -181
, 64
, 45
, 139
, 135
, -71
, 457
, -185
, -59
, 52
, -83
, 139
, -41
, 61
, -101
, -50
, 58
, 59
, 35
, -42
, -109
, 55
, 89
, -172
, -93
, 61
, 52
, -83
, 78
, 62
, 61
, -57
, -71
, -63
, 77
, 98
, -272
, -165
, -107
, 113
, 42
, -42
, -91
, -42
, -119
, -126
, -299
, -74
, -35
, 43
, 80
, 94
, 65
, -50
, -138
, -461
, 47
, -48
, 98
, -61
, -269
, -48
, -163
, -64
, 47
, 47
, -1529
, 59
, 56
, 68
, -64
, 67
, 45
, -81
, 57
, 79
, -47
, -240
, 66
, -63
, 85
, -127
, -119
, 85
, 135
, -157
, 48
, 78
, -111
, 88
, -110
, -55
, -95
, -73
, 54
, -60
, -58
, -68
, -56
, -123
, -272
, -67
, 66
, -67
, 57
, -333
, 53
, 75
, 100
, 48
, -81
, -69
, -386
, -251
, 57
, 51
, -79
, -45
, -123
, 41
, -50
, 47
, -75
, -47
, 68
, 82
, -59
, 88
, 56
, 43
, 46
, 62
, 43
, 89
, -42
, -158
, -64
, 77
, 43
, 49
, 92
, -121
, -81
, -448
, 81
, -237
, 62
, 113
, -44
, 51
, 60
, 58
, -158};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


