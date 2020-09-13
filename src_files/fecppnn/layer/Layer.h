

#ifndef KOIVISTO_LAYER_H
#define KOIVISTO_LAYER_H

#include "../data/Data.h"

#include <iostream>
#include <vector>
class Layer {

    private:
    Data* output;

    std::vector<Layer*> previousLayers {};
    std::vector<Layer*> nextLayers {};

    public:
    virtual void compute()  = 0;
    virtual void backprop() = 0;

    public:
    Layer(int size) { this->output = new Data(size, true); }
    virtual ~Layer() { delete output; }

    Data* getInput(int index) { return previousLayers[index]->getOutput(); }
    Data* getInput() { return previousLayers[0]->getOutput(); }
    Data* getOutput() const { return output; }

    void setOutput(Data* output) { Layer::output = output; }

    void connect(Layer* prevLayer) {
        if (prevLayer->nextLayers.size() != 0) {
            std::cerr << "spreading data to different layers is not important in this version for performance reasons"
                      << std::endl;
            exit(-1);
        }

        previousLayers.push_back(prevLayer);
        prevLayer->nextLayers.push_back(this);
    }
};

#endif    // KOIVISTO_LAYER_H
