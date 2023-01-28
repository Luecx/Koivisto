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

#ifndef KOIVISTO_WEIGHTS_H
#define KOIVISTO_WEIGHTS_H

#include "defs.h"

namespace nn{

extern int16_t inputWeights [INPUT_SIZE ][HIDDEN_SIZE];
extern int16_t hiddenWeights[OUTPUT_SIZE][HIDDEN_DSIZE];
extern int16_t inputBias    [HIDDEN_SIZE];
extern int32_t hiddenBias   [OUTPUT_SIZE];

// initialise and load the weights
void init();
}

#endif    // KOIVISTO_WEIGHTS_H
