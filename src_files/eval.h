//
// Created by finne on 5/31/2020.
//

#ifndef KOIVISTO_EVAL_H
#define KOIVISTO_EVAL_H

#include "Bitboard.h"
#include "Board.h"

static int unusedVariable = 0;

static int INDEX_PAWN_VALUE                = unusedVariable++;
static int INDEX_PAWN_PSQT                 = unusedVariable++;
static int INDEX_PAWN_STRUCTURE            = unusedVariable++;
static int INDEX_PAWN_PASSED               = unusedVariable++;
static int INDEX_PAWN_ISOLATED             = unusedVariable++;
static int INDEX_PAWN_DOUBLED              = unusedVariable++;
static int INDEX_PAWN_DOUBLED_AND_ISOLATED = unusedVariable++;
static int INDEX_PAWN_BACKWARD             = unusedVariable++;
static int INDEX_PAWN_OPEN                 = unusedVariable++;

static int INDEX_KNIGHT_VALUE    = unusedVariable++;
static int INDEX_KNIGHT_PSQT     = unusedVariable++;
static int INDEX_KNIGHT_MOBILITY = unusedVariable++;
static int INDEX_KNIGHT_OUTPOST  = unusedVariable++;

static int INDEX_BISHOP_VALUE            = unusedVariable++;
static int INDEX_BISHOP_PSQT             = unusedVariable++;
static int INDEX_BISHOP_MOBILITY         = unusedVariable++;
static int INDEX_BISHOP_DOUBLED          = unusedVariable++;
static int INDEX_BISHOP_PAWN_SAME_SQUARE = unusedVariable++;
static int INDEX_BISHOP_FIANCHETTO       = unusedVariable++;

static int INDEX_ROOK_VALUE          = unusedVariable++;
static int INDEX_ROOK_PSQT           = unusedVariable++;
static int INDEX_ROOK_MOBILITY       = unusedVariable++;
static int INDEX_ROOK_OPEN_FILE      = unusedVariable++;
static int INDEX_ROOK_HALF_OPEN_FILE = unusedVariable++;
static int INDEX_ROOK_KING_LINE      = unusedVariable++;

static int INDEX_QUEEN_VALUE    = unusedVariable++;
static int INDEX_QUEEN_PSQT     = unusedVariable++;
static int INDEX_QUEEN_MOBILITY = unusedVariable++;

static int INDEX_KING_SAFETY         = unusedVariable++;
static int INDEX_KING_PSQT           = unusedVariable++;
static int INDEX_KING_CLOSE_OPPONENT = unusedVariable++;
static int INDEX_KING_PAWN_SHIELD    = unusedVariable++;

static int INDEX_KNIGHT_DISTANCE_ENEMY_KING = unusedVariable++;
static int INDEX_QUEEN_DISTANCE_ENEMY_KING  = unusedVariable++;

static int INDEX_PINNED_PAWN_BY_BISHOP   = unusedVariable++;
static int INDEX_PINNED_PAWN_BY_ROOK     = unusedVariable++;
static int INDEX_PINNED_PAWN_BY_QUEEN    = unusedVariable++;
static int INDEX_PINNED_KNIGHT_BY_BISHOP = unusedVariable++;
static int INDEX_PINNED_KNIGHT_BY_ROOK   = unusedVariable++;
static int INDEX_PINNED_KNIGHT_BY_QUEEN  = unusedVariable++;
static int INDEX_PINNED_BISHOP_BY_BISHOP = unusedVariable++;
static int INDEX_PINNED_BISHOP_BY_ROOK   = unusedVariable++;
static int INDEX_PINNED_BISHOP_BY_QUEEN  = unusedVariable++;
static int INDEX_PINNED_ROOK_BY_BISHOP   = unusedVariable++;
static int INDEX_PINNED_ROOK_BY_ROOK     = unusedVariable++;
static int INDEX_PINNED_ROOK_BY_QUEEN    = unusedVariable++;
static int INDEX_PINNED_QUEEN_BY_BISHOP  = unusedVariable++;
static int INDEX_PINNED_QUEEN_BY_ROOK    = unusedVariable++;
static int INDEX_PINNED_QUEEN_BY_QUEEN   = unusedVariable++;

static int INDEX_PAWN_HANGING   = unusedVariable++;
static int INDEX_KNIGHT_HANGING = unusedVariable++;
static int INDEX_BISHOP_HANGING = unusedVariable++;
static int INDEX_ROOK_HANGING   = unusedVariable++;
static int INDEX_QUEEN_HANGING  = unusedVariable++;

static int SPACER1 = unusedVariable += unusedVariable % 4 == 0 ? 0 : (4 - unusedVariable % 4);

class Evaluator {
    public:
    float* features = new float[unusedVariable];

    float phase;

    void computePinnedPieces(Board* b);

    void computeHangingPieces(Board* b);

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
