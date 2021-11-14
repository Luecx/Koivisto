
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                                   by. Kim Kahre and Finn Eggers                                  *
 *                                                                                                  *
 *                 Koivisto is free software: you can redistribute it and/or modify                 *
 *               it under the terms of the GNU General Public License as published by               *
 *                 the Free Software Foundation, either version 3 of the License, or                *
 *                                (at your option) any later version.                               *
 *                    Koivisto is distributed in the hope that it will be useful,                   *
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 *                           GNU General Public License for more details.                           *
 *                 You should have received a copy of the GNU General Public License                *
 *                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
 *                                                                                                  *
 ****************************************************************************************************/
#include "newmovegen.h"

void moveGen::init(SearchData* sd, Board* b, Depth ply, Move hashMove, int mode) {
    m_sd            = sd;
    m_board         = b;
    m_ply           = ply;
    m_hashMove      = hashMove;
    m_mode          = mode;
    stage           = GET_HASHMOVE;
    quietSize       = 0;
    noisySize       = 0;
    goodNoisyCount  = 0;
    noisy_index     = 0;
    quiet_index     = 0;
    searched_index  = 0;
}

Move moveGen::next() {
    switch (stage) {
        case GET_HASHMOVE:
            stage++;
            if (m_hashMove)
                return m_hashMove;

        case GEN_NOISY:
            generateNoisy();
            stage++;

        case GET_GOOD_NOISY:
            if (noisy_index < goodNoisyCount) 
                return nextNoisy();
            stage++;

        case GEN_QUIET:
            generateQuiet();
            stage++;

        case GET_QUIET:
            if (quiet_index < quietSize)
                return nextQuiet();
            stage++;

        case GET_BAD_NOISY:
            if (noisy_index < noisySize) 
                return nextNoisy();
            stage++;

        case END:
            return 0;
    }
}

void moveGen::addNoisy(Move m) {
    if (sameMove(m_hashMove, m))
        return;
    noisy[noisySize] = m;
    noisyScores[noisySize++] = 1;
    goodNoisyCount++;
}

void moveGen::addQuiet(Move m) {
    if (sameMove(m_hashMove, m))
        return;
    quiets[quietSize] = m;
    quietScores[quietSize++] = 1;
}

Move moveGen::nextNoisy() {
    int bestNoisy = noisy_index;
    for (int i = noisy_index + 1; i < noisySize; i++) {
        if (noisyScores[i] > noisyScores[bestNoisy])
            bestNoisy = i;
    }
    Move m = noisy[bestNoisy];
    noisyScores[bestNoisy]  = noisyScores[noisy_index];
    noisy[bestNoisy]        = noisy[noisy_index++];
    return m;
}

Move moveGen::nextQuiet() {
    int bestQuiet = quiet_index;
    for (int i = quiet_index + 1; i < quietSize; i++) {
        if (quietScores[i] > quietScores[bestQuiet])
            bestQuiet = i;
    }
    Move m = quiets[bestQuiet];
    quietScores[bestQuiet]  = quietScores[quiet_index];
    quiets[bestQuiet]       = quiets[quiet_index++];
    return m;
}

void moveGen::addSearched(Move m) {
    searched[searched_index++] = m;
}

