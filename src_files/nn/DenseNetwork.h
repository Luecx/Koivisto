//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_DENSENETWORK_H
#define KOIVISTO_DENSENETWORK_H


#include <fstream>
#include "DenseOutput.h"
#include "DenseLayer.h"
#include "DenseInput.h"
#include "../Util.h"

class DenseNetwork {
        
        
        DenseInput *input;
        DenseLayer **layers;
        DenseOutput *output;
        
        int hiddenCount;
    public:
        DenseNetwork(int *sizes, int count);
        
        /**
         * does the computation.
         * compute<true> makes recomputes the entire input which takes about 10x longer
         * @tparam recompute
         * @return
         */
        template<bool recompute>
        float compute() {
            input->compute<recompute>();
            for(int i = 0; i < hiddenCount; i++){
                layers[i]->compute();
            
            }
            output->compute();
            return output->getOutput();
        }
        
        /**
         * trains the data which has already been set onto the given expected output and a learning rate
         * @tparam recompute
         * @param expectedOutput
         * @param eta
         * @return
         */
        template<bool recompute>
        float train(float expectedOutput, float eta){
            compute<recompute>();
            return backprop(expectedOutput, eta);
        }
        
        
        /**
         * trains the network
         * this only runs over all the entries and feeds them into the network once and trains the network.
         */
        float train(std::vector<CompactTrainEntry*>* data, float eta);
        
        
        /**
         * backpropagates the error and updates the weights
         * @param expectedOutput
         * @param eta
         * @return
         */
        float backprop(float expectedOutput, float eta);
        
        /**
         * loads the weights from a file
         * @param file
         */
        void load_weights(std::string file);
        
        /**
         * writes the weights to a file
         * @param file
         */
        void write_weights(std::string file);
        
        
        DenseLayer* getHiddenLayer(int index){return layers[index];}
        DenseInput* getInputLayer(){return input;}
        float *getInput();
        float getInput(int index);
        void adjustInput(int index, float newValue);
        void clearInput();
        void setInput(CompactTrainEntry* trainEntry);
        
};







#endif //KOIVISTO_DENSENETWORK_H
