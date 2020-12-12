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
#define showScore2i(s) "M(" << std::setw(4) << MgScore(s) << ","   << \
                               std::setw(4) << EgScore(s) << ")"

#define L(s1,s2,s3,s4)   (EvalScore4i)( \
                                 ((uint64_t) (s4) << 48) + \
                                 ((uint64_t) (s3) << 32) + \
                                 ((uint64_t) (s2) << 16) +  \
                                 (s1))
#define S1(s)   ((Score)((uint16_t)((uint64_t) ((s)))))
#define S2(s)   ((Score)((uint16_t)((uint64_t) ((s) + 0x8000)           >> 16)))
#define S3(s)   ((Score)((uint16_t)((uint64_t) ((s) + 0x80000000)       >> 32)))
#define S4(s)   ((Score)((uint16_t)((uint64_t) ((s) + 0x800000000000)   >> 48)))
#define showScore4i(s) "L(" << std::setw(4) << S1(s) << ","   << \
                               std::setw(4) << S2(s) << ","   << \
                               std::setw(4) << S3(s) << ","   << \
                               std::setw(4) << S4(s) << ")"
#define interpolateD2(s, x, y)      ((S1(s) * (1 - (x))*(1 - (y)) + \
                                      S2(s) * (1 + (x))*(1 - (y)) + \
                                      S3(s) * (1 - (x))*(1 + (y)) + \
                                      S4(s) * (1 + (x))*(1 + (y))) / 4.0)


#define pst_index_white_s(s)                                squareIndex(7 - rankIndex(s), fileIndex(s))
#define pst_index_black_s(s)                                s
#define pst_index_white(i, kside)                           squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_black(i, kside)                           squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_relative_white(pieceSquare, kingSquare)  (   (rankIndex(kingSquare)-rankIndex(pieceSquare)+7))*15+(fileIndex(kingSquare)-fileIndex(pieceSquare)+7)
#define pst_index_relative_black(pieceSquare, kingSquare)  (14-(rankIndex(kingSquare)-rankIndex(pieceSquare)+7))*15+(fileIndex(kingSquare)-fileIndex(pieceSquare)+7)

#define psqt_kingside_indexing(wkingside, bkingside) (wkingside) * 2 + (bkingside)

#define PSQT(wKing, bKing, piece, square) piece_kk_square_tables[wKing][bKing][piece][square]

// indexed by piece
extern EvalScore4i piece_values[6];

// indexed by piece, relation to king
extern EvalScore4i piece_our_king_square_table[5][15*15];
extern EvalScore4i piece_opp_king_square_table[5][15*15];

// indexed by piece, sameSideCastle, square
extern EvalScore4i piece_square_table[6][2][64];

// indexed by wking, bking, piece, square
extern EvalScore4i piece_kk_square_tables[64][64][12][64];

void psqt_init();

#endif    // KOIVISTO_PSQT_H
