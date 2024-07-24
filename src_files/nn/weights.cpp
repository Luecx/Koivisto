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
, -49
, 42
, -102
, -133
, -44
, -80
, 48
, 84
, 188
, -42
, -521
, 57
, 86
, 47
, 86
, 95
, 194
, -55
, -58
, -142
, -67
, -108
, -45
, -88
, 56
, -123
, -456
, -108
, 52
, -152
, -76
, -79
, 64
, 50
, -75
, -97
, -169
, 214
, -112
, -69
, 81
, 114
, 95
, 72
, 62
, 67
, 81
, -152
, -69
, -56
, 47
, 45
, -92
, -47
, -59
, -58
, -49
, -50
, -52
, 59
, -271
, -47
, -446
, -48
, 75
, -46
, -70
, -58
, 87
, 55
, 82
, -72
, -157
, -84
, -134
, -37
, -52
, -182
, -58
, -77
, -51
, -70
, -1515
, 104
, -92
, 44
, 76
, -62
, -300
, -167
, 81
, 93
, -46
, 47
, 48
, 110
, 59
, -67
, -72
, -39
, -56
, -76
, -184
, 65
, 44
, 133
, 135
, -68
, 479
, -183
, -59
, 53
, -80
, 137
, -41
, 58
, -102
, -49
, 54
, 58
, 35
, -42
, -109
, 52
, 86
, -173
, -88
, 57
, 50
, -85
, 77
, 63
, 59
, -59
, -70
, -67
, 79
, 92
, -256
, -160
, -107
, 112
, 42
, -41
, -94
, -42
, -115
, -131
, -297
, -75
, -36
, 43
, 80
, 93
, 64
, -51
, -130
, -445
, 47
, -47
, 103
, -63
, -266
, -48
, -163
, -64
, 46
, 48
, -1530
, 58
, 54
, 70
, -66
, 65
, 48
, -78
, 57
, 80
, -47
, -246
, 64
, -64
, 83
, -127
, -120
, 85
, 140
, -154
, 48
, 80
, -111
, 86
, -114
, -55
, -93
, -72
, 53
, -62
, -61
, -66
, -56
, -125
, -262
, -68
, 64
, -69
, 55
, -332
, 53
, 73
, 105
, 48
, -81
, -66
, -369
, -249
, 57
, 52
, -78
, -47
, -118
, 42
, -51
, 48
, -75
, -46
, 68
, 80
, -60
, 90
, 56
, 42
, 45
, 63
, 44
, 86
, -42
, -156
, -62
, 77
, 42
, 51
, 95
, -119
, -83
, -452
, 83
, -244
, 62
, 112
, -45
, 51
, 59
, 56
, -157
};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


