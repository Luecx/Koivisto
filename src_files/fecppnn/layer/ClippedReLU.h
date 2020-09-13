//
// Created by finne on 9/13/2020.
//

#ifndef KOIVISTO_CLIPPEDRELU_H
#define KOIVISTO_CLIPPEDRELU_H

#include <immintrin.h>
#include "Layer.h"
class ClippedReLU : public Layer {
    
    public:
    ClippedReLU(Layer* prevLayer);;
    
    void compute() override;
    void backprop() override;
    
};

#endif    // KOIVISTO_CLIPPEDRELU_H
