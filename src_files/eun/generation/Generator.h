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


Move selectRandomMove(MoveList& moveList, double king_walk_p);

Score evalPosition(Board* b);

Score collectAllQuietPositions(Board* b,  std::vector<Board> *leafs, Score alpha=-MAX_MATE_SCORE, Score beta=MAX_MATE_SCORE, Depth ply=0);

[[noreturn]] void generate(const std::string& outpath);

}    // namespace generator

#endif    // KOIVISTO_GENERATOR_H
