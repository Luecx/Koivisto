
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

#include "eval.h"

#include "UCIAssert.h"

int16_t nn::inputWeights [INPUT_SIZE ][HIDDEN_SIZE];
int16_t nn::hiddenWeights[OUTPUT_SIZE][HIDDEN_SIZE];
int16_t nn::inputBias    [HIDDEN_SIZE];
int16_t nn::hiddenBias   [OUTPUT_SIZE];

void    nn::init() {
    FILE *f = fopen("debug.net", "rb");
    
    // figure out how many entries we will store
    uint64_t count =
        + INPUT_SIZE * HIDDEN_SIZE
        + HIDDEN_SIZE
        + HIDDEN_SIZE * OUTPUT_SIZE
        + OUTPUT_SIZE;
   
    
    uint64_t fileCount = 0;
    fread(&fileCount, sizeof(uint64_t), 1, f);
    UCI_ASSERT((count ) == fileCount);
    
    float tempInputWeights [INPUT_SIZE ][HIDDEN_SIZE];
    float tempHiddenWeights[OUTPUT_SIZE][HIDDEN_SIZE];
    float tempInputBias    [HIDDEN_SIZE];
    float tempHiddenBias   [OUTPUT_SIZE];
    
    fread(tempInputWeights  , sizeof(float), INPUT_SIZE * HIDDEN_SIZE, f);
    fread(tempInputBias     , sizeof(float), HIDDEN_SIZE, f);
    fread(tempHiddenWeights , sizeof(float), HIDDEN_SIZE * OUTPUT_SIZE, f);
    fread(tempHiddenBias    , sizeof(float), OUTPUT_SIZE, f);
    
    for(int o = 0; o < HIDDEN_SIZE; o++){
        for(int i = 0; i < INPUT_SIZE; i++){
            inputWeights[i][o] = round(tempInputWeights[i][o] * 128);
        }
        inputBias[o] = round(tempInputBias[o] * 128);
    }
    
    for(int o = 0; o < OUTPUT_SIZE; o++){
        for(int i = 0; i < HIDDEN_SIZE; i++){
            hiddenWeights[o][i] = round(tempHiddenWeights[o][i] * 1024);
        }
        hiddenBias[o] = round(tempHiddenBias[o] * 1024);
    }

    fclose(f);
}
