//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_COMPACTINPUT_H
#define KOIVISTO_COMPACTINPUT_H


#include <string>

struct CompactInputEntry{
    
    int index;
    float value;
    
};

struct CompactTrainEntry{
    
    std::vector<CompactInputEntry> * input;
    float output;
    
    CompactTrainEntry(std::vector<CompactInputEntry> *input, float output);
    
};

std::vector<CompactInputEntry> * fromFen(std::string fen);


#endif //KOIVISTO_COMPACTINPUT_H
