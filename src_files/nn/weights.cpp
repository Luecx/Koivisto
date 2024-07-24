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
    int tw [256] = {-46
, -53
, 40
, -96
, -127
, -44
, -79
, 49
, 87
, 195
, -41
, -534
, 55
, 88
, 48
, 89
, 90
, 194
, -55
, -60
, -147
, -68
, -111
, -43
, -85
, 55
, -120
, -443
, -107
, 50
, -162
, -74
, -79
, 64
, 56
, -73
, -98
, -180
, 218
, -113
, -69
, 78
, 112
, 92
, 65
, 68
, 71
, 81
, -147
, -68
, -58
, 46
, 43
, -92
, -47
, -56
, -58
, -50
, -51
, -50
, 60
, -267
, -46
, -441
, -46
, 71
, -45
, -70
, -59
, 88
, 53
, 89
, -70
, -159
, -83
, -135
, -39
, -52
, -193
, -57
, -74
, -48
, -74
, -1423
, 107
, -96
, 43
, 75
, -65
, -296
, -164
, 77
, 92
, -45
, 44
, 48
, 107
, 65
, -63
, -72
, -41
, -51
, -77
, -193
, 64
, 47
, 141
, 141
, -65
, 438
, -196
, -57
, 55
, -79
, 146
, -41
, 59
, -102
, -44
, 54
, 59
, 36
, -45
, -104
, 54
, 87
, -171
, -92
, 59
, 50
, -85
, 83
, 67
, 62
, -56
, -70
, -63
, 77
, 96
, -267
, -158
, -104
, 114
, 45
, -42
, -95
, -44
, -109
, -132
, -290
, -73
, -37
, 39
, 80
, 92
, 64
, -46
, -123
, -450
, 50
, -48
, 101
, -63
, -253
, -45
, -174
, -62
, 44
, 48
, -1447
, 57
, 53
, 70
, -61
, 63
, 46
, -81
, 58
, 80
, -48
, -237
, 62
, -62
, 83
, -127
, -129
, 87
, 146
, -162
, 50
, 78
, -116
, 86
, -111
, -53
, -96
, -75
, 50
, -58
, -63
, -64
, -54
, -124
, -270
, -72
, 60
, -66
, 56
, -335
, 53
, 78
, 101
, 46
, -78
, -68
, -365
, -259
, 54
, 54
, -78
, -46
, -112
, 41
, -52
, 47
, -75
, -47
, 74
, 82
, -57
, 90
, 62
, 45
, 46
, 63
, 43
, 90
, -39
, -166
, -67
, 78
, 41
, 48
, 99
, -119
, -85
, -419
, 84
, -245
, 58
, 117
, -43
, 54
, 64
, 57
, -154};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


