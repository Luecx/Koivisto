//
// Created by finne on 9/24/2020.
//

#ifndef KOIVISTO_GENERATOR_H
#define KOIVISTO_GENERATOR_H

#include "../../Board.h"
#include "../../TranspositionTable.h"
#include "../../search.h"

#include <fstream>

namespace generator {

extern TranspositionTable* searchedPosition;
extern std::ofstream*      outFile;
extern Evaluator           evaluator;

typedef struct LINE {
    int cmove=0;              // Number of moves in the line.
    Move argmove[32]{};         // The line.
}   LINE;




[[noreturn]] void generate(const std::string& outpath);

void evaluatePositions(const string& inpath, const string& outpath);

}    // namespace generator

#endif    // KOIVISTO_GENERATOR_H
