//
// Created by finne on 5/31/2020.
//

#ifndef KOIVISTO_EVAL_H
#define KOIVISTO_EVAL_H

#include "Bitboard.h"
#include "Board.h"

#define pst_index_white(r, f) squareIndex(7 - r, f)
#define pst_index_white_s(s)  squareIndex(7 - rankIndex(s), fileIndex(s))
#define pst_index_black(r, f) squareIndex(r, f)
#define pst_index_black_s(s)  s

typedef int32_t EvalScore;
#define M(mg, eg)    ((EvalScore)((unsigned int) (eg) << 16) + (mg))
#define MgScore(s)   ((Score)((uint16_t)((unsigned) ((s)))))
#define EgScore(s)   ((Score)((uint16_t)((unsigned) ((s) + 0x8000) >> 16)))
#define showScore(s) std::cout << "(" << MgScore(s) << ", " << EgScore(s) << ")" << std::endl;

extern EvalScore* psqt[6];

extern EvalScore pieceScores[6];

extern EvalScore* features[];
extern EvalScore hangingEval[5];

extern EvalScore pinnedEval[15];

extern EvalScore* mobilities[6];

extern int mobEntryCount[6];

static int unusedVariable = 0;

static int INDEX_KING_SAFETY         = unusedVariable++;
static int INDEX_PASSER_RANK = unusedVariable++;


void eval_init();

class Evaluator {
    public:
//    float features[6];

    float phase;

    EvalScore computePinnedPieces(Board* b);
        
    EvalScore computeHangingPieces(Board* b);

    bb::Score evaluate(Board* b);

    /**
     * returns the phase of the last calculation
     * @return
     */
    float getPhase();

    /**
     * returns a list of features of the last calculation
     * @return
     */
    float* getFeatures();

    /**
     * returns a list of early game parameters
     * @return
     */
    float* getEarlyGameParams();

    /**
     * returns a list of late game parameters
     */
    float* getLateGameParams();

    float* getPSQT(Piece piece, bool early);

    /**
     * returns the amount of tunable parameters
     */
    int paramCount();

    float* getPhaseValues();

#ifdef TUNE_PST
    float* getTunablePST_MG();
    float* getTunablePST_EG();
    float* getTunablePST_MG_grad();
    float* getTunablePST_EG_grad();
#endif
};

void printEvaluation(Board* b);

#endif    // KOIVISTO_EVAL_H
