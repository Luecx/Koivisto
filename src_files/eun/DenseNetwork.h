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
#include "../Board.h"
#include "../Bitboard.h"
#include "nntools.h"



namespace nn {

class DenseNetwork {
        
        
        DenseInput *input;
        DenseLayer **layers;
        DenseOutput *output;
        
        int hiddenCount;
    public:
        DenseNetwork(int *sizes, int count);
        
        virtual ~DenseNetwork();
        
        /**
         * does the computation.
         * compute<true> makes recomputes the entire input which takes about 10x longer
         * @tparam recompute
         * @return
         */
        template<bool recompute>
        float compute() {
            input->compute<recompute>();
            for (int i = 0; i < hiddenCount; i++) {
                layers[i]->compute();
                
            }
            output->compute();
            return output->getOutput();
        }
        
        /**
         * loads the weights from a file
         * @param file
         */
         template<bool binary>
        void load_weights(std::string file){
    
    
            if(binary){
                FILE *outfile = fopen(file.c_str(), "rb");
                fread(input->getBias()->getValue()   ,sizeof(float ),                         input->getOutputSize(), outfile);
                fread(input->getWeights()->getValue(),sizeof(float ), input->getInputSize() * input->getOutputSize(), outfile);
    
                for(int i = 0; i < hiddenCount; i++){
                    fread(layers[i]->getBias()->getValue()   ,sizeof(float ),                             layers[i]->getOutputSize(), outfile);
                    fread(layers[i]->getWeights()->getValue(),sizeof(float ), layers[i]->getInputSize() * layers[i]->getOutputSize(), outfile);
                }
    
                fread(output->getBias()->getValue()   ,sizeof(float ),                          output->getOutputSize(), outfile);
                fread(output->getWeights()->getValue(),sizeof(float ), output->getInputSize() * output->getOutputSize(), outfile);
    
                fclose(outfile);
    
            }else{
                std::ifstream infile(file);
                if (!infile.is_open()) exit(-1);
                int count = 0;
    
                //input first
                for(int j = 0; j < input->getOutputSize(); j++){
                    infile >> input->getBias()->value[j] ;
                    count ++;
                }
                for(int j = 0; j < input->getInputSize() * input->getOutputSize(); j++){
                    infile >> input->getWeights()->value[j] ;
        
                    count ++;
                }
                //hidden
                for(int i = 0; i < hiddenCount; i++){
                    for(int j = 0; j < layers[i]->getOutputSize(); j++){
                        infile >> layers[i]->getBias()->value[j] ;
//            std::cout << layers[i]->getBias()->value[j] << std::endl;
                    }
                    for(int j = 0; j < layers[i]->getInputSize() * layers[i]->getOutputSize(); j++){
                        infile >> layers[i]->getWeights()->value[j] ;
            
                    }
                }
    
                //output layer
                for(int j = 0; j < output->getOutputSize(); j++){
                    infile >> output->getBias()->value[0];
                }
                for(int j = 0; j < output->getInputSize() * output->getOutputSize(); j++){
                    infile >> output->getWeights()->value[j];
                }
    
                infile.close();
            }
            
            
        }
        
        /**
         * writes the weights to a file
         * @param file
         */
        template<bool binary>
        void write_weights(std::string file){
    
            
            if(binary){
    
                FILE *outfile = fopen(file.c_str(), "wb");
                fwrite(input->getBias()->getValue()   ,sizeof(float ),                         input->getOutputSize(), outfile);
                fwrite(input->getWeights()->getValue(),sizeof(float ), input->getInputSize() * input->getOutputSize(), outfile);
    
                for(int i = 0; i < hiddenCount; i++){
                    fwrite(layers[i]->getBias()->getValue()   ,sizeof(float ),                             layers[i]->getOutputSize(), outfile);
                    fwrite(layers[i]->getWeights()->getValue(),sizeof(float ), layers[i]->getInputSize() * layers[i]->getOutputSize(), outfile);
                }
    
                fwrite(output->getBias()->getValue()   ,sizeof(float ),                          output->getOutputSize(), outfile);
                fwrite(output->getWeights()->getValue(),sizeof(float ), output->getInputSize() * output->getOutputSize(), outfile);
                
                fclose(outfile);
                
            }else{
    
                std::ofstream outfile(file);
                if (!outfile.is_open()) exit(-1);
                
                //input first
                for(int j = 0; j < input->getOutputSize(); j++){
                    outfile << input->getBias()->value[j] << std::endl;
                }
                for(int j = 0; j < input->getInputSize() * input->getOutputSize(); j++){
                    outfile << input->getWeights()->value[j] << std::endl;
                }
    
                //hidden
                for(int i = 0; i < hiddenCount; i++){
                    for(int j = 0; j < layers[i]->getOutputSize(); j++){
                        outfile << layers[i]->getBias()->value[j] << std::endl;
                    }
                    for(int j = 0; j < layers[i]->getInputSize() * layers[i]->getOutputSize(); j++){
                        outfile << layers[i]->getWeights()->value[j] << std::endl;
                    }
                }
    
                //output layer
                for(int j = 0; j < output->getOutputSize(); j++){
                    outfile << output->getBias()->value[0] << std::endl;
                }
                for(int j = 0; j < output->getInputSize() * output->getOutputSize(); j++){
                    outfile << output->getWeights()->value[j] << std::endl;
                }
                outfile.close();
            }
            
            
            
    
        }
        
        
        /**
         * resets the network input to the given board.
         * this is the standard way of setting the input
         */
        void resetNetworkInput(Board *board);
        
        /**
         * validate the network input and checks if all the entries are set correct
         * for debugging purpose
         */
        bool validateNetworkInput(Board *board);
        
        /**
         * the amount of hidden layers (not including output)
         * @return
         */
        int getHiddenCount() const;
        
        /**
         * returns the hidden layer at the given index
         * @param index
         * @return
         */
        DenseLayer *getHiddenLayer(int index);
        
        /**
         * returns the input layer
         * @return
         */
        DenseInput *getInputLayer();
        
        /**
         * returns the output layer
         * @return
         */
        DenseOutput *getOutputLayer() const;
        
        /**
         * returns the input float array
         * @return
         */
        float *getInput();
        
        /**
         * returns the input value at the given index
         * @param index
         * @return
         */
        float getInput(int index);
        
        /**
         * changes one value in the input.
         * for setting entire board, consider using resetNetworkInput()
         */
        void adjustInput(int index, float newValue);
        
        /**
         * clears the input
         */
        void clearInput();
        
         /**
          * copies the weights
          * @param other
          */
         void copyWeights(DenseNetwork* other);
        
        /**
         * use the same weights array as the other network
         * @param other
         */
        void setWeights(DenseNetwork* other);
        
        /**
         * creates an exact copy
         */
        DenseNetwork* copy();
        
        /**
         *  creates a copy but keeps weights and bias
         */
        DenseNetwork* shallowCopy();
    
};




}





#endif //KOIVISTO_DENSENETWORK_H
