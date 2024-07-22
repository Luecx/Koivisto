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
, -53
, 44
, -109
, -141
, -49
, -97
, 51
, 84
, 218
, -39
, -609
, 55
, 94
, 47
, 69
, 116
, 192
, -46
, -67
, -121
, -61
, -100
, -40
, -93
, 49
, -138
, -405
, -92
, 49
, -152
, -81
, -76
, 55
, 50
, -76
, -88
, -152
, 232
, -135
, -62
, 96
, 117
, 85
, 71
, 50
, 75
, 71
, -154
, -58
, -56
, 46
, 46
, -101
, -47
, -59
, -56
, -51
, -45
, -46
, 57
, -272
, -48
, -493
, -45
, 68
, -49
, -64
, -64
, 86
, 51
, 85
, -78
, -129
, -81
, -127
, -43
, -61
, -176
, -62
, -58
, -50
, -79
, -1710
, 116
, -107
, 45
, 77
, -49
, -296
, -152
, 86
, 115
, -42
, 46
, 52
, 104
, 65
, -65
, -80
, -42
, -56
, -82
, -210
, 68
, 40
, 112
, 136
, -76
, 544
, -208
, -62
, 52
, -91
, 124
, -40
, 57
, -100
, -53
, 55
, 57
, 37
, -38
, -105
, 43
, 99
, -181
, -78
, 57
, 58
, -86
, 76
, 78
, 70
, -67
, -74
, -50
, 81
, 93
, -237
, -145
, -85
, 111
, 37
, -39
, -85
, -42
, -124
, -131
, -302
, -91
, -38
, 44
, 82
, 88
, 66
, -53
, -130
, -460
, 43
, -55
, 106
, -60
, -310
, -49
, -166
, -73
, 50
, 55
, -1670
, 58
, 46
, 74
, -58
, 71
, 49
, -90
, 54
, 97
, -42
, -216
, 64
, -59
, 90
, -110
, -112
, 81
, 129
, -169
, 47
, 81
, -107
, 88
, -135
, -50
, -86
, -64
, 57
, -57
, -52
, -70
, -59
, -119
, -255
, -52
, 70
, -67
, 50
, -388
, 56
, 71
, 100
, 49
, -85
, -68
, -408
, -253
, 63
, 57
, -79
, -42
, -125
, 44
, -48
, 50
, -69
, -48
, 68
, 74
, -53
, 72
, 51
, 52
, 48
, 57
, 45
, 100
, -49
, -153
, -62
, 80
, 36
, 43
, 78
, -131
, -80
, -461
, 93
, -250
, 66
, 112
, -47
, 53
, 62
, 53
, -141
    };
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


