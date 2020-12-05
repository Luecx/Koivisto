//
// Created by Luecx on 04.12.2020.
//

#ifndef KOIVISTO_MATERIAL_H
#define KOIVISTO_MATERIAL_H

#include "Bitboard.h"

#include <cstdint>

using namespace bb;

#define pst_index_white_s(s)                         squareIndex(7 - rankIndex(s), fileIndex(s))
#define pst_index_black_s(s)                         s
#define pst_index_white(i, kside)                    squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_black(i, kside)                    squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define psqt_kingside_indexing(wkingside, bkingside) (wkingside) * 2 + (bkingside)

extern EvalScore* psqt[11];
extern EvalScore  pieceScores[6];

extern EvalScore fast_pawn_psqt     [2][4][64];
extern EvalScore fast_knight_psqt   [2][4][64];
extern EvalScore fast_bishop_psqt   [2][4][64];
extern EvalScore fast_rook_psqt     [2][4][64];
extern EvalScore fast_queen_psqt    [2][4][64];
extern EvalScore fast_king_psqt     [2]   [64];

extern EvalScore fast_psqt[5][2][4][64];


struct Material{

    // for each of the 4 king relations
    EvalScore scores[4]{};
    
    // the piece shall be given without the color (in the range of 0-5)
    inline void movePiece(Color c, Piece p, Square sFrom, Square sTo){
        if(p != KING){
            for(int i = 0; i < 4; i++){
                scores[i] += fast_psqt[p][c][i][sTo];
                scores[i] -= fast_psqt[p][c][i][sFrom];
            }
        }else{
            for(int i = 0; i < 4; i++){
                scores[i] += fast_king_psqt[c][sTo];
                scores[i] -= fast_king_psqt[c][sFrom];
            }
        }
    }
    inline void setPiece(Color c, Piece p, Square s){
        if(p != KING){
            for(int i = 0; i < 4; i++){
                scores[i] += fast_psqt[p][c][i][s];
            }
        }else{
            for(int i = 0; i < 4; i++){
                scores[i] += fast_king_psqt[c][s];
            }
        }
        
    }
    inline void unsetPiece(Color c, Piece p, Square s){
        if(p != KING){
            for(int i = 0; i < 4; i++){
                scores[i] -= fast_psqt[p][c][i][s];
            }
        }else{
            for(int i = 0; i < 4; i++){
                scores[i] -= fast_king_psqt[c][s];
            }
        }
    }
    
};

#endif    // KOIVISTO_MATERIAL_H
