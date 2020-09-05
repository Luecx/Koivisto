//
// Created by finne on 7/22/2020.
//

#ifndef KOIVISTO_COMPACTINPUT_H
#define KOIVISTO_COMPACTINPUT_H


#include <string>
#include <vector>
#include <fstream>
#include "../../Board.h"
#include "../../Bitboard.h"
#include "../nntools.h"

struct TrainEntry{
    
    Board* board;
    float output;
    
    TrainEntry(Board *board, float output);
    
    virtual ~TrainEntry();
    
};
/**
 * returns a compact train entry which contains a board and the correct eval
 * @param fen
 * @return
 */
TrainEntry * generateTrainEntry(std::string fen);

/**
 * generates a vector of compact train entries from a file
 * the file is read line by line where each line contains an fen and a score separated by a semicolon (;)
 * @param file
 * @return
 */
std::vector<TrainEntry*>* generateTrainData(std::string file, int max_count= 1e9, std::vector<TrainEntry*>* vec= nullptr);


/**
 * takes a position file + eval and prepares them for python
 * @param inputFile
 * @param outputFile
 */
void writeInSparseFormat(std::string inputFile, std::string outputFile);

/**
 *
 */
void writeInSparseFormat(std::vector<TrainEntry*>*  data, std::string file);

#endif //KOIVISTO_COMPACTINPUT_H
