

#ifndef KOIVISTO_LOSS_H
#define KOIVISTO_LOSS_H

#include "../data/Data.h"
class Loss {
    
    public:
        virtual float computeLoss(Data* output, Data* expected) = 0;
    
};

#endif    // KOIVISTO_LOSS_H
