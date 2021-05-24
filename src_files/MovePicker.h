
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

#ifndef CHESSCOMPUTER_MOVEPICKER_H
#define CHESSCOMPUTER_MOVEPICKER_H

#include "Move.h"
#include "Board.h"
#include "History.h"
#include "psqt.h"

enum STAGE {
    TT_MOVE,
    GENERATE_CAPS,
    GOOD_CAPS,
    GENERATE_QUIETS,
    QUIETS,
    BAD_CAPS,
};

class moveList {

    private:
    
    move::Move ttMove;
    move::Move quiet[128];
    move::Move noisy[32];
    int noisyScore[128];
    int quietScore[128];
    move::Move searchedQuiet[128];
    move::Move searchednoisy[32];
    int countGoodCap;
    int countBadCap;
    int countQuiet;
    int indexCap;
    int indexQuiet;
    STAGE stage;
    Board* b;
    SearchData* sd;
    Depth ply;
    Color c;

    public:

    void init(Board* board, SearchData* searchData, move::Move hashMove, Depth halfMove ) {
        b       = board;
        sd      = searchData;
        ttMove  = hashMove;
        ply     = halfMove;
        c       = b->getActivePlayer();
    };

    bool next(move::Move* m) {
        switch (stage) {
            case TT_MOVE:
                stage = GENERATE_CAPS;
                if (ttMove) {
                    *m = ttMove;
                    return true;
                }
            case GENERATE_CAPS:
                generateCaptures();
                stage = GOOD_CAPS;
            case GOOD_CAPS:
                if (indexCap < countGoodCap) {
                    pickBestCap(m);
                    return true;
                }
            case GENERATE_QUIETS:
                generateQuiets();
                stage = QUIETS;
            case QUIETS:
                if (indexQuiet < countQuiet) {
                    pickBestQuiet(m);
                    return true;
                }
            case BAD_CAPS:
                if (indexCap >= countGoodCap+countBadCap)
                    return false;
                pickBestCap(m);
                return true;
        }
        return false;
    }

    void pickBestCap(move::Move* m) {
        int highestScore    = noisyScore[indexCap];
        int bestIndex       = indexCap;
        for (int i = indexCap + 1; i < countGoodCap + countBadCap; i++) {
            if (noisyScore[i] > highestScore) {
                highestScore    = noisyScore[i];
                bestIndex       = i;
            }
        }
        *m                      = noisy[bestIndex];
        noisy[bestIndex]        = noisy[indexCap];
        noisyScore[bestIndex]   = noisyScore[indexCap];
        indexCap++; 
    }

    void pickBestQuiet(move::Move* m) {
        int highestScore    = quietScore[indexQuiet];
        int bestIndex       = indexQuiet;
        for (int i = indexQuiet + 1; i < countQuiet; i++) {
            if (noisyScore[i] > highestScore) {
                highestScore    = quietScore[i];
                bestIndex       = i;
            }
        }
        *m                      = quiet[bestIndex];
        quiet[bestIndex]        = quiet[indexQuiet];
        quietScore[bestIndex]   = quietScore[indexQuiet];
        indexQuiet++; 
    }

    void generateCaptures() {
        countGoodCap    = 0;
        countBadCap     = 0;
        indexCap        = 0;
        generatePawnNoisy();
        generatePieceNoisy();
        generateKingNoisy();
        indexCap        = 0;
    }

    void generateQuiets() {
        indexQuiet      = 0;
        generatePawnQuiet();
        generatePieceQuiet();
        generateKingQuiet();
        indexQuiet      = 0;
    }


    void addCapture(move::Move m) {
        if (sameMove(m, ttMove)) return;
        noisy[indexCap]     = m;
        Score     SEE       = b->staticExchangeEvaluation(m);
        MoveScore mvvLVA    = MgScore(piece_values[(getCapturedPiece(m) % 8)]);
        if (SEE >= 0) {
            noisyScore[indexCap] = 100000 + mvvLVA + sd->getHistories(m, b->getActivePlayer(), b->getPreviousMove());
            countGoodCap++;
        } else {
            noisyScore[indexCap] = 10000 + sd->getHistories(m, b->getActivePlayer(), b->getPreviousMove());
            countBadCap++;
        }
        indexCap++;
    }
    void addQuiet(move::Move m) {
        if (sameMove(m, ttMove)) return;
        quiet[indexQuiet]     = m;
        if (sd->isKiller(m, ply, c)){
            quietScore[indexQuiet] = 30000 + sd->isKiller(m, ply, c);
        } else{
            quietScore[indexQuiet] = 20000 + sd->getHistories(m, b->getActivePlayer(), b->getPreviousMove());
        }
        indexQuiet++;
    }

    void addNoisyPromotion(move::Move m) {
        noisy[indexCap]         = m;
        noisyScore[indexCap]    = 40000;
        indexCap++;
    }

