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
    int tw [256] = { -45
, -52
, 41
, -105
, -129
, -42
, -78
, 47
, 84
, 197
, -43
, -549
, 58
, 85
, 47
, 89
, 97
, 198
, -55
, -57
, -142
, -68
, -110
, -43
, -87
, 57
, -128
, -458
, -109
, 50
, -150
, -75
, -76
, 64
, 51
, -74
, -98
, -163
, 208
, -116
, -71
, 79
, 111
, 96
, 70
, 63
, 69
, 82
, -151
, -69
, -56
, 46
, 45
, -91
, -46
, -59
, -58
, -49
, -51
, -51
, 59
, -260
, -46
, -437
, -46
, 75
, -46
, -70
, -58
, 88
, 55
, 82
, -73
, -163
, -83
, -133
, -36
, -53
, -177
, -57
, -78
, -51
, -67
, -1441
, 104
, -92
, 45
, 78
, -62
, -287
, -157
, 77
, 98
, -45
, 47
, 50
, 113
, 58
, -67
, -70
, -42
, -57
, -77
, -186
, 65
, 42
, 136
, 140
, -69
, 459
, -186
, -58
, 54
, -83
, 136
, -43
, 59
, -101
, -49
, 54
, 58
, 35
, -41
, -110
, 55
, 90
, -168
, -91
, 60
, 54
, -84
, 75
, 62
, 59
, -59
, -70
, -63
, 77
, 93
, -254
, -166
, -104
, 109
, 45
, -43
, -90
, -44
, -114
, -127
, -292
, -75
, -36
, 42
, 78
, 90
, 63
, -51
, -127
, -453
, 48
, -46
, 105
, -61
, -251
, -46
, -165
, -63
, 44
, 47
, -1494
, 57
, 55
, 71
, -68
, 66
, 46
, -77
, 59
, 80
, -46
, -238
, 65
, -65
, 82
, -124
, -125
, 85
, 139
, -151
, 49
, 77
, -113
, 89
, -108
, -56
, -96
, -75
, 53
, -60
, -63
, -67
, -56
, -125
, -274
, -69
, 62
, -70
, 58
, -319
, 53
, 74
, 110
, 48
, -83
, -67
, -392
, -246
, 59
, 53
, -77
, -48
, -115
, 43
, -54
, 47
, -75
, -46
, 68
, 81
, -60
, 88
, 55
, 42
, 45
, 63
, 42
, 91
, -41
, -161
, -60
, 80
, 42
, 50
, 97
, -122
, -84
, -446
, 82
, -233
, 60
, 113
, -44
, 51
, 58
, 57
, -156};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


