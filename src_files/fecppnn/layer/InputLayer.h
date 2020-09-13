

#ifndef KOIVISTO_INPUTLAYER_H
#define KOIVISTO_INPUTLAYER_H

#include "Layer.h"
class InputLayer : public Layer {
    
    
    public:
    void compute() override {}
    void backprop() override {}
    
    public:
    InputLayer(int size): Layer(size){}

};

#endif    // KOIVISTO_INPUTLAYER_H
