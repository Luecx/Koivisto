
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

#ifndef KOIVISTO_GRADIENT_H
#define KOIVISTO_GRADIENT_H

#include "Board.h"
#include "eval.h"

#include <atomic>
#include <cstdint>
#include <omp.h>
#include <ostream>
#include <vector>
#include <fstream>

/**
 * How to add new eval params:
 * If it is a new feature, do the following:
 *    1. Add the feature name starting with I_ inside feature_indices
 *    2. Add the evaluation of the feature inside FeatureData.init()
 *    3. Add the feature name at the displayValues() to print the features.
 *
 * If it is a new array, ask Finn first
 *
 */
//#define TUNING
#ifdef TUNING
#define N_THREAD 16
namespace tuning {

    inline double sigmoid(double s, double K) { return (double) 1 / (1 + exp(-K * s / 400)); }

    inline double sigmoidPrime(double s, double K) {
        double ex = exp(-s * K / 400);
        return (K * ex) / (400 * (ex + 1) * (ex + 1));
    }

    enum feature_indices {
        I_SIDE_TO_MOVE,

        I_PAWN_STRUCTURE,
        I_PAWN_PASSED_AND_DOUBLED,
        I_PAWN_PASSED_AND_BLOCKED,
        I_PAWN_PASSED_COVERED_PROMO,
        I_PAWN_PASSED_HELPER,
        I_PAWN_PASSED_AND_DEFENDED,
        I_PAWN_PASSED_SQUARE_RULE,
        I_PAWN_CANDIDATE_PASSER,
        I_PAWN_ISOLATED,
        I_PAWN_DOUBLED,
        I_PAWN_DOUBLED_AND_ISOLATED,
        I_PAWN_BACKWARD,
        I_PAWN_OPEN,
        I_PAWN_BLOCKED,

        I_KNIGHT_OUTPOST,
        I_KNIGHT_DISTANCE_ENEMY_KING,

        I_ROOK_OPEN_FILE,
        I_ROOK_HALF_OPEN_FILE,
        I_ROOK_KING_LINE,

        I_BISHOP_DOUBLED,
        I_BISHOP_FIANCHETTO,
        I_BISHOP_PIECE_SAME_SQUARE_E,

        I_QUEEN_DISTANCE_ENEMY_KING,

        I_KING_CLOSE_OPPONENT,
        I_KING_PAWN_SHIELD,

        I_CASTLING_RIGHTS,
        I_MINOR_BEHIND_PAWN,
        
        I_SAFE_QUEEN_CHECK,
        I_SAFE_ROOK_CHECK,
        I_SAFE_BISHOP_CHECK,
        I_SAFE_KNIGHT_CHECK,

        I_PAWN_ATTACK_MINOR,
        I_PAWN_ATTACK_ROOK,
        I_PAWN_ATTACK_QUEEN,

        I_MINOR_ATTACK_ROOK,
        I_MINOR_ATTACK_QUEEN,
        I_ROOK_ATTACK_QUEEN,

        // always have this at the end
        I_END,
    };

    struct Param {
        Param() {  };

        Param(float value) : value(value) {}

        float value = 0;
        float gradient = 0;

        double firstMoment = 0;
        double secondMoment = 0;
        double t = 0;

        void update(float eta) {


            static double beta1 = 0.9;
            static double beta2 = 0.999;
            static double eps = 1e-8;


            this->t++;


            firstMoment  = beta1 *  firstMoment + (1 - beta1) * gradient;
            secondMoment = beta2 * secondMoment + (1 - beta2) * gradient * gradient;

            double  firstMomentCorrected =  firstMoment / (1 - pow(beta1, t));
            double secondMomentCorrected = secondMoment / (1 - pow(beta2, t));

            this->value -= eta / (sqrt(secondMomentCorrected) + eps) * firstMomentCorrected;

            this->gradient = 0;
        }
    };

    struct Weight {
        Param midgame{};
        Param endgame{};

        friend ostream &operator<<(ostream &os, const Weight &weight) {
            os << "M(" << std::setw(5) << round(weight.midgame.value) << "," << std::setw(5)
               << round(weight.endgame.value)
               << ")";
            return os;
        }

        void set(EvalScore &ev){
            midgame = Param{ (float )MgScore(ev)};
            endgame = Param{ (float )EgScore(ev)};
        }

        void set(Weight& other){
            midgame.value = other.midgame.value;
            endgame.value = other.endgame.value;
        }

        void update(float eta){
            midgame.update(eta);
            endgame.update(eta);
        }

        void merge(Weight& other){
            other.midgame.gradient += midgame.gradient;
            other.endgame.gradient += endgame.gradient;
            midgame.gradient = 0;
            endgame.gradient = 0;
        }

    };

    struct ThreadData {
        Weight w_piece_square_table[6][2][64]{};
        Weight w_piece_opp_king_square_table[5][15 * 15]{};
        Weight w_piece_our_king_square_table[5][15 * 15]{};
        Weight w_mobility[5][28]{};
        Weight w_features[I_END]{};
        Weight w_bishop_pawn_e[9]{};
        Weight w_bishop_pawn_o[9]{};
        Weight w_king_safety[100]{};
        Weight w_passer[8]{};
        Weight w_pinned[15]{};
        Weight w_hanging[5]{};
    };

    ThreadData threadData[N_THREAD]{};

    struct MetaData {
        // it can happen that the final evaluation is reduced by a given scalar

        float evalReduction = 1;
        float phase = 0;
        float matingMaterialWhite = false;
        float matingMaterialBlack = false;

        void init(Board *b, EvalData *ev) {
            phase =
                    (24.0f + phaseValues[5] -
                     phaseValues[0] * bitCount(b->getPieceBB()[WHITE_PAWN] | b->getPieceBB()[BLACK_PAWN])
                     - phaseValues[1] * bitCount(b->getPieceBB()[WHITE_KNIGHT] | b->getPieceBB()[BLACK_KNIGHT])
                     - phaseValues[2] * bitCount(b->getPieceBB()[WHITE_BISHOP] | b->getPieceBB()[BLACK_BISHOP])
                     - phaseValues[3] * bitCount(b->getPieceBB()[WHITE_ROOK] | b->getPieceBB()[BLACK_ROOK])
                     - phaseValues[4] * bitCount(b->getPieceBB()[WHITE_QUEEN] | b->getPieceBB()[BLACK_QUEEN]))
                    / 24.0f;

            if (phase > 1)
                phase = 1;
            if (phase < 0)
                phase = 0;

            matingMaterialWhite = hasMatingMaterial(b, WHITE);
            matingMaterialBlack = hasMatingMaterial(b, BLACK);
        }

        void evaluate(float &res, ThreadData* td) {
            if (res > 0 ? !matingMaterialWhite : !matingMaterialBlack)
                evalReduction = 1.0 / 10;
            else {
                evalReduction = 1;
            }
            res *= evalReduction;
        }
    };

    struct Pst64Data {
        // to quickly recompute the values for the piece square tables, we require the indices of each square
        // and how often is being used. We also need to know if we deal with same side or opposite side castling
        bool sameside_castle;

        int8_t indices_white[6][10]{};
        int8_t indices_black[6][10]{};

