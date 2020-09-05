//
// Created by finne on 7/22/2020.
//

#include "DenseNetwork.h"

/**
 * assumes that the last node is always size = 1
 * -> put in: 128,64,32, count=3
 * -> hiddenLayerCount = 2
 */
nn::DenseNetwork::DenseNetwork(int *sizes, int count) {
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



void nn::DenseNetwork::adjustInput(int index, float newValue) {
    input->adjustInput(index, newValue);
}

void nn::DenseNetwork::clearInput() {
    input->clearInput();
}

float* nn::DenseNetwork::getInput() {
    return input->getInput();
}

float nn::DenseNetwork::getInput(int index) {
    return input->getInput()[index];
}




void nn::DenseNetwork::resetNetworkInput(Board *board) {
    clearInput();
    Color activePlayer = board->getActivePlayer();
    Square whiteKing = bitscanForward(board->getPieces()[WHITE_KING]);
    Square blackKing = bitscanForward(board->getPieces()[BLACK_KING]);
    for(Piece p = WHITE_PAWN; p <= BLACK_KING; p++){
        U64 k = board->getPieces()[p];
        while(k){
            Square s = bitscanForward(k);

            if(p / 6 == WHITE){
                adjustInput(whiteInputIndex(activePlayer, p, s, blackKing),1);
            }else{
                adjustInput(blackInputIndex(activePlayer, p, s, whiteKing),1);
            }

            k = lsbReset(k);
        }
    }
//    std::cout << "_-------------------------------_" << std::endl;
}

bool nn::DenseNetwork::validateNetworkInput(Board *board) {
    
    U64 activePlayer = board->getActivePlayer();
    
    Square whiteKing = bitscanForward(board->getPieces()[WHITE_KING]);
    Square blackKing = bitscanForward(board->getPieces()[BLACK_KING]);
    
    
//    std::cerr << bitCount(*board->getOccupied()) << std::endl;
//    std::cerr << getInputLayer()->getNonZeroCount() << std::endl;
    


    if(bitCount(*board->getOccupied()) != getInputLayer()->getNonZeroCount()) return false;
    
    
    InputTracker container = getInputLayer()->getNonZeroIndices();
    
    
//    for(int i = 0; i < container.count(); i++){
//        std::cerr << container.at(i) << std::endl;
//    }
//
//    std::cout << "---"<<std::endl;
    
    for(int i = 0; i < 12; i++){
        
        U64 pBB = board->getPieces()[i];
        
        if(i / 6 == WHITE){
            
            while(pBB){
                Square s = bitscanForward(pBB);
    
                int index = whiteInputIndex(activePlayer, i, s, blackKing);
//                std::cout << index << std::endl;
                if(!container.contains(index)) return false;
                pBB = lsbReset(pBB);
            }
            
        }else{
            while(pBB){
                Square s = bitscanForward(pBB);
                
                int index = blackInputIndex(activePlayer, i, s, whiteKing);
//                std::cout << index << std::endl;
                if(!container.contains(index)) return false;
                pBB = lsbReset(pBB);
            }
        }
        
        
    }
    return true;
}

nn::DenseNetwork *nn::DenseNetwork::copy() {
    int* sizes = new int[hiddenCount+2];
    sizes[0] = getInputLayer()->getInputSize();
    for(int i = 1; i <= hiddenCount; i++){
        sizes[i] = layers[i-1]->getInputSize();
    }
    sizes[hiddenCount+1] = layers[hiddenCount-1]->getOutputSize();
    
    DenseNetwork *net = new DenseNetwork(sizes, hiddenCount+2);
    
    net->copyWeights(this);
    
   
    
    return net;
}

nn::DenseNetwork::~DenseNetwork() {
    delete input;
    for(int i = 0; i < hiddenCount;i ++){
        delete layers[i];
    }
    delete layers;
    delete output;
}

nn::DenseOutput *nn::DenseNetwork::getOutputLayer() const {
    return output;
}

nn::DenseInput *nn::DenseNetwork::getInputLayer() { return input; }

nn::DenseLayer *nn::DenseNetwork::getHiddenLayer(int index) { return layers[index]; }


int nn::DenseNetwork::getHiddenCount() const {
    return hiddenCount;
}

void nn::DenseNetwork::copyWeights(nn::DenseNetwork *other) {
    memcpy(getInputLayer()->getWeights()->value, other->getInputLayer()->getWeights()->value, this->getInputLayer()->getWeights()->size * sizeof(float));
    memcpy(getInputLayer()->getBias()->value, other->getInputLayer()->getBias()->value, this->getInputLayer()->getBias()->size * sizeof(float));
    
    for(int i = 0; i < hiddenCount; i++){
        memcpy(getHiddenLayer(i)->getWeights()->value, other->getHiddenLayer(i)->getWeights()->value, this->getHiddenLayer(i)->getWeights()->size * sizeof(float));
        memcpy(getHiddenLayer(i)->getBias()->value, other->getHiddenLayer(i)->getBias()->value, this->getHiddenLayer(i)->getBias()->size * sizeof(float));
    }
    
    memcpy(getOutputLayer()->getWeights()->value, other->getOutputLayer()->getWeights()->value, this->getOutputLayer()->getWeights()->size * sizeof(float));
    memcpy(getOutputLayer()->getBias()->value, other->getOutputLayer()->getBias()->value, this->getOutputLayer()->getBias()->size * sizeof(float));
}

void nn::DenseNetwork::setWeights(nn::DenseNetwork *other) {
    
    getInputLayer()->setWeights(other->getInputLayer()->getWeights());
    getInputLayer()->setBias(other->getInputLayer()->getBias());
    
    for(int i = 0; i < hiddenCount; i++){
        
        getHiddenLayer(i)->setWeights(other->getHiddenLayer(i)->getWeights());
        getHiddenLayer(i)->setBias(other->getHiddenLayer(i)->getBias());
        
    }
    
    getOutputLayer()->setWeights(other->getOutputLayer()->getWeights());
    getOutputLayer()->setBias(other->getOutputLayer()->getBias());
    
}

nn::DenseNetwork *nn::DenseNetwork::shallowCopy() {
    int* sizes = new int[hiddenCount+2];
    sizes[0] = getInputLayer()->getInputSize();
    for(int i = 1; i <= hiddenCount; i++){
        sizes[i] = layers[i-1]->getInputSize();
    }
    sizes[hiddenCount+1] = layers[hiddenCount-1]->getOutputSize();
    
    DenseNetwork *net = new DenseNetwork(sizes, hiddenCount+2);
    
    net->setWeights(this);
    return net;
}



