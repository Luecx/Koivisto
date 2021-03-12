//
// Created by Luecx on 06.12.2020.
//

#ifndef KOIVISTO_PSQT_H
#define KOIVISTO_PSQT_H

#include "Bitboard.h"
using namespace bb;

#define M(mg, eg)    ((EvalScore)((unsigned int) (eg) << 16) + (mg))
#define MgScore(s)   ((Score)((uint16_t)((unsigned) ((s)))))
#define EgScore(s)   ((Score)((uint16_t)((unsigned) ((s) + 0x8000) >> 16)))
#define showScore(s) std::cout << "(" << MgScore(s) << ", " << EgScore(s) << ")" << std::endl;

#define pst_index_white_s(s)                                squareIndex(7 - rankIndex(s), fileIndex(s))
#define pst_index_black_s(s)                                s
#define pst_index_white(i, kside)                           squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_black(i, kside)                           squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_relative_white(pieceSquare, kingSquare)  (   (rankIndex(kingSquare)-rankIndex(pieceSquare)+7))*15+(fileIndex(kingSquare)-fileIndex(pieceSquare)+7)
#define pst_index_relative_black(pieceSquare, kingSquare)  (14-(rankIndex(kingSquare)-rankIndex(pieceSquare)+7))*15+(fileIndex(kingSquare)-fileIndex(pieceSquare)+7)

#define psqt_kingside_indexing(wkingside, bkingside) (wkingside) * 2 + (bkingside)

#define PSQT(wKing, bKing, piece, square) piece_kk_square_tables[wKing][bKing][piece][square]

// indexed by piece
extern EvalScore piece_values[6];

// indexed by piece, relation to king
extern EvalScore piece_our_king_square_table[5][15*15];
extern EvalScore piece_opp_king_square_table[5][15*15];

// indexed by piece, sameSideCastle, square
extern EvalScore piece_square_table[6][2][64];

// indexed by wking, bking, piece, square
extern EvalScore piece_kk_square_tables[64][64][14][64];

void psqt_init();

#endif    // KOIVISTO_PSQT_H