void moveGen::generateNoisy() {

    const Color c = m_board->getActivePlayer();
    
    const U64 relative_rank_8_bb = c == WHITE ? RANK_8_BB : RANK_1_BB;
    const U64 relative_rank_7_bb = c == WHITE ? RANK_7_BB : RANK_2_BB;
    const U64 relative_rank_4_bb = c == WHITE ? RANK_4_BB : RANK_5_BB;
    
    const Direction forward      = c == WHITE ? NORTH:SOUTH;
    const Direction right        = c == WHITE ? NORTH_EAST:SOUTH_EAST;
    const Direction left         = c == WHITE ? NORTH_WEST:SOUTH_WEST;
    
    const U64 opponents         = m_board->getTeamOccupiedBB(!c);
    
    const U64 pawns             = m_board->getPieceBB(c, PAWN);
    const U64 occupied          = m_board->getOccupiedBB();
    
    const U64 pawnsLeft   =  c == WHITE ? shiftNorthWest(pawns) : shiftSouthWest(pawns);
    const U64 pawnsRight  =  c == WHITE ? shiftNorthEast(pawns) : shiftSouthEast(pawns);
    const U64 pawnsCenter = (c == WHITE ? shiftNorth (pawns) : shiftSouth(pawns)) & ~occupied;
    
    const Piece movingPiece = c * 8 + PAWN;
    
    U64 nonPromoAttacks = opponents & ~relative_rank_8_bb;
    Square target;
    
    U64 attacks = pawnsLeft & nonPromoAttacks;
    while (attacks) {
        target = bitscanForward(attacks);
        addNoisy(genMove(target - left, target, CAPTURE, movingPiece, m_board->getPiece(target)));
        attacks = lsbReset(attacks);
    }
    
    attacks = pawnsRight & nonPromoAttacks;
    while (attacks) {
        target = bitscanForward(attacks);
        addNoisy(genMove(target - right, target, CAPTURE, movingPiece, m_board->getPiece(target)));
        attacks = lsbReset(attacks);
    }
    
    if (pawnsLeft & m_board->getBoardStatus()->enPassantTarget) {
        target = m_board->getEnPassantSquare();
        addNoisy(genMove(target - left, target, EN_PASSANT, movingPiece));
    }
    
    if (pawnsRight & m_board->getBoardStatus()->enPassantTarget) {
        target = m_board->getEnPassantSquare();
        addNoisy(genMove(target - right, target, EN_PASSANT, movingPiece));
    }
 
    if (pawns & relative_rank_7_bb) {
        
        attacks = pawnsCenter & relative_rank_8_bb;
        while (attacks) {
            target = bitscanForward(attacks);
            addNoisy(genMove(target - forward, target, QUEEN_PROMOTION, movingPiece));
            attacks = lsbReset(attacks);
        }
        
        attacks = pawnsLeft & relative_rank_8_bb & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            addNoisy(genMove(target - left, target, QUEEN_PROMOTION_CAPTURE , movingPiece, m_board->getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & relative_rank_8_bb & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            addNoisy(genMove(target - right, target, QUEEN_PROMOTION_CAPTURE , movingPiece, m_board->getPiece(target)));
            attacks = lsbReset(attacks);
        }
    }
}

void moveGen::generateQuiet() {
        
    const Color c = m_board->getActivePlayer();
    
    const U64 relative_rank_8_bb = c == WHITE ? RANK_8_BB : RANK_1_BB;
    const U64 relative_rank_7_bb = c == WHITE ? RANK_7_BB : RANK_2_BB;
    const U64 relative_rank_4_bb = c == WHITE ? RANK_4_BB : RANK_5_BB;
    
    const Direction forward      = c == WHITE ? NORTH:SOUTH;
    const Direction right        = c == WHITE ? NORTH_EAST:SOUTH_EAST;
    const Direction left         = c == WHITE ? NORTH_WEST:SOUTH_WEST;
    
    const U64 opponents         = m_board->getTeamOccupiedBB(!c);
    
    const U64 pawns             = m_board->getPieceBB(c, PAWN);
    const U64 occupied          = m_board->getOccupiedBB();
    
    const U64 pawnsLeft   =  c == WHITE ? shiftNorthWest(pawns) : shiftSouthWest(pawns);
    const U64 pawnsRight  =  c == WHITE ? shiftNorthEast(pawns) : shiftSouthEast(pawns);
    const U64 pawnsCenter = (c == WHITE ? shiftNorth (pawns) : shiftSouth(pawns)) & ~occupied;
    
    const Piece movingPiece = c * 8 + PAWN;

    Square target;

    
    U64 pawnPushes = pawnsCenter & ~relative_rank_8_bb;
    U64 attacks = pawnPushes;
    while (attacks) {
        target = bitscanForward(attacks);
        addQuiet(genMove(target - forward, target, QUIET, movingPiece));
        attacks = lsbReset(attacks);
    }
        
    attacks = (c == WHITE ? shiftNorth(pawnPushes) : shiftSouth(pawnPushes)) & relative_rank_4_bb & ~occupied;
    while (attacks) {
        target = bitscanForward(attacks);
        addQuiet(genMove(target - forward * 2, target, DOUBLED_PAWN_PUSH, movingPiece));
        attacks = lsbReset(attacks);
    }
}

void moveGen::updateHistory(int weight, Move previous, Move followup) {

    weight          = weight * weight + 5 * weight;
    Move bestMove   = searched[searched_index];    
    Color side      = m_board->getActivePlayer();

    if (isCapture(bestMove)) {
        m_sd->captureHistory[side][getSqToSqFromCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->captureHistory[side][getSqToSqFromCombination(bestMove)]
                    / MAX_HIST;

        for (int i = 0; i < searched_index - 1; i++) {
            Move m = searched[i];
            if (isCapture(m)) {
                    m_sd->captureHistory[side][getSqToSqFromCombination(m)] +=
                                - weight
                                - weight * m_sd->captureHistory[side][getSqToSqFromCombination(m)]
                                / MAX_HIST;
            }
        } 
    } else {
        m_sd->history[side][getSqToSqFromCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->history[side][getSqToSqFromCombination(bestMove)]
                    / MAX_HIST;
        m_sd->cmh[getPieceTypeSqToCombination(previous)][side][getPieceTypeSqToCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->cmh[getPieceTypeSqToCombination(previous)][side][getPieceTypeSqToCombination(bestMove)]
                    / MAX_HIST;
        m_sd->fmh[getPieceTypeSqToCombination(followup)][side][getPieceTypeSqToCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->fmh[getPieceTypeSqToCombination(followup)][side][getPieceTypeSqToCombination(bestMove)]
                    / MAX_HIST;
        for (int i = 0; i < searched_index - 1; i++) {
            Move m = searched[i];
            if (isCapture(m)) {
                m_sd->captureHistory[side][getSqToSqFromCombination(m)] +=
                            - weight
                            - weight * m_sd->captureHistory[side][getSqToSqFromCombination(m)]
                            / MAX_HIST;
            } else {
                m_sd->history[side][getSqToSqFromCombination(bestMove)] +=
                            - weight
                            - weight * m_sd->history[side][getSqToSqFromCombination(m)]
                            / MAX_HIST;
                m_sd->cmh[getPieceTypeSqToCombination(previous)][side][getPieceTypeSqToCombination(m)] +=
                            - weight
                            - weight * m_sd->cmh[getPieceTypeSqToCombination(previous)][side][getPieceTypeSqToCombination(m)]
                            / MAX_HIST;
                m_sd->fmh[getPieceTypeSqToCombination(followup)][side][getPieceTypeSqToCombination(m)] +=
                            - weight
                            - weight * m_sd->fmh[getPieceTypeSqToCombination(followup)][side][getPieceTypeSqToCombination(m)]
                            / MAX_HIST;
            }
        } 
    }
}