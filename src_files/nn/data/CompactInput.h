//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_COMPACTINPUT_H
#define KOIVISTO_COMPACTINPUT_H


#include <string>
#include <vector>

struct CompactInputEntry{
    
    int index;
    float value;
    
};

struct CompactTrainEntry{
    
    std::vector<struct CompactInputEntry> * input;
    float output;
    
    CompactTrainEntry(std::vector<CompactInputEntry> *input, float output);

};
/**
 * returns a compact train entry which contains a sparse vector for the inputs and the target output
 * @param fen
 * @return
 */
CompactTrainEntry * trainDataFromFen(std::string fen);

/**
 * generates a vector of compact train entries from a file
 * the file is read line by line where each line contains an fen and a score seperated by a semicolon (;)
 * @param file
 * @return
 */
std::vector<CompactTrainEntry*>* trainDataFromFile(std::string file, int max_count=1e9, std::vector<CompactTrainEntry*>* vec= nullptr);




#endif //KOIVISTO_COMPACTINPUT_H
