

#ifndef KOIVISTO_OPTIMISER_H
#define KOIVISTO_OPTIMISER_H

#include "../data/Data.h"

#include <vector>
class Optimiser {

    public:
    virtual void init(std::vector<Data*> &weights) = 0;

    virtual void update() = 0;
};

#endif    // KOIVISTO_OPTIMISER_H