    void generatePawnNoisy() {
        U64 relative_rank_8_bb = c == WHITE ? RANK_8_BB : RANK_1_BB;
        U64 relative_rank_7_bb = c == WHITE ? RANK_7_BB : RANK_2_BB;
        U64 relative_rank_4_bb = c == WHITE ? RANK_4_BB : RANK_5_BB;
    
        Direction forward      = c == WHITE ? NORTH:SOUTH;
        Direction right        = c == WHITE ? NORTH_EAST:SOUTH_EAST;
        Direction left         = c == WHITE ? NORTH_WEST:SOUTH_WEST;
    
        const U64 opponents         =  b->getTeamOccupiedBB(!c);
    
        const U64 pawns             = b->getPieceBB(c, PAWN);
        const U64 occupied          = *b->getOccupiedBB();
    
        const U64 pawnsLeft   =  c == WHITE ? shiftNorthWest(pawns) : shiftSouthWest(pawns);
        const U64 pawnsRight  =  c == WHITE ? shiftNorthEast(pawns) : shiftSouthEast(pawns);
        const U64 pawnsCenter = (c == WHITE ? shiftNorth (pawns) : shiftSouth(pawns)) & ~occupied;

        const Piece movingPiece = c * 8 + PAWN;
    
        U64 nonPromoAttacks = opponents & ~relative_rank_8_bb;
        Square target;
        
        U64 attacks = pawnsLeft & nonPromoAttacks;
        while (attacks) {
            target = bitscanForward(attacks);
            addCapture(genMove(target - left, target, CAPTURE, movingPiece, b->getPiece(target)));
            attacks = lsbReset(attacks);
        }
        
        attacks = pawnsRight & nonPromoAttacks;
        while (attacks) {
            target = bitscanForward(attacks);
            addCapture(genMove(target - right, target, CAPTURE, movingPiece, b->getPiece(target)));
            attacks = lsbReset(attacks);
        }

        if (pawnsLeft & b->getBoardStatus()->enPassantTarget) {
            target = b->getEnPassantSquare();
            addCapture(genMove(target - left, target, EN_PASSANT, movingPiece));
        }
        
        if (pawnsRight & b->getBoardStatus()->enPassantTarget) {
            target = b->getEnPassantSquare();
            addCapture(genMove(target - right, target, EN_PASSANT, movingPiece));
        }
    
        if (pawns & relative_rank_7_bb) {
            
            attacks = pawnsCenter & relative_rank_8_bb;
            while (attacks) {
                target = bitscanForward(attacks);
                addNoisyPromotion(genMove(target - forward, target, QUEEN_PROMOTION, movingPiece));
                attacks = lsbReset(attacks);
            }
            
            attacks = pawnsLeft & relative_rank_8_bb & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addCapture(genMove(target - left, target, QUEEN_PROMOTION_CAPTURE , movingPiece, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & relative_rank_8_bb & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addCapture(genMove(target - right, target, QUEEN_PROMOTION_CAPTURE , movingPiece, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }
    }

    void generatePieceNoisy() {

        const U64 occupied   = *b->getOccupiedBB();
        const U64 opponents  = b->getTeamOccupiedBB(!c);
        const U64 friendly   = b->getTeamOccupiedBB(c);
        
        for(Piece p = KNIGHT; p <= QUEEN; p++){
            U64 pieceOcc    = b->getPieceBB(c, p);
            U64 movingPiece = p + 8 * c;
            while(pieceOcc){
                Square square = bitscanForward(pieceOcc);
                U64 attacks = ZERO;
                switch (p) {
                    case KNIGHT:
                        attacks = opponents & KNIGHT_ATTACKS[square];
                        break;
                    case BISHOP:
                        attacks =
                            opponents & lookUpBishopAttack  (square, occupied);
                        break;
                    case ROOK:
                        attacks =
                            opponents & lookUpRookAttack    (square,occupied);
                        break;
                    case QUEEN:
                        attacks =
                            (opponents & lookUpBishopAttack  (square, occupied)) |
                            (opponents & lookUpRookAttack    (square, occupied));
                        break;
                    
                }
                attacks &= ~friendly;

                while(attacks){
                    Square target = bitscanForward(attacks);
                    addCapture(genMove(square, target, CAPTURE, movingPiece, b->getPiece(target)));
                    attacks = lsbReset(attacks);
                }
                pieceOcc = lsbReset(pieceOcc);
            }
        }
    }

    void generateKingNoisy() {
        Piece movingPiece = KING + c * 8;
        
        const U64 occupied   = *b->getOccupiedBB();
        const U64 opponents  = b->getTeamOccupiedBB(!c);
        const U64 friendly   = b->getTeamOccupiedBB(c);
        
        U64 kings      = b->getPieceBB(c, KING);
        
        while (kings) {
            Square s       = bitscanForward(kings);
            U64 attacks = opponents & KING_ATTACKS[s] & ~friendly;
            while (attacks) {
                Square target = bitscanForward(attacks);
                addCapture(genMove(s, target, CAPTURE, movingPiece, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
            kings = lsbReset(kings);
        }
    }

    void generatePawnQuiet() { 
        U64 relative_rank_8_bb = c == WHITE ? RANK_8_BB : RANK_1_BB;
        U64 relative_rank_7_bb = c == WHITE ? RANK_7_BB : RANK_2_BB;
        U64 relative_rank_4_bb = c == WHITE ? RANK_4_BB : RANK_5_BB;
        
        Direction forward      = c == WHITE ? NORTH:SOUTH;
        Direction right        = c == WHITE ? NORTH_EAST:SOUTH_EAST;
        Direction left         = c == WHITE ? NORTH_WEST:SOUTH_WEST;
        
        const U64 opponents         =  b->getTeamOccupiedBB(!c);
        
        const U64 pawns             = b->getPieceBB(c, PAWN);
        const U64 occupied          = *b->getOccupiedBB();
        
        const U64 pawnsLeft   =  c == WHITE ? shiftNorthWest(pawns) : shiftSouthWest(pawns);
        const U64 pawnsRight  =  c == WHITE ? shiftNorthEast(pawns) : shiftSouthEast(pawns);
        const U64 pawnsCenter = (c == WHITE ? shiftNorth (pawns) : shiftSouth(pawns)) & ~occupied;
        
        const Piece movingPiece = c * 8 + PAWN;
        
        U64 nonPromoAttacks = opponents & ~relative_rank_8_bb;
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
    
        if (pawns & relative_rank_7_bb) {
            
            attacks = pawnsCenter & relative_rank_8_bb;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuiet(genMove(target - forward, target, ROOK_PROMOTION, movingPiece));
                addQuiet(genMove(target - forward, target, BISHOP_PROMOTION, movingPiece));
                addQuiet(genMove(target - forward, target, KNIGHT_PROMOTION, movingPiece));
                attacks = lsbReset(attacks);
            }
            
            attacks = pawnsLeft & relative_rank_8_bb & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuiet(genMove(target - left, target, ROOK_PROMOTION_CAPTURE  , movingPiece, b->getPiece(target)));
                addQuiet(genMove(target - left, target, BISHOP_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                addQuiet(genMove(target - left, target, KNIGHT_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & relative_rank_8_bb & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuiet(genMove(target - right, target, ROOK_PROMOTION_CAPTURE  , movingPiece, b->getPiece(target)));
                addQuiet(genMove(target - right, target, BISHOP_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                addQuiet(genMove(target - right, target, KNIGHT_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }
    }
    void generatePieceQuiet() {

        const U64 occupied   = *b->getOccupiedBB();
        const U64 opponents  = b->getTeamOccupiedBB(!c);
        const U64 friendly   = b->getTeamOccupiedBB(c);
        
        for(Piece p = KNIGHT; p <= QUEEN; p++){
            U64 pieceOcc    = b->getPieceBB(c, p);
            U64 movingPiece = p + 8 * c;
            while(pieceOcc){
                Square square = bitscanForward(pieceOcc);
                U64 attacks = ZERO;
                switch (p) {
                    case KNIGHT:
                        attacks = ~opponents & KNIGHT_ATTACKS[square];
                        break;
                    case BISHOP:
                        attacks =
                            ~opponents & lookUpBishopAttack  (square, occupied);
                        break;
                    case ROOK:
                        attacks =
                            ~opponents & lookUpRookAttack    (square,occupied);
                        break;
                    case QUEEN:
                        attacks =
                            (~opponents & lookUpBishopAttack  (square, occupied)) |
                            (~opponents & lookUpRookAttack    (square, occupied));
                        break;
                    
                }
                attacks &= ~friendly;

                while(attacks){
                    Square target = bitscanForward(attacks);
                    addQuiet(genMove(square, target, CAPTURE, movingPiece, b->getPiece(target)));
                    attacks = lsbReset(attacks);
                }
                pieceOcc = lsbReset(pieceOcc);
            }
        }
    }

    void generateKingQuiet() {
        Piece movingPiece = KING + c * 8;
        
        const U64 occupied   = *b->getOccupiedBB();
        const U64 opponents  = b->getTeamOccupiedBB(!c);
        const U64 friendly   = b->getTeamOccupiedBB(c);
        
        U64 kings      = b->getPieceBB(c, KING);
        
        while (kings) {
            Square s       = bitscanForward(kings);
            U64 attacks = ~opponents & KING_ATTACKS[s] & ~friendly;
            while (attacks) {
                Square target = bitscanForward(attacks);
                addQuiet(genMove(s, target, CAPTURE, movingPiece, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
            kings = lsbReset(kings);
        }
    }
};
#endif
