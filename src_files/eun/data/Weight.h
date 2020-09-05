//
// Created by finne on 7/23/2020.
//

#ifndef KOIVISTO_WEIGHT_H
#define KOIVISTO_WEIGHT_H

struct Weight{
    
    float* value;
    float* gradient;
    
    int size;
    
    Weight(int size);
    
    virtual ~Weight();
    
    void setValue(float *value);
    
    void setGradient(float *gradient);
    
    void resetGradient();
    
    void setSize(int size);
    
    float *getValue() const;
    
    float *getGradient() const;
    
    int getSize() const;
    
 
    
};

#endif //KOIVISTO_WEIGHT_H
