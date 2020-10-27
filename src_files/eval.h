//
// Created by finne on 5/31/2020.
//

#ifndef KOIVISTO_EVAL_H
#define KOIVISTO_EVAL_H

#include "Bitboard.h"
#include "Board.h"

namespace eval {

#define pst_index_white_s(s)                         squareIndex(7 - rankIndex(s), fileIndex(s))
#define pst_index_black_s(s)                         s
#define pst_index_white(i, kside)                    squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_black(i, kside)                    squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define psqt_kingside_indexing(wkingside, bkingside) (wkingside) * 2 + (bkingside)

typedef int32_t EvalScore;
#define M(mg, eg)    ((EvalScore)((unsigned int) (eg) << 16) + (mg))
#define MgScore(s)   ((Score)((uint16_t)((unsigned) ((s)))))
#define EgScore(s)   ((Score)((uint16_t)((unsigned) ((s) + 0x8000) >> 16)))
#define showScore(s) std::cout << "(" << MgScore(s) << ", " << EgScore(s) << ")" << std::endl;

extern EvalScore* psqt[11];
extern EvalScore  pieceScores[6];
extern EvalScore* evfeatures[];
extern EvalScore  hangingEval[5];
extern EvalScore  pinnedEval[15];
extern EvalScore* mobilities[6];
extern int        mobEntryCount[6];

void eval_init();


EvalScore computePinnedPieces(Board* b);

EvalScore computeHangingPieces(Board* b);

float phase(Board* b);

bb::Score evaluate(Board* b);

void printEvaluation(Board* b);

}

#endif    // KOIVISTO_EVAL_H
