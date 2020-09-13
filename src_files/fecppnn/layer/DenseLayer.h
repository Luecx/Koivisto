

#ifndef KOIVISTO_DENSELAYER_H
#define KOIVISTO_DENSELAYER_H

#include "Layer.h"

#include <immintrin.h>
#include <iostream>
#include <math.h>

class DenseLayer : public Layer {

    private:
    Data* weights;
    Data* bias;

    public:
    DenseLayer(Layer* previousLayer, int size);

    virtual ~DenseLayer() {
        delete weights;
        delete bias;
    }

    void compute() override;
    void backprop() override;

    Data* getWeights() const;
    void  setWeights(Data* weights);
    Data* getBias() const;
    void  setBias(Data* bias);
};

#endif    // KOIVISTO_DENSELAYER_H
