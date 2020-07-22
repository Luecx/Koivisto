//
// Created by finne on 7/22/2020.
//

#include "DenseNetwork.h"

/**
 * assumes that the last node is always size = 1
 * -> put in: 128,64,32, count=3
 * -> hiddenLayerCount = 2
 */
DenseNetwork::DenseNetwork(int *sizes, int count) {
    input = new DenseInput(sizes[0], sizes[1]);
    output = new DenseOutput(sizes[count-1]);
    
    
    hiddenCount = count-2;
    layers = new DenseLayer*[hiddenCount];
    
    
    for(int i = 1; i < count-1; i++){
        
    
        layers[i-1] = new DenseLayer(sizes[i], sizes[i+1]);
        if(i == 1){
            layers[i-1]->setInput(input->getOutput());
        }else{
            layers[i-1]->setInput(layers[i-2]->getOutput());
        }
    }
    
    
    if(hiddenCount == 0){
    
        output->setInput(input->getOutput());
    }else{
    
        output->setInput(layers[hiddenCount-1]->getOutput());
    }
}



void DenseNetwork::adjustInput(int index, float newValue) {
    input->adjustInput(index, newValue);
}

void DenseNetwork::clearInput() {
    memset(input->getSums(),0, input->getOutputSize()*sizeof(float));
    memset(input->getInput(),0, input->getInputSize()*sizeof(float));
}



