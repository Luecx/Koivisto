

#ifndef KOIVISTO_MSE_H
#define KOIVISTO_MSE_H

#include "Loss.h"
class MSE : public Loss {
    public:
    float computeLoss(Data* output, Data* expected) override {

        float loss = 0;

        for (int i = 0; i < output->getSize(); i++) {
            float difference = output->getValues()[i] - expected->getValues()[i];

            output->getGradient()->getValues()[i] = 2 * difference;
            loss += difference * difference;
        }

        return loss;
    }
};

#endif    // KOIVISTO_MSE_H
