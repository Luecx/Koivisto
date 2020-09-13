
#ifndef KOIVISTO_CONCAT_H
#define KOIVISTO_CONCAT_H

#include "Layer.h"
class Concat : public Layer {

    public:
    Concat(Layer* prev1, Layer* prev2);

    void compute() override;
    void backprop() override;
};

#endif    // KOIVISTO_CONCAT_H