        void init(Board *b, EvalData *ev) {

            bool wKSide = (fileIndex(bitscanForward(b->getPieceBB()[WHITE_KING])) > 3 ? 0 : 1);
            bool bKSide = (fileIndex(bitscanForward(b->getPieceBB()[BLACK_KING])) > 3 ? 0 : 1);
            sameside_castle = wKSide == bKSide;

            for (Piece p = PAWN; p <= KING; p++) {
                for (int c= 0; c <= 1; c++) {
                    U64 k = b->getPieceBB(c, p);
                    while (k) {
                        Square s = bitscanForward(k);

                        if (p == KING) {
                            if (c == WHITE) {
                                indices_white[p][++indices_white[p][0]] = (pst_index_white_s(s));
                            } else {
                                indices_black[p][++indices_black[p][0]] = (pst_index_black_s(s));
                            }
                        } else {
                            if (c == WHITE) {
                                indices_white[p][++indices_white[p][0]] = (pst_index_white(s, wKSide));
                            } else {
                                indices_black[p][++indices_black[p][0]] = (pst_index_black(s, bKSide));
                            }
                        }

                        k = lsbReset(k);
                    }
                }
            }
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (Piece p = PAWN; p < KING; p++) {
                for (int i = 1; i <= indices_white[p][0]; i++) {
                    int8_t w = indices_white[p][i];
                    midgame += td->w_piece_square_table[p][!sameside_castle][w].midgame.value;
                    endgame += td->w_piece_square_table[p][!sameside_castle][w].endgame.value;
                }
                for (int i = 1; i <= indices_black[p][0]; i++) {
                    int8_t b = indices_black[p][i];
                    midgame -= td->w_piece_square_table[p][!sameside_castle][b].midgame.value;
                    endgame -= td->w_piece_square_table[p][!sameside_castle][b].endgame.value;
                }
            }

            // the kings are done seperate as they are not affected by castling side
            for (int i = 1; i <= indices_white[KING][0]; i++) {
                int8_t w = indices_white[KING][i];
                midgame += td->w_piece_square_table[KING][0][w].midgame.value;
                endgame += td->w_piece_square_table[KING][0][w].endgame.value;
            }
            for (int i = 1; i <= indices_black[KING][0]; i++) {
                int8_t b = indices_black[KING][i];
                midgame -= td->w_piece_square_table[KING][0][b].midgame.value;
                endgame -= td->w_piece_square_table[KING][0][b].endgame.value;
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {
            for (Piece p = PAWN; p < KING; p++) {
                for (int i = 1; i <= indices_white[p][0]; i++) {
                    int8_t w = indices_white[p][i];
                    td->w_piece_square_table[p][!sameside_castle][w].midgame.gradient +=
                            (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_piece_square_table[p][!sameside_castle][w].endgame.gradient +=
                            (meta->phase) * meta->evalReduction * lossgrad;
                }
                for (int i = 1; i <= indices_black[p][0]; i++) {
                    int8_t b = indices_black[p][i];
                    td->w_piece_square_table[p][!sameside_castle][b].midgame.gradient -=
                            (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_piece_square_table[p][!sameside_castle][b].endgame.gradient -=
                            (meta->phase) * meta->evalReduction * lossgrad;
                }
            }

            // the kings are done seperate as they are not affected by castling side
            for (int i = 1; i <= indices_white[KING][0]; i++) {
                int8_t w = indices_white[KING][i];
                td->w_piece_square_table[KING][0][w].midgame.gradient +=
                        (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_piece_square_table[KING][0][w].endgame.gradient +=
                        (meta->phase) * meta->evalReduction * lossgrad;
            }
            for (int i = 1; i <= indices_black[KING][0]; i++) {
                int8_t b = indices_black[KING][i];
                td->w_piece_square_table[KING][0][b].midgame.gradient -=
                        (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_piece_square_table[KING][0][b].endgame.gradient -=
                        (meta->phase) * meta->evalReduction * lossgrad;
            }
        }
    };

    struct Pst225Data {

        // only for pawns currently,
        uint8_t indices_white_wk[5][10]{};
        uint8_t indices_black_wk[5][10]{};
        uint8_t indices_white_bk[5][10]{};
        uint8_t indices_black_bk[5][10]{};

        void init(Board *b, EvalData *ev) {

            Square wKingSq = bitscanForward(b->getPieceBB()[WHITE_KING]);
            Square bKingSq = bitscanForward(b->getPieceBB()[BLACK_KING]);

            for (Piece p: {PAWN, QUEEN}) {
                for (int c= 0; c <= 1; c++) {
                    U64 k = b->getPieceBB(c, p);
                    while (k) {
                        Square s = bitscanForward(k);

                        if (c == WHITE) {
                            indices_white_wk[p][++indices_white_wk[p][0]] = (pst_index_relative_white(s, wKingSq));
                            indices_white_bk[p][++indices_white_bk[p][0]] = (pst_index_relative_white(s, bKingSq));
                        } else {
                            indices_black_wk[p][++indices_black_wk[p][0]] = (pst_index_relative_black(s, wKingSq));
                            indices_black_bk[p][++indices_black_bk[p][0]] = (pst_index_relative_black(s, bKingSq));
                        }

                        k = lsbReset(k);
                    }
                }
            }
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (Piece p : {PAWN, QUEEN}) {

                for (int i = 1; i <= indices_white_wk[p][0]; i++) {
                    uint8_t w = indices_white_wk[p][i];
                    midgame += td->w_piece_our_king_square_table[p][w].midgame.value;
                    endgame += td->w_piece_our_king_square_table[p][w].endgame.value;
                }

                for (int i = 1; i <= indices_white_bk[p][0]; i++) {
                    uint8_t w = indices_white_bk[p][i];
                    midgame += td->w_piece_opp_king_square_table[p][w].midgame.value;
                    endgame += td->w_piece_opp_king_square_table[p][w].endgame.value;
                }
                for (int i = 1; i <= indices_black_bk[p][0]; i++) {
                    uint8_t b = indices_black_bk[p][i];
                    midgame -= td->w_piece_our_king_square_table[p][b].midgame.value;
                    endgame -= td->w_piece_our_king_square_table[p][b].endgame.value;
                }

                for (int i = 1; i <= indices_black_wk[p][0]; i++) {
                    uint8_t b = indices_black_wk[p][i];
                    midgame -= td->w_piece_opp_king_square_table[p][b].midgame.value;
                    endgame -= td->w_piece_opp_king_square_table[p][b].endgame.value;
                }
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {
            for (Piece p: {PAWN, QUEEN}) {
                for (int i = 1; i <= indices_white_wk[p][0]; i++) {
                    uint8_t w = indices_white_wk[p][i];
                    td->w_piece_our_king_square_table[p][w].midgame.gradient +=
                            (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_piece_our_king_square_table[p][w].endgame.gradient +=
                            (meta->phase) * meta->evalReduction * lossgrad;
                }

                for (int i = 1; i <= indices_white_bk[p][0]; i++) {
                    uint8_t w = indices_white_bk[p][i];
                    td->w_piece_opp_king_square_table[p][w].midgame.gradient +=
                            (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_piece_opp_king_square_table[p][w].endgame.gradient +=
                            (meta->phase) * meta->evalReduction * lossgrad;
                }
                for (int i = 1; i <= indices_black_bk[p][0]; i++) {
                    uint8_t b = indices_black_bk[p][i];
                    td->w_piece_our_king_square_table[p][b].midgame.gradient -=
                            (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_piece_our_king_square_table[p][b].endgame.gradient -=
                            (meta->phase) * meta->evalReduction * lossgrad;
                }

                for (int i = 1; i <= indices_black_wk[p][0]; i++) {
                    uint8_t b = indices_black_wk[p][i];
                    td->w_piece_opp_king_square_table[p][b].midgame.gradient -=
                            (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_piece_opp_king_square_table[p][b].endgame.gradient -=
                            (meta->phase) * meta->evalReduction * lossgrad;
                }
            }
        }
    };

    struct KingSafetyData {
        // we only need to store one index for the white and black king

        int8_t wkingsafety_index;
        int8_t bkingsafety_index;

        void init(Board *b, EvalData *ev) {

            U64 k;
            Square square;
            U64 attacks;
            U64 occupied = *(b->getOccupiedBB());

            static int factors[6] = {0, 2, 2, 3, 4};

            Square whiteKingSquare = bitscanForward(b->getPieceBB()[WHITE_KING]);
            Square blackKingSquare = bitscanForward(b->getPieceBB()[BLACK_KING]);

            U64 whiteKingZone = KING_ATTACKS[whiteKingSquare];
            U64 blackKingZone = KING_ATTACKS[blackKingSquare];

            int wkingSafety_attPiecesCount = 0;
            int wkingSafety_valueOfAttacks = 0;

            int bkingSafety_attPiecesCount = 0;
            int bkingSafety_valueOfAttacks = 0;

            for (Piece p = KNIGHT; p <= QUEEN; p++) {
                for (int c= 0; c <= 1; c++) {
                    k = b->getPieceBB(c, p);
                    while (k) {
                        square = bitscanForward(k);
                        attacks = ZERO;
                        switch (p) {
                            case KNIGHT:
                                attacks = KNIGHT_ATTACKS[square];
                                break;
                            case BISHOP:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieceBB(c, QUEEN));
                                break;
                            case QUEEN:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieceBB(c, BISHOP)) |
                                        lookUpRookAttack    (square, occupied &~b->getPieceBB(c, ROOK));
                                break;
                            case ROOK:
                                attacks =
                                        lookUpRookAttack    (square,occupied &
                                                                    ~b->getPieceBB(c, QUEEN)&
                                                                    ~b->getPieceBB(c, ROOK));
                                break;
                        }
                        if (c == WHITE) {
                            addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount,
                                            bkingSafety_valueOfAttacks,
                                            factors[p]);
                        } else {
                            addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount,
                                            wkingSafety_valueOfAttacks,
                                            factors[p]);
                        }

                        k = lsbReset(k);
                    }
                }
            }

            wkingsafety_index = wkingSafety_valueOfAttacks;
            bkingsafety_index = bkingSafety_valueOfAttacks;
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            midgame += td->w_king_safety[bkingsafety_index].midgame.value - td->w_king_safety[wkingsafety_index].midgame.value;
            endgame += td->w_king_safety[bkingsafety_index].endgame.value - td->w_king_safety[wkingsafety_index].endgame.value;
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {
            td->w_king_safety[bkingsafety_index].midgame.gradient +=
                    (1 - meta->phase) * meta->evalReduction * lossgrad;
            td->w_king_safety[bkingsafety_index].endgame.gradient +=
                    (meta->phase) * meta->evalReduction * lossgrad;

            td->w_king_safety[wkingsafety_index].midgame.gradient -=
                    (1 - meta->phase) * meta->evalReduction * lossgrad;
            td->w_king_safety[wkingsafety_index].endgame.gradient -=
                    (meta->phase) * meta->evalReduction * lossgrad;
        }
    };

    struct BishopPawnTableData {
        int8_t count_e[9]{};
        int8_t count_o[9]{};

        void init(Board *b, EvalData *ev) {
            U64 k;
            Square square;

            U64 whitePawns = b->getPieceBB(WHITE, PAWN);
            U64 blackPawns = b->getPieceBB(BLACK, PAWN);

            k = b->getPieceBB()[WHITE_BISHOP];
            while (k) {
                square = bitscanForward(k); 
                count_e[bitCount(
                        blackPawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))] += 1;
                count_o[bitCount(
                        whitePawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))] += 1;
                k = lsbReset(k);
            }

            k = b->getPieceBB()[BLACK_BISHOP];
            while (k) {
                square = bitscanForward(k);
                count_e[bitCount(
                        whitePawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))] -= 1;
                count_o[bitCount(
                        blackPawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))] -= 1;
                k = lsbReset(k);
            }
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (int i = 0; i < 9; i++) {
                midgame += count_e[i] * td->w_bishop_pawn_e[i].midgame.value;
                endgame += count_e[i] * td->w_bishop_pawn_e[i].endgame.value;

                midgame += count_o[i] * td->w_bishop_pawn_o[i].midgame.value;
                endgame += count_o[i] * td->w_bishop_pawn_o[i].endgame.value;
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {
            for (int i = 0; i < 9; i++) {
                td->w_bishop_pawn_e[i].midgame.gradient +=
                        count_e[i] * (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_bishop_pawn_e[i].endgame.gradient +=
                        count_e[i] * (meta->phase) * meta->evalReduction * lossgrad;
                //            midgame += count_e[i] * w_bishop_pawn_e[i].midgame.value;
                //            endgame += count_e[i] * w_bishop_pawn_e[i].endgame.value;

                td->w_bishop_pawn_o[i].midgame.gradient +=
                        count_o[i] * (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_bishop_pawn_o[i].endgame.gradient +=
                        count_o[i] * (meta->phase) * meta->evalReduction * lossgrad;
                //            midgame += count_o[i] * w_bishop_pawn_o[i].midgame.value;
                //            endgame += count_o[i] * w_bishop_pawn_o[i].endgame.value;
            }
        }
    };

    struct PasserData {

        int8_t count[8]{};

        void init(Board *b, EvalData *ev) {
            for(Color color:{WHITE,BLACK}){
                U64 pawns    = b->getPieceBB( color, PAWN);
                U64 oppPawns = b->getPieceBB(!color, PAWN);
    
                U64 bb = pawns;
    
                while (bb) {
                    Square s      = bitscanForward(bb);
                    Rank   r      = color == WHITE ? rankIndex(s) : 7 - rankIndex(s);
                    File   f      = fileIndex(s);
                    U64    sqBB   = ONE << s;
        
                    U64 passerMask = passedPawnMask[color][s];
        
                    // check if passer
                    if (!(passerMask & oppPawns)){
                        U64    teleBB  = color == WHITE ? shiftNorth(sqBB) : shiftSouth(sqBB);
                        U64    promBB  = FILES_BB[f] & (color == WHITE ? RANK_8_BB:RANK_1_BB);
                        U64    promCBB = promBB & WHITE_SQUARES_BB ? WHITE_SQUARES_BB : BLACK_SQUARES_BB;
                        
                        count[r] += color == WHITE ? 1:-1;
                        
                    }
                    bb = lsbReset(bb);
                }
            }
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (int i = 0; i < 8; i++) {
                midgame += count[i] * td->w_passer[i].midgame.value;
                endgame += count[i] * td->w_passer[i].endgame.value;
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {

            for (int i = 0; i < 8; i++) {
                td->w_passer[i].midgame.gradient += count[i] * (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_passer[i].endgame.gradient += count[i] * (meta->phase) * meta->evalReduction * lossgrad;
            }
        }
    };

    struct FeatureData {
        // we assume that these features are linear which means that we only need their count

        int8_t count[I_END]{};

        void init(Board *b, EvalData *ev) {

            U64 k, attacks;
            Square square;

            Square whiteKingSquare = bitscanForward(b->getPieceBB()[WHITE_KING]);
            Square blackKingSquare = bitscanForward(b->getPieceBB()[BLACK_KING]);

            U64 whitePawns = b->getPieceBB()[WHITE_PAWN];
            U64 blackPawns = b->getPieceBB()[BLACK_PAWN];

            U64 whiteTeam = b->getTeamOccupiedBB()[WHITE];
            U64 blackTeam = b->getTeamOccupiedBB()[BLACK];

            // all passed pawns for white/black
            U64 whitePassers = wPassedPawns(whitePawns, blackPawns);
            U64 blackPassers = bPassedPawns(blackPawns, whitePawns);

            // doubled pawns without the pawn least developed
            U64 whiteDoubledWithoutFirst = wFrontSpans(whitePawns) & whitePawns;
            U64 blackDoubledWithoutFirst = bFrontSpans(blackPawns) & blackPawns;

            // all doubled pawns
            U64 whiteDoubledPawns = whiteDoubledWithoutFirst | (wRearSpans(whiteDoubledWithoutFirst) & whitePawns);
            U64 blackDoubledPawns = blackDoubledWithoutFirst | (bRearSpans(blackDoubledWithoutFirst) & blackPawns);

            // all isolated pawns
            U64 whiteIsolatedPawns = whitePawns & ~(fillFile(shiftWest(whitePawns) | shiftEast(whitePawns)));
            U64 blackIsolatedPawns = blackPawns & ~(fillFile(shiftWest(blackPawns) | shiftEast(blackPawns)));

            U64 whiteBlockedPawns = shiftNorth(whitePawns) & (whiteTeam | blackTeam);
            U64 blackBlockedPawns = shiftSouth(blackPawns) & (whiteTeam | blackTeam);

            U64 openFilesWhite = ~fillFile(whitePawns);
            U64 openFilesBlack = ~fillFile(blackPawns);
            U64 openFiles = openFilesBlack & openFilesWhite;

            U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
            U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
            U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
            U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;

            U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
            U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    
            
            U64 occupied = *b->getOccupiedBB();
            U64 wKingBishopAttacks = lookUpBishopAttack(whiteKingSquare, occupied)  & ~blackTeam;
            U64 bKingBishopAttacks = lookUpBishopAttack(blackKingSquare, occupied)  & ~whiteTeam;
            U64 wKingRookAttacks   = lookUpRookAttack  (whiteKingSquare, occupied)  & ~blackTeam;
            U64 bKingRookAttacks   = lookUpRookAttack  (blackKingSquare, occupied)  & ~whiteTeam;
            U64 wKingKnightAttacks = KNIGHT_ATTACKS    [whiteKingSquare]            & ~blackTeam;
            U64 bKingKnightAttacks = KNIGHT_ATTACKS    [blackKingSquare]            & ~whiteTeam;
            
            for(Color color:{WHITE,BLACK}){
    
                U64 pawns    = b->getPieceBB( color, PAWN);
                U64 oppPawns = b->getPieceBB(!color, PAWN);
    
                U64 bb = pawns;
                int h  = color == WHITE ? 1:-1;
    
                while (bb) {
                    Square s      = bitscanForward(bb);
                    Rank   r      = color == WHITE ? rankIndex(s) : 7 - rankIndex(s);
                    File   f      = fileIndex(s);
                    U64    sqBB   = ONE << s;
        
                    U64 passerMask = passedPawnMask[color][s];
        
                    // check if passer
                    if (!(passerMask & oppPawns)){
                        U64    teleBB  = color == WHITE ? shiftNorth(sqBB) : shiftSouth(sqBB);
                        U64    promBB  = FILES_BB[f] & (color == WHITE ? RANK_8_BB:RANK_1_BB);
                        U64    promCBB = promBB & WHITE_SQUARES_BB ? WHITE_SQUARES_BB : BLACK_SQUARES_BB;
            
            
                        // check if doubled
                        count[I_PAWN_PASSED_AND_DOUBLED] += bitCount(teleBB & pawns) * h;
            
                        // check if square in front is blocked
                        count[I_PAWN_PASSED_AND_BLOCKED] += bitCount(teleBB & b->getTeamOccupiedBB(!color)) * h;
            
                        // check if promotion square can be covered
                        count[I_PAWN_PASSED_COVERED_PROMO] +=
                               (  bitCount(b->getPieceBB(color, BISHOP) & promCBB)
                                + bitCount(b->getPieceBB(color, QUEEN))
                                - bitCount(b->getPieceBB(!color, BISHOP) & promCBB)
                                - bitCount(b->getPieceBB(!color, QUEEN))) * h;
            
                        // check if there is a helper
                        count[I_PAWN_PASSED_HELPER]       += (bitCount(pawns & (color == WHITE ? wAttackRearSpans(pawns) : bAttackRearSpans(pawns)))) * h;
            
                        // check if its defended
                        count[I_PAWN_PASSED_AND_DEFENDED] += (bitCount(sqBB & ev->pawnWestAttacks[color]) + bitCount(sqBB & ev->pawnEastAttacks[color])) * h;
            
                        // check if can be caught by king
                        count[I_PAWN_PASSED_SQUARE_RULE]  += ((7 - r + (color != b->getActivePlayer())) < manhattanDistance(
                            bitscanForward(promBB),
                            bitscanForward(b->getPieceBB(!color, KING)))) * h;
                    }
                    // check for candidates
                    if((ONE << s) & ev->semiOpen[color]){
                        U64 frontSpan           = color == WHITE ? wFrontSpans(sqBB) : bFrontSpans(sqBB);
                        int defendingPawnCount  = bitCount(frontSpan & ev->pawnEastAttacks[!color]) +
                                                  bitCount(frontSpan & ev->pawnWestAttacks[!color]);
                        int attackingPawnCount  = bitCount(fillFile(FILES_NEIGHBOUR_BB[f] & pawns)) >> 3;
        
                        // it is a candidate
                        if(attackingPawnCount >= defendingPawnCount){
                            count[I_PAWN_CANDIDATE_PASSER] += (attackingPawnCount - defendingPawnCount + 1) * h;
                        }
                    }
                    
                    bb = lsbReset(bb);
                }
            }
            
            // clang-format off
            count[I_PAWN_ATTACK_MINOR]          = (bitCount(whitePawnCover & (b->getPieceBB<BLACK>(KNIGHT) | b->getPieceBB<BLACK>(BISHOP)))-bitCount(blackPawnCover & (b->getPieceBB<WHITE>(KNIGHT) | b->getPieceBB<WHITE>(BISHOP))));
            count[I_PAWN_ATTACK_ROOK]           = (bitCount(whitePawnCover & b->getPieceBB<BLACK>(ROOK))-bitCount(blackPawnCover & b->getPieceBB<WHITE>(ROOK)));
            count[I_PAWN_ATTACK_QUEEN]          = (bitCount(whitePawnCover & b->getPieceBB<BLACK>(QUEEN))-bitCount(blackPawnCover & b->getPieceBB<WHITE>(QUEEN)));
            count[I_PAWN_DOUBLED_AND_ISOLATED]  = (
                    +bitCount(whiteIsolatedPawns & whiteDoubledPawns)
                    - bitCount(blackIsolatedPawns & blackDoubledPawns));
            count[I_PAWN_DOUBLED] += (
                    +bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
                    - bitCount(~blackIsolatedPawns & blackDoubledPawns));
            count[I_PAWN_ISOLATED] += (
                    +bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
                    - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
            count[I_PAWN_STRUCTURE] += (
                    +bitCount(whitePawnEastCover)
                    + bitCount(whitePawnWestCover)
                    - bitCount(blackPawnEastCover)
                    - bitCount(blackPawnWestCover));
            count[I_PAWN_OPEN] += (
                    +bitCount(whitePawns & ~fillSouth(blackPawns))
                    - bitCount(blackPawns & ~fillNorth(whitePawns)));
            count[I_PAWN_BACKWARD] += (
                    +bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & blackPawnCover) & whitePawns)
                    - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & whitePawnCover) & blackPawns));
            count[I_PAWN_BLOCKED] += (
                    +bitCount(whiteBlockedPawns)
                    - bitCount(blackBlockedPawns));
            count[I_MINOR_BEHIND_PAWN] += (
                    +bitCount(shiftNorth(b->getPieceBB()[WHITE_KNIGHT] | b->getPieceBB()[WHITE_BISHOP]) &
                              (b->getPieceBB()[WHITE_PAWN] | b->getPieceBB()[BLACK_PAWN]))
                    - bitCount(shiftSouth(b->getPieceBB()[BLACK_KNIGHT] | b->getPieceBB()[BLACK_BISHOP]) &
                               (b->getPieceBB()[WHITE_PAWN] | b->getPieceBB()[BLACK_PAWN])));

            k = b->getPieceBB()[WHITE_KNIGHT];
            while (k) {
                square = bitscanForward(k);
                attacks = KNIGHT_ATTACKS[square];
                count[I_MINOR_ATTACK_ROOK] +=  bitCount(attacks & b->getPieceBB<BLACK>(ROOK));
                count[I_MINOR_ATTACK_QUEEN] += bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
                count[I_KNIGHT_OUTPOST] += isOutpost(square, WHITE, blackPawns, whitePawnCover);
                count[I_KNIGHT_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
                count[I_SAFE_KNIGHT_CHECK] += bitCount(bKingKnightAttacks & attacks & ~blackPawnCover);
                k = lsbReset(k);
            }

            k = b->getPieceBB()[BLACK_KNIGHT];
            while (k) {
                square = bitscanForward(k);
                attacks = KNIGHT_ATTACKS[square];
                count[I_MINOR_ATTACK_ROOK] -=  bitCount(attacks & b->getPieceBB<WHITE>(ROOK));
                count[I_MINOR_ATTACK_QUEEN] -= bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
                count[I_KNIGHT_OUTPOST] -= isOutpost(square, BLACK, whitePawns, blackPawnCover);
                count[I_KNIGHT_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
                count[I_SAFE_KNIGHT_CHECK] -= bitCount(wKingKnightAttacks & attacks & ~whitePawnCover);
                k = lsbReset(k);
            }


            k = b->getPieceBB()[WHITE_BISHOP];
            while (k) {
                square = bitscanForward(k);
                attacks = lookUpBishopAttack(square, occupied & ~b->getPieceBB()[WHITE_QUEEN]);
                count[I_MINOR_ATTACK_ROOK] +=  bitCount(attacks & b->getPieceBB<BLACK>(ROOK));
                count[I_MINOR_ATTACK_QUEEN] += bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
                count[I_BISHOP_PIECE_SAME_SQUARE_E] +=
                        bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB));
                count[I_BISHOP_FIANCHETTO] +=
                        (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                         && whitePawns & (ONE << G3 | ONE << G4));
                count[I_BISHOP_FIANCHETTO] +=
                        (square == B2 && whitePawns & ONE << A2 && whitePawns & ONE << C2
                         && whitePawns & (ONE << B3 | ONE << B4));
                count[I_SAFE_BISHOP_CHECK] += bitCount(bKingBishopAttacks & attacks & ~blackPawnCover);
                k = lsbReset(k);
            }

            k = b->getPieceBB()[BLACK_BISHOP];
            while (k) {
                square = bitscanForward(k);
                attacks = lookUpBishopAttack(square, occupied & ~b->getPieceBB()[BLACK_QUEEN]);
                count[I_MINOR_ATTACK_ROOK] -=  bitCount(attacks & b->getPieceBB<WHITE>(ROOK));
                count[I_MINOR_ATTACK_QUEEN] -= bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
                count[I_BISHOP_PIECE_SAME_SQUARE_E] -=
                        bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB));
                count[I_BISHOP_FIANCHETTO] -=
                        (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                         && blackPawns & (ONE << G6 | ONE << G5));
                count[I_BISHOP_FIANCHETTO] -=
                        (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                         && blackPawns & (ONE << B6 | ONE << B5));
                count[I_SAFE_BISHOP_CHECK] -= bitCount(wKingBishopAttacks & attacks & ~whitePawnCover);

                k = lsbReset(k);
            }
            count[I_BISHOP_DOUBLED] += (
                    +(bitCount(b->getPieceBB()[WHITE_BISHOP]) == 2)
                    - (bitCount(b->getPieceBB()[BLACK_BISHOP]) == 2));
    
    
            k = b->getPieceBB()[WHITE_ROOK];
            while (k) {
                square  = bitscanForward(k);
                attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[WHITE_ROOK] & ~b->getPieceBB()[WHITE_QUEEN]);
                count[I_ROOK_ATTACK_QUEEN] += bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
                count[I_SAFE_ROOK_CHECK] += bitCount(bKingRookAttacks & attacks & ~blackPawnCover);

                k = lsbReset(k);
            }
    
            k = b->getPieceBB()[BLACK_ROOK];
            while (k) {
                square  = bitscanForward(k);
                attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[BLACK_ROOK] & ~b->getPieceBB()[BLACK_QUEEN]);
                count[I_ROOK_ATTACK_QUEEN] -= bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
                count[I_SAFE_ROOK_CHECK] -= bitCount(wKingRookAttacks & attacks & ~whitePawnCover);

                k = lsbReset(k);
            }

            count[I_ROOK_KING_LINE] += (
                    +bitCount(lookUpRookAttack(blackKingSquare, *b->getOccupiedBB()) & b->getPieceBB(WHITE, ROOK))
                    - bitCount(lookUpRookAttack(whiteKingSquare, *b->getOccupiedBB()) & b->getPieceBB(BLACK, ROOK)));
            count[I_ROOK_OPEN_FILE] += (
                    +bitCount(openFiles & b->getPieceBB(WHITE, ROOK))
                    - bitCount(openFiles & b->getPieceBB(BLACK, ROOK)));
            count[I_ROOK_HALF_OPEN_FILE] += (
                    +bitCount(openFilesBlack & ~openFiles & b->getPieceBB(WHITE, ROOK))
                    - bitCount(openFilesWhite & ~openFiles & b->getPieceBB(BLACK, ROOK)));


            k = b->getPieceBB()[WHITE_QUEEN];
            while (k) {
                square = bitscanForward(k);
                attacks = lookUpRookAttack  (square,    occupied & ~b->getPieceBB()[WHITE_ROOK])
                        | lookUpBishopAttack(square,    occupied & ~b->getPieceBB()[WHITE_BISHOP]);
                count[I_QUEEN_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
                count[I_SAFE_QUEEN_CHECK]          += bitCount((bKingRookAttacks | bKingBishopAttacks) & attacks & ~blackPawnCover);
                k = lsbReset(k);
            }

            k = b->getPieceBB()[BLACK_QUEEN];
            while (k) {
                square = bitscanForward(k);
                attacks = lookUpRookAttack  (square,    occupied & ~b->getPieceBB()[BLACK_ROOK])
                        | lookUpBishopAttack(square,    occupied & ~b->getPieceBB()[BLACK_BISHOP]);
                count[I_QUEEN_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
                count[I_SAFE_QUEEN_CHECK]          -= bitCount((wKingRookAttacks | wKingBishopAttacks) & attacks & ~whitePawnCover);
                k = lsbReset(k);
            }

            k = b->getPieceBB()[WHITE_KING];
            while (k) {
                square = bitscanForward(k);
                count[I_KING_PAWN_SHIELD] += bitCount(KING_ATTACKS[square] & whitePawns);
                count[I_KING_CLOSE_OPPONENT] += bitCount(KING_ATTACKS[square] & blackTeam);
                k = lsbReset(k);
            }

            k = b->getPieceBB()[BLACK_KING];
            while (k) {
                square = bitscanForward(k);
                count[I_KING_PAWN_SHIELD] -= bitCount(KING_ATTACKS[square] & blackPawns);
                count[I_KING_CLOSE_OPPONENT] -= bitCount(KING_ATTACKS[square] & whiteTeam);
                k = lsbReset(k);
            }

            count[I_CASTLING_RIGHTS] +=
                    + b->getCastlingRights(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
                    + b->getCastlingRights(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
                    - b->getCastlingRights(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
                    - b->getCastlingRights(STATUS_INDEX_BLACK_KINGSIDE_CASTLING);
            count[I_SIDE_TO_MOVE] += (b->getActivePlayer() == WHITE ? 1 : -1);
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (int i = 0; i < I_END; i++) {
                midgame += count[i] * td->w_features[i].midgame.value;
                endgame += count[i] * td->w_features[i].endgame.value;
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {

            for (int i = 0; i < I_END; i++) {
                td->w_features[i].midgame.gradient +=
                        count[i] * (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_features[i].endgame.gradient += count[i] * (meta->phase) * meta->evalReduction * lossgrad;
            }
        }
    };

    struct PinnedData {
        int8_t count[15]{};

        void init(Board *b, EvalData *ev) {

            for (int color= 0; color <= 1; color++) {

                Color us = color;
                Color them = 1 - color;

                // figure out where the opponent has pieces
                U64 opponentOcc = b->getTeamOccupiedBB()[them];
                U64 ourOcc = b->getTeamOccupiedBB()[us];

                // get the pieces which can pin our king
                U64 bishops = b->getPieceBB(them, BISHOP);
                U64 rooks = b->getPieceBB(them, ROOK);
                U64 queens = b->getPieceBB(them, QUEEN);

                // get the king positions
                Square kingSq = bitscanForward(b->getPieceBB(us, KING));

                // get the potential pinners for rook/bishop attacks
                U64 rookAttacks = lookUpRookAttack(kingSq, opponentOcc) & (rooks | queens);
                U64 bishopAttacks = lookUpBishopAttack(kingSq, opponentOcc) & (bishops | queens);

                // get all pinners (either rook or bishop attackers)
                U64 potentialPinners = (rookAttacks | bishopAttacks);

                while (potentialPinners) {

                    Square pinnerSquare = bitscanForward(potentialPinners);

                    // get all the squares in between the king and the potential pinner
                    U64 inBetween = inBetweenSquares[kingSq][pinnerSquare];

                    // if there is exactly one of our pieces in the way, consider it pinned. Otherwise, continue
                    U64 potentialPinned = ourOcc & inBetween;
                    if (potentialPinned == 0 || lsbIsolation(potentialPinned) != potentialPinned) {
                        potentialPinners = lsbReset(potentialPinners);
                        continue;
                    }

                    // extract the pinner pieces and the piece that pins
                    Piece pinnedPiece = b->getPiece(bitscanForward(potentialPinned));
                    Piece pinnerPiece = b->getPiece(pinnerSquare) - BISHOP;

                    // normalise the values (black pieces will be made to white pieces)
                    if (us == WHITE) {
                        pinnerPiece -= 8;
                    } else {
                        pinnedPiece -= 8;
                    }


                    // add to the result indexing using pinnedPiece for which there are 5 different pieces and the pinner
                    count[pinnedPiece * 3 + pinnerPiece] += color == WHITE ? 1 : -1;

                    // reset the lsb
                    potentialPinners = lsbReset(potentialPinners);
                }
            }

        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (int i = 0; i < 15; i++) {
                midgame += count[i] * td->w_pinned[i].midgame.value;
                endgame += count[i] * td->w_pinned[i].endgame.value;
            }
            
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {

            for (int i = 0; i < 15; i++) {
                td->w_pinned[i].midgame.gradient += count[i] * (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_pinned[i].endgame.gradient += count[i] * (meta->phase) * meta->evalReduction * lossgrad;
            }
        }
    };

    struct HangingData {
        int8_t count[5]{};

        void init(Board *b, EvalData *ev) {
            U64 WnotAttacked = ~b->getAttackedSquares<WHITE>();
            U64 BnotAttacked = ~b->getAttackedSquares<BLACK>();

            for (int i = PAWN; i <= QUEEN; i++) {
                count[i] =
                        + bitCount(b->getPieceBB(WHITE, i) & WnotAttacked)
                        - bitCount(b->getPieceBB(BLACK, i) & BnotAttacked);
            }
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (int i = 0; i < 5; i++) {
                midgame += count[i] * td->w_hanging[i].midgame.value;
                endgame += count[i] * td->w_hanging[i].endgame.value;
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {

            for (int i = 0; i < 5; i++) {
                td->w_hanging[i].midgame.gradient +=
                        count[i] * (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_hanging[i].endgame.gradient += count[i] * (meta->phase) * meta->evalReduction * lossgrad;
            }
        }
    };

    struct MobilityData {

        uint8_t indices_white[5][10]{};
        uint8_t indices_black[5][10]{};

        void init(Board *b, EvalData *ev) {

            U64 k, attacks;
            Square square;

            U64 whitePawnCover = shiftNorthEast(b->getPieceBB(WHITE, PAWN)) | shiftNorthWest(b->getPieceBB(WHITE, PAWN));
            U64 blackPawnCover = shiftSouthEast(b->getPieceBB(BLACK, PAWN)) | shiftSouthWest(b->getPieceBB(BLACK, PAWN));

            U64 mobilitySquaresWhite = ~(b->getTeamOccupiedBB()[WHITE]) & ~(blackPawnCover);
            U64 mobilitySquaresBlack = ~(b->getTeamOccupiedBB()[BLACK]) & ~(whitePawnCover);

            U64 occupied = *b->getOccupiedBB();

            for (Piece p = KNIGHT; p <= QUEEN; p++) {
                for (int c= 0; c <= 1; c++) {
                    k = b->getPieceBB(c, p);
                    while (k) {
                        square = bitscanForward(k);
                        attacks = ZERO;
                        switch (p) {
                            case KNIGHT:
                                attacks = KNIGHT_ATTACKS[square];
                                break;
                            case BISHOP:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieceBB(c, QUEEN));
                                break;
                            case QUEEN:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieceBB(c, BISHOP)) |
                                        lookUpRookAttack    (square, occupied &~b->getPieceBB(c, ROOK));
                                break;
                            case ROOK:
                                attacks =
                                        lookUpRookAttack    (square,occupied &
                                                ~b->getPieceBB(c, QUEEN)&
                                                ~b->getPieceBB(c, ROOK));
                                break;
                        }
                        if (c == WHITE) {
                            indices_white[p][++indices_white[p][0]] = (bitCount(attacks & mobilitySquaresWhite));
                        } else {
                            indices_black[p][++indices_black[p][0]] = (bitCount(attacks & mobilitySquaresBlack));
                        }

                        k = lsbReset(k);
                    }
                }
            }
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (Piece p = PAWN; p <= QUEEN; p++) {
                for (int i = 1; i <= indices_white[p][0]; i++) {
                    int8_t w = indices_white[p][i];
                    midgame += td->w_mobility[p][w].midgame.value;
                    endgame += td->w_mobility[p][w].endgame.value;
                }
                for (int i = 1; i <= indices_black[p][0]; i++) {
                    int8_t b = indices_black[p][i];
                    midgame -= td->w_mobility[p][b].midgame.value;
                    endgame -= td->w_mobility[p][b].endgame.value;
                }
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {

            for (Piece p = PAWN; p <= QUEEN; p++) {
                for (int i = 1; i <= indices_white[p][0]; i++) {
                    int8_t w = indices_white[p][i];

                    td->w_mobility[p][w].midgame.gradient += (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_mobility[p][w].endgame.gradient += (meta->phase) * meta->evalReduction * lossgrad;

                }
                for (int i = 1; i <= indices_black[p][0]; i++) {
                    int8_t b = indices_black[p][i];

                    td->w_mobility[p][b].midgame.gradient -= (1 - meta->phase) * meta->evalReduction * lossgrad;
                    td->w_mobility[p][b].endgame.gradient -= (meta->phase) * meta->evalReduction * lossgrad;

                }
            }
        }


    };

    struct PosEvalData {
        FeatureData features{};
        MobilityData mobility{};
        HangingData hanging{};
        PinnedData pinned{};
        PasserData passed{};
        BishopPawnTableData bishop_pawn{};
        KingSafetyData king_safety{};
        Pst64Data pst64{};
        Pst225Data pst225{};
        MetaData meta{};

        void init(Board *b, EvalData *ev) {
            features.init(b, ev);
            mobility.init(b, ev);
            hanging.init(b, ev);
            pinned.init(b, ev);
            passed.init(b, ev);
            bishop_pawn.init(b, ev);
            king_safety.init(b, ev);
            pst64.init(b, ev);
            pst225.init(b, ev);
            meta.init(b, ev);
        }

        double evaluate(int threadID = 0) {
            float midgame = 0;
            float endgame = 0;
    

            features.evaluate(midgame, endgame, &threadData[threadID]);
            mobility.evaluate(midgame, endgame, &threadData[threadID]);
            hanging.evaluate(midgame, endgame, &threadData[threadID]);
            pinned.evaluate(midgame, endgame, &threadData[threadID]);
            passed.evaluate(midgame, endgame, &threadData[threadID]);
            bishop_pawn.evaluate(midgame, endgame, &threadData[threadID]);
            king_safety.evaluate(midgame, endgame, &threadData[threadID]);
            pst64.evaluate(midgame, endgame, &threadData[threadID]);
            pst225.evaluate(midgame, endgame, &threadData[threadID]);

            float res = (int) (meta.phase * endgame) + (int) ((1 - meta.phase) * midgame);
            meta.evaluate(res, &threadData[threadID]);

            return res;
        }

        void gradient(float lossgrad, int threadID) {
            features.gradient(&meta, lossgrad, &threadData[threadID]);
            mobility.gradient(&meta, lossgrad, &threadData[threadID]);
            hanging.gradient(&meta, lossgrad, &threadData[threadID]);
            pinned.gradient(&meta, lossgrad, &threadData[threadID]);
            passed.gradient(&meta, lossgrad, &threadData[threadID]);
            bishop_pawn.gradient(&meta, lossgrad, &threadData[threadID]);
            king_safety.gradient(&meta, lossgrad, &threadData[threadID]);
            pst64.gradient(&meta, lossgrad, &threadData[threadID]);
            pst225.gradient(&meta, lossgrad, &threadData[threadID]);
        }

        double train(float target, float K, int threadID) {
            double out = evaluate(threadID);
            double sig = sigmoid(out, K);
            double sigPrime = sigmoidPrime(out, K);
            double difference = sig - target;
            double lossgrad = difference * sigPrime;

            gradient(lossgrad, threadID);
            return 0.5 * difference * difference;
        }

        float error(float target, float K) {
            double out = evaluate();
            double sig = sigmoid(out, K);
            double difference = sig - target;

            return 0.5 * difference * difference;
        }
    };

    struct TrainEntry {
        PosEvalData posEvalData;
        double target;

        TrainEntry(Board *b, EvalData* ev, float target) {
            this->posEvalData.init(b, ev);
            this->target = target;
        }
    };


    std::vector<TrainEntry> positions{};

    void load_weights() {
        for (int t = 0; t < N_THREAD; t++) {
            for (int i = 0; i < 6; i++) {

                for (int n = 0; n < 2; n++) {
                    for (int j = 0; j < 64; j++) {
                        float w1 = MgScore(piece_square_table[i][n][j] + piece_values[i]);
                        float w2 = EgScore(piece_square_table[i][n][j] + piece_values[i]);
                        threadData[t].w_piece_square_table[i][n][j] = {{w1},
                                                                       {w2}};
                    }
                }

                if (i < 5) {

                    for (int n = 0; n < 15 * 15; n++) {
                        threadData[t].w_piece_opp_king_square_table[i][n].set(piece_opp_king_square_table[i][n]);

                        threadData[t].w_piece_our_king_square_table[i][n].set(piece_our_king_square_table[i][n]);
                    }

                    for (int n = 0; n < mobEntryCount[i]; n++) {
                        threadData[t].w_mobility[i][n].set(mobilities[i][n]);
                    }

                }
            }
            for (int i = 0; i < 1000; i++) {
                if (i < I_END) {
                    threadData[t].w_features[i].set(*evfeatures[i]);
                }
                if (i < 9) {
                    threadData[t].w_bishop_pawn_e[i].set(bishop_pawn_same_color_table_e[i]);
                    threadData[t].w_bishop_pawn_o[i].set(bishop_pawn_same_color_table_o[i]);
                }

                if (i < 100) {
                    threadData[t].w_king_safety[i].set(kingSafetyTable[i]);
                }
                if (i < 8) {
                    threadData[t].w_passer[i].set(passer_rank_n[i]);
                }
                if (i < 15) {
                    threadData[t].w_pinned[i].set(pinnedEval[i]);
                }
                if (i < 5) {
                    threadData[t].w_hanging[i].set(hangingEval[i]);
                }
            }
        }

    }

    void merge_gradients() {
        for (int t = 1; t < N_THREAD; t++) {
            for (int i = 0; i < 6; i++) {

                for (int n = 0; n < 2; n++) {
                    for (int j = 0; j < 64; j++) {
                        threadData[t].w_piece_square_table[i][n][j].merge(threadData[0].w_piece_square_table[i][n][j]);
                    }
                }

                if (i < 5) {

                    for (int n = 0; n < 15 * 15; n++) {
                        threadData[t].w_piece_opp_king_square_table[i][n].merge(threadData[0].w_piece_opp_king_square_table[i][n]);
                        threadData[t].w_piece_our_king_square_table[i][n].merge(threadData[0].w_piece_our_king_square_table[i][n]);
                    }

                    for (int n = 0; n < mobEntryCount[i]; n++) {
                        threadData[t].w_mobility[i][n].merge(threadData[0].w_mobility[i][n]);
                    }

                }
            }
            for (int i = 0; i < 1000; i++) {
                if (i < I_END) {
                    threadData[t].w_features[i].merge(threadData[0].w_features[i]);
                }
                if (i < 9) {
                    threadData[t].w_bishop_pawn_e[i].merge(threadData[0].w_bishop_pawn_e[i]);
                    threadData[t].w_bishop_pawn_o[i].merge(threadData[0].w_bishop_pawn_o[i]);
                }

                if (i < 100) {
                    threadData[t].w_king_safety[i].merge(threadData[0].w_king_safety[i]);

                }
                if (i < 8) {
                    threadData[t].w_passer[i].merge(threadData[0].w_passer[i]);
                }
                if (i < 15) {

                    threadData[t].w_pinned[i].merge(threadData[0].w_pinned[i]);

                }
                if (i < 5) {
                    threadData[t].w_hanging[i].merge(threadData[0].w_hanging[i]);

                }
            }
        }
    }

    void share_weights() {
        for (int t = 1; t < N_THREAD; t++) {
            for (int i = 0; i < 6; i++) {

                for (int n = 0; n < 2; n++) {
                    for (int j = 0; j < 64; j++) {
                        threadData[t].w_piece_square_table[i][n][j].set(threadData[0].w_piece_square_table[i][n][j]);
                    }
                }

                if (i < 5) {

                    for (int n = 0; n < 15 * 15; n++) {
                        threadData[t].w_piece_opp_king_square_table[i][n].set(threadData[0].w_piece_opp_king_square_table[i][n]);
                        threadData[t].w_piece_our_king_square_table[i][n].set(threadData[0].w_piece_our_king_square_table[i][n]);
                    }

                    for (int n = 0; n < mobEntryCount[i]; n++) {
                        threadData[t].w_mobility[i][n].set(threadData[0].w_mobility[i][n]);
                    }

                }
            }
            for (int i = 0; i < 1000; i++) {
                if (i < I_END) {
                    threadData[t].w_features[i].set(threadData[0].w_features[i]);
                }
                if (i < 9) {
                    threadData[t].w_bishop_pawn_e[i].set(threadData[0].w_bishop_pawn_e[i]);
                    threadData[t].w_bishop_pawn_o[i].set(threadData[0].w_bishop_pawn_o[i]);
                }

                if (i < 100) {
                    threadData[t].w_king_safety[i].set(threadData[0].w_king_safety[i]);

                }
                if (i < 8) {
                    threadData[t].w_passer[i].set(threadData[0].w_passer[i]);

                }
                if (i < 15) {

                    threadData[t].w_pinned[i].set(threadData[0].w_pinned[i]);

                }
                if (i < 5) {
                    threadData[t].w_hanging[i].set(threadData[0].w_hanging[i]);

                }
            }
        }
    }

    void adjust_weights(float eta) {

        merge_gradients();

        for (int i = 0; i < 6; i++) {

            for (int n = 0; n < 2; n++) {
                for (int j = 0; j < 64; j++) {
                    threadData[0].w_piece_square_table[i][n][j].update(eta);
                }
            }

            if (i < 5) {

                if (i == PAWN || i == QUEEN) {
                    for (int n = 0; n < 15 * 15; n++) {
                        threadData[0].w_piece_opp_king_square_table[i][n].update(eta);
                        threadData[0].w_piece_our_king_square_table[i][n].update(eta);
                    }
                }


                for (int n = 0; n < mobEntryCount[i]; n++) {
                    threadData[0].w_mobility[i][n].update(eta);
                }

            }
        }
        for (int i = 0; i < 1000; i++) {
            if (i < I_END) {
                threadData[0].w_features[i].update(eta);
            }
            if (i < 9) {
                threadData[0].w_bishop_pawn_e[i].update(eta);
            }
            if (i < 9) {
                threadData[0].w_bishop_pawn_o[i].update(eta);
            }
            if (i < 100) {
                threadData[0].w_king_safety[i].update(eta);
            }
            if (i < 8) {
                threadData[0].w_passer[i].update(eta);
            }
            if (i < 15) {
                threadData[0].w_pinned[i].update(eta);
            }
            if (i < 5) {
                threadData[0].w_hanging[i].update(eta);
            }
        }

        share_weights();
    }

    void load_positions(const std::string &path, int count, int start=0) {

        positions.reserve(positions.size() + count);
        fstream newfile;
        newfile.open(path, ios::in);
        Evaluator evaluator{};
        if (newfile.is_open()) {
            string tp;
            int lineCount = 0;
            int posCount = 0;
            while (getline(newfile, tp)) {

                if (lineCount < start) {
                    lineCount++;
                    continue;
                }

                // finding the first "c" to check where the fen ended
                auto firstC = tp.find_first_of('c');
                auto lastC = tp.find_last_of('c');
                if (firstC == string::npos || lastC == string::npos) {
                    continue;
                }

                // extracting the fen and result and removing bad characters.
                string fen = tp.substr(0, firstC);
                string res = tp.substr(lastC + 2, string::npos);

                fen = trim(fen);
                res = findAndReplaceAll(res, "\"", "");
                res = findAndReplaceAll(res, ";", "");
                res = trim(res);

                Board b{fen};
                Score originalEval = evaluator.evaluate(&b);
                TrainEntry new_entry{&b, &evaluator.evalData, 0};
                Score newEval = new_entry.posEvalData.evaluate();
                if ((int) std::abs(newEval - originalEval) > 3){
                    std::cout << fen << std::endl;
                    std::cout << originalEval << std::endl;
                    std::cout << newEval << std::endl;
                    exit(-1);
                }

                // parsing the result to a usable value:
                // assuming that the result is given as : a-b
                if (res.find('-') != string::npos) {
                    if (res == "1/2-1/2") {
                        new_entry.target = 0.5;
                    } else if (res == "1-0") {
                        new_entry.target = 1;
                    } else if (res == "0-1") {
                        new_entry.target = 0;
                    } else {
                        continue;
                    }
                }
                    // trying to read the result as a decimal
                else {
                    try {
                        double actualResult = stod(res);
                        new_entry.target = actualResult;
                    } catch (std::invalid_argument &e) { continue; }
                }

                positions.push_back(new_entry);

                lineCount++;
                posCount++;

                if (posCount % 10000 == 0) {

                    std::cout << "\r" << loadingBar(posCount, count, "Loading data") << std::flush;
                }

                if (posCount >= count)
                    break;
            }

            std::cout << std::endl;
            newfile.close();
        }
    }

    float compute_loss(float K) {
        double sum = 0;
#pragma omp parallel for schedule(static) num_threads(N_THREAD) reduction(+: sum)
        for (int i = 0; i < positions.size(); i++) {
            sum += positions[i].posEvalData.train(positions[i].target, K, omp_get_thread_num());
        }
        return sum / positions.size();
        return 0;
    }

    float compute_error(float K) {
        double sum = 0;
#pragma omp parallel for schedule(static) num_threads(N_THREAD) reduction(+: sum)
        for (int i = 0; i < positions.size(); i++) {
//        const int threadID = omp_get_thread_num();
            sum += positions[i].posEvalData.error(positions[i].target, K);
        }
        return sum / positions.size();
    }

    void train(int iterations, float K, float eta) {
        for (int i = 0; i < iterations; i++) {
            startMeasure();
            std::cout << left;
            std::cout << "loss= " << setw(20) << compute_loss(K)
                      << " eps= " << setw(20) << positions.size() / stopMeasure() * 1000 << std::endl;
            adjust_weights(eta);
        }
    }

    float compute_K(double initK, double rate, double deviation) {

        double K = initK;
        double dK = 0.01;
        double dEdK = 1;

        while (abs(dEdK) > deviation) {

            double Epdk = compute_error(K + dK);
            double Emdk = compute_error(K - dK);

            dEdK = (Epdk - Emdk) / (2 * dK);

            std::cout << "K:" << K << " Error: " << (Epdk + Emdk) / 2 << " dev: " << abs(dEdK) << std::endl;

            K -= dEdK * rate;
        }

        return K;
    }

    void display_params() {

        // --------------------------------- features ---------------------------------
        const static std::string feature_names[]{
                "SIDE_TO_MOVE",
                "PAWN_STRUCTURE",
                "PAWN_PASSED_AND_DOUBLED",
                "PAWN_PASSED_AND_BLOCKED",
                "PAWN_PASSED_COVERED_PROMO",
                "PAWN_PASSED_HELPER",
                "PAWN_PASSED_AND_DEFENDED",
                "PAWN_PASSED_SQUARE_RULE",
                "PAWN_CANDIDATE_PASSER",
                "PAWN_ISOLATED",
                "PAWN_DOUBLED",
                "PAWN_DOUBLED_AND_ISOLATED",
                "PAWN_BACKWARD",
                "PAWN_OPEN",
                "PAWN_BLOCKED",
                "KNIGHT_OUTPOST",
                "KNIGHT_DISTANCE_ENEMY_KING",
                "ROOK_OPEN_FILE",
                "ROOK_HALF_OPEN_FILE",
                "ROOK_KING_LINE",
                "BISHOP_DOUBLED",
                "BISHOP_FIANCHETTO",
                "BISHOP_PIECE_SAME_SQUARE_E",
                "QUEEN_DISTANCE_ENEMY_KING",
                "KING_CLOSE_OPPONENT",
                "KING_PAWN_SHIELD",
                "CASTLING_RIGHTS",
                "MINOR_BEHIND_PAWN",
                "SAFE_QUEEN_CHECK",
                "SAFE_ROOK_CHECK",
                "SAFE_BISHOP_CHECK",
                "SAFE_KNIGHT_CHECK",
                "PAWN_ATTACK_MINOR",
                "PAWN_ATTACK_ROOK",
                "PAWN_ATTACK_QUEEN",
                "MINOR_ATTACK_ROOK",
                "MINOR_ATTACK_QUEEN",
                "ROOK_ATTACK_QUEEN",
        };

        for (int i = 0; i < I_END; i++) {
            std::cout << "EvalScore " << left << setw(30) << feature_names[i] << right << "= ";
            std::cout << threadData[0].w_features[i] << ";" << std::endl;
        }
        std::cout << std::endl;

        // --------------------------------- mobility ---------------------------------

        const static std::string mobility_names[]{
                "EvalScore mobilityKnight[9] = {",
                "EvalScore mobilityBishop[14] = {",
                "EvalScore mobilityRook[15] = {",
                "EvalScore mobilityQueen[28] = {",};

        for (int i = 0; i < 4; i++) {
            std::cout << mobility_names[i];
            for (int n = 0; n < mobEntryCount[i + 1]; n++) {
                if (n % 5 == 0) std::cout << std::endl << "\t";
                std::cout << threadData[0].w_mobility[i + 1][n] << ", ";
            }
            std::cout << "};\n" << std::endl;
        }

        // --------------------------------- hanging ---------------------------------
        std::cout << "EvalScore hangingEval[5] = {";
        for (int n = 0; n < 5; n++) {
            if (n % 5 == 0) std::cout << std::endl << "\t";
            std::cout << threadData[0].w_hanging[n] << ", ";
        }
        std::cout << "};\n" << std::endl;

        // --------------------------------- pinned ---------------------------------
        std::cout << "EvalScore pinnedEval[15] = {";
        for (int n = 0; n < 15; n++) {
            if (n % 5 == 0) std::cout << std::endl << "\t";
            std::cout << threadData[0].w_pinned[n] << ", ";
        }
        std::cout << "};\n" << std::endl;

        // --------------------------------- passer_rank_n ---------------------------------
        std::cout << "EvalScore passer_rank_n[N_RANKS] = {";
        for (int n = 0; n < 8; n++) {
            if (n % 4 == 0) std::cout << std::endl << "\t";
            std::cout << threadData[0].w_passer[n] << ", ";
        }
        std::cout << "};\n" << std::endl;

        // --------------------------------- bishop_pawn_same_color_table_o ---------------------------------
        std::cout << "EvalScore bishop_pawn_same_color_table_o[9] = {";
        for (int n = 0; n < 9; n++) {
            if (n % 3 == 0) std::cout << std::endl << "\t";
            std::cout << threadData[0].w_bishop_pawn_o[n] << ", ";
        }
        std::cout << "};\n" << std::endl;

        // --------------------------------- bishop_pawn_same_color_table_o ---------------------------------
        std::cout << "EvalScore bishop_pawn_same_color_table_e[9] = {";
        for (int n = 0; n < 9; n++) {
            if (n % 3 == 0) std::cout << std::endl << "\t";
            std::cout << threadData[0].w_bishop_pawn_e[n] << ", ";
        }
        std::cout << "};\n" << std::endl;

        // --------------------------------- kingSafetyTable ---------------------------------
        std::cout << "EvalScore kingSafetyTable[100] = {";
        for (int n = 0; n < 100; n++) {
            if (n % 5 == 0) std::cout << std::endl << "\t";
            std::cout << threadData[0].w_king_safety[n] << ", ";
        }
        std::cout << "};\n" << std::endl;

        // --------------------------------- piece_square_table ---------------------------------

        std::cout << "EvalScore piece_square_table[6][2][64]{\n";
        for (Piece p = 0; p < 6; p++) {
            std::cout << "\t{\n";

            for (int i = 0; i < 2; i++) {
                std::cout << "\t\t{\n";
                for (int n = 0; n < 64; n++) {
                    if (n % 4 == 0) std::cout << std::endl << "\t\t\t";
                    Weight temp = threadData[0].w_piece_square_table[p][i][n];
                    temp.midgame.value -= MgScore(piece_values[p]);
                    temp.endgame.value -= EgScore(piece_values[p]);
                    std::cout << temp << ", ";
                }
                std::cout << "\n\t\t},\n";
            }
            std::cout << "\t},\n";
        }
        std::cout << "};" << std::endl;


        // --------------------------------- piece_our_king_square_table ---------------------------------
        std::cout << "EvalScore piece_our_king_square_table[5][15*15]{\n";
        for (Piece p = 0; p < 5; p++) {
            std::cout << "\t{";
            for (int n = 0; n < 15 * 15; n++) {
                if (n % 5 == 0) std::cout << std::endl << "\t\t";
                std::cout << threadData[0].w_piece_our_king_square_table[p][n] << ", ";
            }

            std::cout << "\n\t},\n";
        }
        std::cout << "};" << std::endl;

        // --------------------------------- piece_opp_king_square_table ---------------------------------
        std::cout << "EvalScore piece_opp_king_square_table[5][15*15]{\n";
        for (Piece p = 0; p < 5; p++) {
            std::cout << "\t{";
            for (int n = 0; n < 15 * 15; n++) {
                if (n % 5 == 0) std::cout << std::endl << "\t\t";
                std::cout << threadData[0].w_piece_opp_king_square_table[p][n] << ", ";
            }

            std::cout << "\n\t},\n";
        }
        std::cout << "};" << std::endl;
    }

}
#endif

#endif    // KOIVISTO_GRADIENT_H
