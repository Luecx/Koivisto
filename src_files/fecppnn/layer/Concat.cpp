

#include "Concat.h"

Concat::Concat(Layer* prev1, Layer* prev2) : Layer(prev1->getOutput()->getSize() + prev2->getOutput()->getSize()){
    connect(prev1);
    connect(prev2);
}

void Concat::compute() {
    
    float* in1 = getInput(0)->getValues();
    float* in2 = getInput(1)->getValues();
    
    float* out = getOutput()->getValues();
    
    int in1Size = getInput(0)->getSize();
    int in2Size = getInput(1)->getSize();
    
    memcpy(&out[0], in1, sizeof(float) * in1Size);
    memcpy(&out[in1Size], in2, sizeof(float) * in2Size);
    
}
void Concat::backprop() {
    float* in1 = getInput(0)->getGradient()->getValues();
    float* in2 = getInput(1)->getGradient()->getValues();
    
    float* out = getOutput()->getGradient()->getValues();
    
    int in1Size = getInput(0)->getSize();
    int in2Size = getInput(1)->getSize();
    
    memcpy(in1,&out[0], sizeof(float) * in1Size);
    memcpy(in2, &out[in1Size], sizeof(float) * in2Size);
}
