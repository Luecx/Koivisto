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
    int tw [256] = {-51
, -48
, 46
, -108
, -138
, -41
, -78
, 58
, 73
, 181
, -50
, -557
, 64
, 93
, 56
, 82
, 111
, 159
, -61
, -62
, -115
, -70
, -106
, -52
, -107
, 59
, -135
, -464
, -88
, 56
, -160
, -63
, -82
, 82
, 49
, -88
, -81
, -159
, 199
, -129
, -74
, 82
, 124
, 96
, 71
, 54
, 74
, 82
, -163
, -77
, -58
, 58
, 53
, -92
, -43
, -63
, -53
, -45
, -61
, -50
, 65
, -296
, -45
, -523
, -45
, 85
, -52
, -71
, -65
, 91
, 48
, 82
, -83
, -159
, -90
, -151
, -41
, -56
, -187
, -58
, -71
, -54
, -72
, -1456
, 98
, -109
, 44
, 86
, -71
, -289
, -174
, 81
, 108
, -51
, 44
, 44
, 101
, 57
, -62
, -67
, -40
, -46
, -59
, -179
, 86
, 46
, 174
, 118
, -63
, 527
, -178
, -63
, 61
, -97
, 166
, -40
, 60
, -119
, -58
, 62
, 63
, 43
, -42
, -99
, 48
, 94
, -207
, -83
, 58
, 55
, -82
, 81
, 71
, 73
, -58
, -75
, -76
, 76
, 99
, -298
, -181
, -104
, 90
, 43
, -46
, -78
, -53
, -130
, -134
, -253
, -76
, -26
, 46
, 80
, 93
, 62
, -44
, -121
, -514
, 49
, -51
, 99
, -50
, -250
, -42
, -176
, -57
, 56
, 35
, -1568
, 66
, 57
, 64
, -68
, 87
, 44
, -84
, 57
, 78
, -51
, -202
, 58
, -54
, 91
, -130
, -136
, 92
, 132
, -160
, 55
, 69
, -118
, 95
, -120
, -57
, -101
, -80
, 50
, -53
, -65
, -81
, -53
, -125
, -275
, -60
, 59
, -79
, 47
, -258
, 49
, 72
, 135
, 50
, -74
, -73
, -384
, -257
, 50
, 62
, -73
, -54
, -119
, 41
, -41
, 45
, -74
, -49
, 70
, 89
, -59
, 112
, 48
, 42
, 53
, 61
, 43
, 66
, -41
, -182
, -67
, 75
, 34
, 58
, 113
, -154
, -78
, -477
, 105
, -275
, 56
, 121
, -49
, 63
, 59
, 57
, -159};
    for (int i = 0; i < 256; i++) {
        hiddenWeights[0][i] = tw[i];
    }
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);
}


