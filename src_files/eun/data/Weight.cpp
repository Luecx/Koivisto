//
// Created by finne on 7/23/2020.
//

#include <cstring>
#include <new>
#include <iostream>
#include "Weight.h"
#include "math.h"

float *Weight::getValue() const {
    return value;
}

float *Weight::getGradient() const {
    return gradient;
}

int Weight::getSize() const {
    return size;
}

void Weight::setValue(float *value) {
    Weight::value = value;
}

void Weight::setGradient(float *gradient) {
    Weight::gradient = gradient;
}

void Weight::setSize(int size) {
    Weight::size = size;
}

Weight::Weight(int size) : size(size) {
    this->value = new (std::align_val_t(32))float[size]{};
    this->gradient = new (std::align_val_t(32))float[size]{};
}

Weight::~Weight() {
    
//    delete[] (std::align_val_t(32)) this->value;
    
//    if(this->value != nullptr){
//        delete this->value;
//        this->value = nullptr;
//    }
//
//    if(this->gradient != nullptr){
//        delete this->gradient;
//        this->gradient = nullptr;
//    }
//
//    if(this->m != nullptr){
//        delete this->m;
//        this->m = nullptr;
//    }
//
//    if(this->v != nullptr){
//        delete this->v;
//        this->v = nullptr;
//    }
    
}

void Weight::resetGradient() {
    std::memset(this->gradient, 0, sizeof(float)*size);
}

