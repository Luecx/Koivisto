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
        
        template<bool recompute>
        float compute() {
            input->compute<recompute>();

//            printArray(input->getSums(), input->getOutputSize());
//    printArray(input->getBias(), input->getOutputSize());
//    printArray(input->getOutput(), input->getOutputSize());
            for(int i = 0; i < hiddenCount; i++){
                layers[i]->compute();
//        printArray(layers[i]->getOutput(), layers[i]->getOutputSize());
//        printArray(layers[i]->getBias(), layers[i]->getOutputSize());
            
            }
            
            
            output->compute();
            return output->getOutput();
        }
        float backprop(float expectedOutput, float eta){
            float d = output->backprop(layers[hiddenCount-1]->getErrorSignal(), eta, expectedOutput);
            for(int i = hiddenCount-1; i > 0; i--){
                layers[i]->backprop(layers[i-1]->getErrorSignal(), eta);
            }
            layers[0]->backprop(input->getErrorSignal(), eta);
            input->backprop(eta);
            return d * d * 0.5;
        }
        float train(float expectedOutput, float eta){
            compute<true>();
            return backprop(expectedOutput, eta);
        }


        void load_weights(std::string file){
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
        void write_weights(std::string file) {
            
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
        
        void adjustInput(int index, float newValue);
        void clearInput();
        
};







#endif //KOIVISTO_DENSENETWORK_H
