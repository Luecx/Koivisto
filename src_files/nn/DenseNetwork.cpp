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



float* DenseNetwork::getInput() {
    return input->getInput();
}

float DenseNetwork::getInput(int index) {
    return input->getInput()[index];
}

float DenseNetwork::backprop(float expectedOutput, float eta) {
    float d = output->backprop(layers[hiddenCount-1]->getErrorSignal(), eta, expectedOutput);
    if(eta == 0) {
        return d * d * 0.5;
    }
    for(int i = hiddenCount-1; i > 0; i--){
        layers[i]->backprop(layers[i-1]->getErrorSignal(), eta);
    }
    layers[0]->backprop(input->getErrorSignal(), eta);
    input->backprop(eta);
    return d * d * 0.5;
}

void DenseNetwork::write_weights(std::string file) {
    
    std::ofstream outfile(file);
    if (!outfile.is_open()) exit(-1);
    
    //input first
    for(int j = 0; j < input->getOutputSize(); j++){
        outfile << input->getBias()[j] << std::endl;
    }
    for(int j = 0; j < input->getInputSize() * input->getOutputSize(); j++){
        outfile << input->getWeights()[j] << std::endl;
    }
    
    //hidden
    for(int i = 0; i < hiddenCount; i++){
        for(int j = 0; j < layers[i]->getOutputSize(); j++){
            outfile << layers[i]->getBias()[j] << std::endl;
        }
        for(int j = 0; j < layers[i]->getInputSize() * layers[i]->getOutputSize(); j++){
            outfile << layers[i]->getWeights()[j] << std::endl;
        }
    }
    
    //output layer
    for(int j = 0; j < output->getOutputSize(); j++){
        outfile << output->getBias() << std::endl;
    }
    for(int j = 0; j < output->getInputSize() * output->getOutputSize(); j++){
        outfile << output->getWeights()[j] << std::endl;
    }
    
    outfile.close();
    
}

void DenseNetwork::load_weights(std::string file) {
    std::ifstream infile(file);
    if (!infile.is_open()) exit(-1);
    
    
    
    //input first
    for(int j = 0; j < input->getOutputSize(); j++){
        infile >> input->getBias()[j] ;
    }
    for(int j = 0; j < input->getInputSize() * input->getOutputSize(); j++){
        infile >> input->getWeights()[j] ;
    }
    
    //hidden
    for(int i = 0; i < hiddenCount; i++){
        for(int j = 0; j < layers[i]->getOutputSize(); j++){
            infile >> layers[i]->getBias()[j] ;
        }
        for(int j = 0; j < layers[i]->getInputSize() * layers[i]->getOutputSize(); j++){
            infile >> layers[i]->getWeights()[j] ;
        }
    }
    
    //output layer
    for(int j = 0; j < output->getOutputSize(); j++){
        float biasOut;
        infile >> biasOut;
        output->setBias(biasOut) ;
    }
    for(int j = 0; j < output->getInputSize() * output->getOutputSize(); j++){
        infile >> output->getWeights()[j] ;
    }
    
    infile.close();
}


void DenseNetwork::setInput(CompactTrainEntry* trainEntry) {
    clearInput();
    for(int i = 0; i < trainEntry->input->size(); i++){
        adjustInput(trainEntry->input->at(i).index, trainEntry->input->at(i).value);
    }
    input->setLastInput(trainEntry);
}

float DenseNetwork::train(std::vector<CompactTrainEntry *> *data, float eta) {
    
    float error = 0;
    for(int i = 0; i <data->size(); i++){
        
        if(i % 100000 == 0){
            loadingBar(i, data->size(), "progress");
        }
        
        setInput( data->at(i));
        error += train<false>(data->at(i)->output, eta);
    }
    
    std::cout << std::endl;
    
    return error/data->size();
}




