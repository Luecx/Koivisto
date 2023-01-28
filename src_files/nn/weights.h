//
// Created by Luecx on 28.01.2023.
//

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
