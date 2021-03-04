//
// Created by finne on 06.01.2021.
//

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
#define TUNING
#ifdef TUNING
#define N_THREAD 24

namespace tuning {

    inline double sigmoid(double s, double K) { return (double) 1 / (1 + exp(-K * s / 400)); }

    inline double sigmoidPrime(double s, double K) {
        double ex = exp(-s * K / 400);
        return (K * ex) / (400 * (ex + 1) * (ex + 1));
    }

    enum feature_indices {
        I_SIDE_TO_MOVE,

        I_PAWN_STRUCTURE,
        I_PAWN_PASSED,
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

        // always have this at the end
        I_END,
    };

    struct Param {
        Param() {}

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
    };

    struct ThreadData {
        Weight w_piece_square_table[6][2][64]{};
        Weight w_piece_opp_king_square_table[5][15 * 15]{};
        Weight w_piece_our_king_square_table[5][15 * 15]{};
        Weight w_mobility[5][28]{};
        Weight w_features[I_END]{};
        Weight w_bishop_pawn_e[9]{};
        Weight w_bishop_pawn_o[9]{};
        Weight w_passer[16]{};
        Weight w_pinned[15]{};
        Weight w_hanging[5]{};
        
        // only contain 1 value
        Weight w_attack_weights[6];
        Weight w_king_danger_factors[KING_DANGER_FACTORS_COUNT];
    };

    ThreadData threadData[N_THREAD]{};

    struct MetaData {
        // it can happen that the final evaluation is reduced by a given scalar

        float evalReduction = 1;
        float phase = 0;
        float matingMaterialWhite = false;
        float matingMaterialBlack = false;

        void init(Board *b) {
            phase =
                    (24.0f + phaseValues[5] -
                     phaseValues[0] * bitCount(b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN])
                     - phaseValues[1] * bitCount(b->getPieces()[WHITE_KNIGHT] | b->getPieces()[BLACK_KNIGHT])
                     - phaseValues[2] * bitCount(b->getPieces()[WHITE_BISHOP] | b->getPieces()[BLACK_BISHOP])
                     - phaseValues[3] * bitCount(b->getPieces()[WHITE_ROOK] | b->getPieces()[BLACK_ROOK])
                     - phaseValues[4] * bitCount(b->getPieces()[WHITE_QUEEN] | b->getPieces()[BLACK_QUEEN]))
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

        void init(Board *b) {

            bool wKSide = (fileIndex(bitscanForward(b->getPieces(WHITE, KING))) > 3 ? 0 : 1);
            bool bKSide = (fileIndex(bitscanForward(b->getPieces(BLACK, KING))) > 3 ? 0 : 1);
            sameside_castle = wKSide == bKSide;

            for (Piece p = PAWN; p <= KING; p++) {
                for (Color c = WHITE; c <= BLACK; c++) {
                    U64 k = b->getPieces(c, p);
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
        uint8_t indices_white_wk[1][10]{};
        uint8_t indices_black_wk[1][10]{};
        uint8_t indices_white_bk[1][10]{};
        uint8_t indices_black_bk[1][10]{};

        void init(Board *b) {

            Square wKingSq = bitscanForward(b->getPieces(WHITE, KING));
            Square bKingSq = bitscanForward(b->getPieces(BLACK, KING));

            for (Piece p = PAWN; p <= PAWN; p++) {
                for (Color c = WHITE; c <= BLACK; c++) {
                    U64 k = b->getPieces(c, p);
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
            for (Piece p = PAWN; p <= PAWN; p++) {

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
            for (Piece p = PAWN; p <= PAWN; p++) {
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
        int  king_danger_counts[N_COLOR][KING_DANGER_FACTORS_COUNT]{};
        uint8_t attack_counts  [N_COLOR][N_PIECE]{};
        int  king_dangers_last [N_COLOR];
        
        void init(Board *b) {

            U64 k;
            Square square;
            U64 attacks;
            U64 occupied = *(b->getOccupied());
            
            Square whiteKingSquare = bitscanForward(b->getPieces(WHITE, KING));
            Square blackKingSquare = bitscanForward(b->getPieces(BLACK, KING));
            
            U64 whiteKingZone = KING_ATTACKS[whiteKingSquare];
            U64 blackKingZone = KING_ATTACKS[blackKingSquare];
            if(fileIndex(whiteKingSquare) == 0) whiteKingZone |= shiftEast(whiteKingZone);
            if(fileIndex(whiteKingSquare) == 7) whiteKingZone |= shiftWest(whiteKingZone);
            if(fileIndex(blackKingSquare) == 0) blackKingZone |= shiftEast(blackKingZone);
            if(fileIndex(blackKingSquare) == 7) blackKingZone |= shiftWest(blackKingZone);
            
            U64 attacks_table[2][6]{};
            U64 attack_scores[2]{};

            for (Piece p = PAWN; p <= KING; p++) {
                for (Color c = WHITE; c <= BLACK; c++) {
                    k = b->getPieces(c, p);
                    while (k) {
                        square = bitscanForward(k);
                        attacks = ZERO;
                        switch (p) {
                            case PAWN:
                                if(c == WHITE){
                                    attacks = shiftNorthEast((ONE << square)) | shiftNorthWest((ONE << square));
                                }else{
                                    attacks = shiftSouthEast((ONE << square)) | shiftSouthWest((ONE << square));
                                }
                                break;
                            case KNIGHT:
                                attacks = KNIGHT_ATTACKS[square];
                                break;
                            case BISHOP:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieces(c, QUEEN));
                                break;
                            case QUEEN:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieces(c, BISHOP)) |
                                        lookUpRookAttack    (square, occupied &~b->getPieces(c, ROOK));
                                break;
                            case ROOK:
                                attacks =
                                        lookUpRookAttack    (square,occupied &
                                                                    ~b->getPieces(c, QUEEN)&
                                                                    ~b->getPieces(c, ROOK));
                                break;
                            case KING:
                                attacks = KING_ATTACKS[square];
                                break;
                        }
                        attacks_table[c][p] |= attacks;
                        if (c == WHITE) {
                            attack_counts[c][p]  += bitCount(attacks & blackKingZone);
                        } else {
                            attack_counts[c][p]  += bitCount(attacks & whiteKingZone);
                        }

                        k = lsbReset(k);
                    }
                }
            }
            
            king_danger_counts[WHITE][WEAK_KING_RING] =
                bitCount(   (attacks_table[BLACK][PAWN] | attacks_table[BLACK][KNIGHT] | attacks_table[BLACK][BISHOP] |
                             attacks_table[BLACK][ROOK] | attacks_table[BLACK][QUEEN ] | attacks_table[BLACK][KING  ])
                         & ~(attacks_table[WHITE][PAWN] | attacks_table[WHITE][KNIGHT] | attacks_table[WHITE][BISHOP] |
                             attacks_table[WHITE][ROOK])
                         & whiteKingZone);
            king_danger_counts[BLACK][WEAK_KING_RING] =
                bitCount(   (attacks_table[WHITE][PAWN] | attacks_table[WHITE][KNIGHT] | attacks_table[WHITE][BISHOP] |
                             attacks_table[WHITE][ROOK] | attacks_table[WHITE][QUEEN ] | attacks_table[WHITE][KING  ])
                         & ~(attacks_table[BLACK][PAWN] | attacks_table[BLACK][KNIGHT] | attacks_table[BLACK][BISHOP] |
                             attacks_table[BLACK][ROOK])
                         & blackKingZone);
            king_danger_counts[WHITE][QUEEN_EXISTENCE] = b->getPieces(BLACK, QUEEN) != 0;
            king_danger_counts[BLACK][QUEEN_EXISTENCE] = b->getPieces(WHITE, QUEEN) != 0;
            
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            
            int attack_scores[N_COLOR]{};
            for(Piece p = PAWN; p <= KING; p++){
                attack_scores[WHITE] += td->w_attack_weights[p].midgame.value * attack_counts[WHITE][p];
                attack_scores[BLACK] += td->w_attack_weights[p].midgame.value * attack_counts[BLACK][p];
            }
            king_danger_counts[WHITE][ATTACK_COUNT] = attack_scores[WHITE];
            king_danger_counts[BLACK][ATTACK_COUNT] = attack_scores[BLACK];
            
            king_dangers_last[WHITE] = 0;
            king_dangers_last[BLACK] = 0;
            for(Color c = WHITE; c <= BLACK; c++){
                for(int i = 0; i < KING_DANGER_FACTORS_COUNT; i++){
                    king_dangers_last[c] += king_danger_counts[c][i] * td->w_king_danger_factors[i].midgame.value;
                }
            }
        
            midgame += king_dangers_last[WHITE] * king_dangers_last[WHITE] / 4096;
            midgame -= king_dangers_last[BLACK] * king_dangers_last[BLACK] / 4096;
    
            endgame += king_dangers_last[WHITE] / 16;
            endgame -= king_dangers_last[BLACK] / 16;
    
            
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {
            
            double devalddanger_w =
                     meta->phase    * 1 / 16.0 +
                (1 - meta->phase)   * 2.0 / 4096.0 * king_dangers_last[WHITE];
            double devalddanger_b = -
                     meta->phase    * 1 / 16.0 -
                (1 - meta->phase)   * 2.0 / 4096.0 * king_dangers_last[BLACK];
            
            devalddanger_w *= meta->evalReduction * lossgrad;
            devalddanger_b *= meta->evalReduction * lossgrad;
            
            // backtrack gradients to each scalar
            for(int i = 0; i < KING_DANGER_FACTORS_COUNT; i++){
                td->w_king_danger_factors[i].midgame.gradient += devalddanger_w * king_danger_counts[WHITE][i];
                td->w_king_danger_factors[i].midgame.gradient += devalddanger_b * king_danger_counts[BLACK][i];
            }
            
            for(int i = 0; i < 6; i++){
                td->w_attack_weights[i].midgame.gradient += devalddanger_w * attack_counts[WHITE][i];
                td->w_attack_weights[i].midgame.gradient += devalddanger_b * attack_counts[BLACK][i];
            }
            
            
        
//            td->w_king_safety[bkingsafety_index].midgame.gradient +=
//                    (1 - meta->phase) * meta->evalReduction * lossgrad;
//            td->w_king_safety[bkingsafety_index].endgame.gradient +=
//                    (meta->phase) * meta->evalReduction * lossgrad;
//
//            td->w_king_safety[wkingsafety_index].midgame.gradient -=
//                    (1 - meta->phase) * meta->evalReduction * lossgrad;
//            td->w_king_safety[wkingsafety_index].endgame.gradient -=
//                    (meta->phase) * meta->evalReduction * lossgrad;
        }
    };

    struct BishopPawnTableData {
        int8_t count_e[9]{};
        int8_t count_o[9]{};

        void init(Board *b) {
            U64 k;
            Square square;

            U64 whitePawns = b->getPieces(WHITE, PAWN);
            U64 blackPawns = b->getPieces(BLACK, PAWN);

            k = b->getPieces()[WHITE_BISHOP];
            while (k) {
                square = bitscanForward(k);
                count_e[bitCount(
                        blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] += 1;
                count_o[bitCount(
                        whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] += 1;
                k = lsbReset(k);
            }

            k = b->getPieces()[BLACK_BISHOP];
            while (k) {
                square = bitscanForward(k);
                count_e[bitCount(
                        whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] -= 1;
                count_o[bitCount(
                        blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] -= 1;
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

        int8_t count[16]{};

        void init(Board *b) {
            U64 whiteTeam = b->getTeamOccupied()[WHITE];
            U64 blackTeam = b->getTeamOccupied()[BLACK];

            U64 whitePawns = b->getPieces()[WHITE_PAWN];
            U64 blackPawns = b->getPieces()[BLACK_PAWN];

            U64 whitePassers = wPassedPawns(whitePawns, blackPawns);
            U64 blackPassers = bPassedPawns(blackPawns, whitePawns);

            U64 whiteBlockedPawns = shiftNorth(whitePawns) & (whiteTeam | blackTeam);
            U64 blackBlockedPawns = shiftSouth(blackPawns) & (whiteTeam | blackTeam);

            U64 k = whitePassers;
            while (k) {
                Square square = bitscanForward(k);
                count[getBit(whiteBlockedPawns, square) * 8 + rankIndex(square)] += 1;
                k = lsbReset(k);
            }
            k = blackPassers;
            while (k) {
                Square square = bitscanForward(k);
                count[getBit(blackBlockedPawns, square) * 8 + 7 - rankIndex(square)] -= 1;
                k = lsbReset(k);
            }
        }

        void evaluate(float &midgame, float &endgame, ThreadData* td) {
            for (int i = 0; i < 16; i++) {
                midgame += count[i] * td->w_passer[i].midgame.value;
                endgame += count[i] * td->w_passer[i].endgame.value;
            }
        }

        void gradient(MetaData *meta, float lossgrad, ThreadData* td) {

            for (int i = 0; i < 16; i++) {
                td->w_passer[i].midgame.gradient += count[i] * (1 - meta->phase) * meta->evalReduction * lossgrad;
                td->w_passer[i].endgame.gradient += count[i] * (meta->phase) * meta->evalReduction * lossgrad;
            }
        }
    };

    struct FeatureData {
        // we assume that these features are linear which means that we only need their count

        int8_t count[I_END];

        void init(Board *b) {

            U64 k;
            Square square;

            Square whiteKingSquare = bitscanForward(b->getPieces(WHITE, KING));
            Square blackKingSquare = bitscanForward(b->getPieces(BLACK, KING));

            U64 whitePawns = b->getPieces()[WHITE_PAWN];
            U64 blackPawns = b->getPieces()[BLACK_PAWN];

            U64 whiteTeam = b->getTeamOccupied()[WHITE];
            U64 blackTeam = b->getTeamOccupied()[BLACK];

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

            // clang-format off
            count[I_PAWN_DOUBLED_AND_ISOLATED] = (
                    +bitCount(whiteIsolatedPawns & whiteDoubledPawns)
                    - bitCount(blackIsolatedPawns & blackDoubledPawns));
            count[I_PAWN_DOUBLED] += (
                    +bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
                    - bitCount(~blackIsolatedPawns & blackDoubledPawns));
            count[I_PAWN_ISOLATED] += (
                    +bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
                    - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
            count[I_PAWN_PASSED] += (
                    +bitCount(whitePassers)
                    - bitCount(blackPassers));
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
                    +bitCount(shiftNorth(b->getPieces()[WHITE_KNIGHT] | b->getPieces()[WHITE_BISHOP]) &
                              (b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN]))
                    - bitCount(shiftSouth(b->getPieces()[BLACK_KNIGHT] | b->getPieces()[BLACK_BISHOP]) &
                               (b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN])));

            k = b->getPieces()[WHITE_KNIGHT];
            while (k) {
                square = bitscanForward(k);
                count[I_KNIGHT_OUTPOST] += isOutpost(square, WHITE, blackPawns, whitePawnCover);
                count[I_KNIGHT_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
                k = lsbReset(k);
            }

            k = b->getPieces()[BLACK_KNIGHT];
            while (k) {
                square = bitscanForward(k);
                count[I_KNIGHT_OUTPOST] -= isOutpost(square, BLACK, whitePawns, blackPawnCover);
                count[I_KNIGHT_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
                k = lsbReset(k);
            }


            k = b->getPieces()[WHITE_BISHOP];
            while (k) {
                square = bitscanForward(k);

                count[I_BISHOP_PIECE_SAME_SQUARE_E] +=
                        bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
                count[I_BISHOP_FIANCHETTO] +=
                        (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                         && whitePawns & (ONE << G3 | ONE << G4));
                count[I_BISHOP_FIANCHETTO] +=
                        (square == B2 && whitePawns & ONE << A2 && whitePawns & ONE << C2
                         && whitePawns & (ONE << B3 | ONE << B4));
                k = lsbReset(k);
            }

            k = b->getPieces()[BLACK_BISHOP];
            while (k) {
                square = bitscanForward(k);
                count[I_BISHOP_PIECE_SAME_SQUARE_E] -=
                        bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
                count[I_BISHOP_FIANCHETTO] -=
                        (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                         && blackPawns & (ONE << G6 | ONE << G5));
                count[I_BISHOP_FIANCHETTO] -=
                        (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                         && blackPawns & (ONE << B6 | ONE << B5));
                k = lsbReset(k);
            }
            count[I_BISHOP_DOUBLED] += (
                    +(bitCount(b->getPieces()[WHITE_BISHOP]) == 2)
                    - (bitCount(b->getPieces()[BLACK_BISHOP]) == 2));


            count[I_ROOK_KING_LINE] += (
                    +bitCount(lookUpRookAttack(blackKingSquare, *b->getOccupied()) & b->getPieces(WHITE, ROOK))
                    - bitCount(lookUpRookAttack(whiteKingSquare, *b->getOccupied()) & b->getPieces(BLACK, ROOK)));
            count[I_ROOK_OPEN_FILE] += (
                    +bitCount(openFiles & b->getPieces(WHITE, ROOK))
                    - bitCount(openFiles & b->getPieces(BLACK, ROOK)));
            count[I_ROOK_HALF_OPEN_FILE] += (
                    +bitCount(openFilesBlack & ~openFiles & b->getPieces(WHITE, ROOK))
                    - bitCount(openFilesWhite & ~openFiles & b->getPieces(BLACK, ROOK)));


            k = b->getPieces()[WHITE_QUEEN];
            while (k) {
                square = bitscanForward(k);
                count[I_QUEEN_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
                k = lsbReset(k);
            }

            k = b->getPieces()[BLACK_QUEEN];
            while (k) {
                square = bitscanForward(k);
                count[I_QUEEN_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
                k = lsbReset(k);
            }

            k = b->getPieces()[WHITE_KING];
            while (k) {
                square = bitscanForward(k);
                count[I_KING_PAWN_SHIELD] += bitCount(KING_ATTACKS[square] & whitePawns);
                count[I_KING_CLOSE_OPPONENT] += bitCount(KING_ATTACKS[square] & blackTeam);
                k = lsbReset(k);
            }

            k = b->getPieces()[BLACK_KING];
            while (k) {
                square = bitscanForward(k);
                count[I_KING_PAWN_SHIELD] -= bitCount(KING_ATTACKS[square] & blackPawns);
                count[I_KING_CLOSE_OPPONENT] -= bitCount(KING_ATTACKS[square] & whiteTeam);
                k = lsbReset(k);
            }

            count[I_CASTLING_RIGHTS] += (
                    +b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
                    + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
                    - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
                    - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
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

        void init(Board *b) {


            for (Color color = WHITE; color <= BLACK; color++) {

                Color us = color;
                Color them = 1 - color;

                // figure out where the opponent has pieces
                U64 opponentOcc = b->getTeamOccupied()[them];
                U64 ourOcc = b->getTeamOccupied()[us];

                // get the pieces which can pin our king
                U64 bishops = b->getPieces(them, BISHOP);
                U64 rooks = b->getPieces(them, ROOK);
                U64 queens = b->getPieces(them, QUEEN);

                // get the king positions
                Square kingSq = bitscanForward(b->getPieces(us, KING));

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
                        pinnerPiece -= 6;
                    } else {
                        pinnedPiece -= 6;
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

        void init(Board *b) {
            U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
            U64 BnotAttacked = ~b->getAttackedSquares(BLACK);

            for (int i = PAWN; i <= QUEEN; i++) {
                count[i] =
                        +bitCount(b->getPieces(WHITE, i) & WnotAttacked)
                        - bitCount(b->getPieces(BLACK, i) & BnotAttacked);
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

        void init(Board *b) {

            U64 k, attacks;
            Square square;

            U64 whitePawnCover = shiftNorthEast(b->getPieces(WHITE, PAWN)) | shiftNorthWest(b->getPieces(WHITE, PAWN));
            U64 blackPawnCover = shiftSouthEast(b->getPieces(BLACK, PAWN)) | shiftSouthWest(b->getPieces(BLACK, PAWN));

            U64 mobilitySquaresWhite = ~(b->getTeamOccupied()[WHITE]) & ~(blackPawnCover);
            U64 mobilitySquaresBlack = ~(b->getTeamOccupied()[BLACK]) & ~(whitePawnCover);

            U64 occupied = *b->getOccupied();

            for (Piece p = KNIGHT; p <= QUEEN; p++) {
                for (Color c = WHITE; c <= BLACK; c++) {
                    k = b->getPieces(c, p);
                    while (k) {
                        square = bitscanForward(k);
                        attacks = ZERO;
                        switch (p) {
                            case KNIGHT:
                                attacks = KNIGHT_ATTACKS[square];
                                break;
                            case BISHOP:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieces(c, QUEEN));
                                break;
                            case QUEEN:
                                attacks =
                                        lookUpBishopAttack  (square, occupied &~b->getPieces(c, BISHOP)) |
                                        lookUpRookAttack    (square, occupied &~b->getPieces(c, ROOK));
                                break;
                            case ROOK:
                                attacks =
                                        lookUpRookAttack    (square,occupied &
                                                ~b->getPieces(c, QUEEN)&
                                                ~b->getPieces(c, ROOK));
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

    struct EvalData {
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

        void init(Board *b) {
            features.init(b);
            mobility.init(b);
            hanging.init(b);
            pinned.init(b);
            passed.init(b);
            bishop_pawn.init(b);
            king_safety.init(b);
            pst64.init(b);
            pst225.init(b);
            meta.init(b);
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
//            features.gradient(&meta, lossgrad, &threadData[threadID]);
//            mobility.gradient(&meta, lossgrad, &threadData[threadID]);
//            hanging.gradient(&meta, lossgrad, &threadData[threadID]);
//            pinned.gradient(&meta, lossgrad, &threadData[threadID]);
//            passed.gradient(&meta, lossgrad, &threadData[threadID]);
//            bishop_pawn.gradient(&meta, lossgrad, &threadData[threadID]);
            king_safety.gradient(&meta, lossgrad, &threadData[threadID]);
//            pst64.gradient(&meta, lossgrad, &threadData[threadID]);
//            pst225.gradient(&meta, lossgrad, &threadData[threadID]);
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
        EvalData evalData;
        double target;

        TrainEntry(Board *b, float target) {
            this->evalData.init(b);
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
                        float w1 = MgScore(piece_opp_king_square_table[i][n]);
                        float w2 = EgScore(piece_opp_king_square_table[i][n]);
                        threadData[t].w_piece_opp_king_square_table[i][n] = {{w1},
                                                                             {w2}};

                        w1 = MgScore(piece_our_king_square_table[i][n]);
                        w2 = EgScore(piece_our_king_square_table[i][n]);
                        threadData[t].w_piece_our_king_square_table[i][n] = {{w1},
                                                                             {w2}};
                    }

                    for (int n = 0; n < mobEntryCount[i]; n++) {
                        float w1 = MgScore(mobilities[i][n]);
                        float w2 = EgScore(mobilities[i][n]);
                        threadData[t].w_mobility[i][n] = {{w1},
                                                          {w2}};
                    }

                }
            }
            for (int i = 0; i < 1000; i++) {
                if (i < I_END) {
                    float w1 = MgScore(*evfeatures[i]);
                    float w2 = EgScore(*evfeatures[i]);
                    threadData[t].w_features[i] = {{w1},
                                                   {w2}};
                }
                if (i < 9) {
                    float w1 = MgScore(bishop_pawn_same_color_table_e[i]);
                    float w2 = EgScore(bishop_pawn_same_color_table_e[i]);
                    threadData[t].w_bishop_pawn_e[i] = {{w1},
                                                        {w2}};
                }
                if (i < 9) {
                    float w1 = MgScore(bishop_pawn_same_color_table_o[i]);
                    float w2 = EgScore(bishop_pawn_same_color_table_o[i]);
                    threadData[t].w_bishop_pawn_o[i] = {{w1},
                                                        {w2}};
                }

                if (i < 16) {
                    float w1 = MgScore(passer_rank_n[i]);
                    float w2 = EgScore(passer_rank_n[i]);
                    threadData[t].w_passer[i] = {{w1},
                                                 {w2}};
                }
                if (i < 15) {
                    float w1 = MgScore(pinnedEval[i]);
                    float w2 = EgScore(pinnedEval[i]);
                    threadData[t].w_pinned[i] = {{w1},
                                                 {w2}};
                }
                if (i < 5) {
                    float w1 = MgScore(hangingEval[i]);
                    float w2 = EgScore(hangingEval[i]);
                    threadData[t].w_hanging[i] = {{w1},
                                                  {w2}};
                }
                if (i < KING_DANGER_FACTORS_COUNT){
                    float w1 = kingDangerFactors[i];
                    threadData[t].w_king_danger_factors[i] = {w1,0};
                }
                if (i < 6){
                    float w1 = kingSafetyAttackWeights[i];
                    threadData[t].w_attack_weights[i] = {w1,0};
                }
            }
        }

    }

    void merge_gradients() {
        for (int t = 1; t < N_THREAD; t++) {
            for (int i = 0; i < 6; i++) {

                for (int n = 0; n < 2; n++) {
                    for (int j = 0; j < 64; j++) {
                        threadData[0].w_piece_square_table[i][n][j].midgame.gradient +=
                                threadData[t].w_piece_square_table[i][n][j].midgame.gradient;
                        threadData[0].w_piece_square_table[i][n][j].endgame.gradient +=
                                threadData[t].w_piece_square_table[i][n][j].endgame.gradient;

                        threadData[t].w_piece_square_table[i][n][j].midgame.gradient = 0;
                        threadData[t].w_piece_square_table[i][n][j].endgame.gradient = 0;
                    }
                }

                if (i < 5) {

                    for (int n = 0; n < 15 * 15; n++) {
                        threadData[0].w_piece_opp_king_square_table[i][n].midgame.gradient +=
                                threadData[t].w_piece_opp_king_square_table[i][n].midgame.gradient;
                        threadData[0].w_piece_opp_king_square_table[i][n].endgame.gradient +=
                                threadData[t].w_piece_opp_king_square_table[i][n].endgame.gradient;

                        threadData[t].w_piece_opp_king_square_table[i][n].midgame.gradient = 0;
                        threadData[t].w_piece_opp_king_square_table[i][n].endgame.gradient = 0;
                    }

                    for (int n = 0; n < mobEntryCount[i]; n++) {
                        threadData[0].w_mobility[i][n].midgame.gradient +=
                                threadData[t].w_mobility[i][n].midgame.gradient;
                        threadData[0].w_mobility[i][n].endgame.gradient +=
                                threadData[t].w_mobility[i][n].endgame.gradient;
                        threadData[t].w_mobility[i][n].midgame.gradient = 0;
                        threadData[t].w_mobility[i][n].endgame.gradient = 0;
                    }

                }
            }
            for (int i = 0; i < 1000; i++) {
                if (i < I_END) {
                    threadData[0].w_features[i].midgame.gradient +=
                            threadData[t].w_features[i].midgame.gradient;
                    threadData[0].w_features[i].endgame.gradient +=
                            threadData[t].w_features[i].endgame.gradient;
                    threadData[t].w_features[i].midgame.gradient = 0;
                    threadData[t].w_features[i].endgame.gradient = 0;
                }
                if (i < 9) {
                    threadData[0].w_bishop_pawn_e[i].midgame.gradient +=
                            threadData[t].w_bishop_pawn_e[i].midgame.gradient;
                    threadData[0].w_bishop_pawn_e[i].endgame.gradient +=
                            threadData[t].w_bishop_pawn_e[i].endgame.gradient;
                    threadData[t].w_bishop_pawn_e[i].midgame.gradient = 0;
                    threadData[t].w_bishop_pawn_e[i].endgame.gradient = 0;
                }
                if (i < 9) {

                    threadData[0].w_bishop_pawn_o[i].midgame.gradient +=
                            threadData[t].w_bishop_pawn_o[i].midgame.gradient;
                    threadData[0].w_bishop_pawn_o[i].endgame.gradient +=
                            threadData[t].w_bishop_pawn_o[i].endgame.gradient;
                    threadData[t].w_bishop_pawn_o[i].midgame.gradient = 0;
                    threadData[t].w_bishop_pawn_o[i].endgame.gradient = 0;

                }
                if (i < 16) {
                    threadData[0].w_passer[i].midgame.gradient +=
                            threadData[t].w_passer[i].midgame.gradient;
                    threadData[0].w_passer[i].endgame.gradient +=
                            threadData[t].w_passer[i].endgame.gradient;
                    threadData[t].w_passer[i].midgame.gradient = 0;
                    threadData[t].w_passer[i].endgame.gradient = 0;

                }
                if (i < 15) {

                    threadData[0].w_pinned[i].midgame.gradient +=
                            threadData[t].w_pinned[i].midgame.gradient;
                    threadData[0].w_pinned[i].endgame.gradient +=
                            threadData[t].w_pinned[i].endgame.gradient;
                    threadData[t].w_pinned[i].midgame.gradient = 0;
                    threadData[t].w_pinned[i].endgame.gradient = 0;

                }
                if (i < 5) {
                    threadData[0].w_hanging[i].midgame.gradient +=
                            threadData[t].w_hanging[i].midgame.gradient;
                    threadData[0].w_hanging[i].endgame.gradient +=
                            threadData[t].w_hanging[i].endgame.gradient;
                    threadData[t].w_hanging[i].midgame.gradient = 0;
                    threadData[t].w_hanging[i].endgame.gradient = 0;
                }
                if (i < KING_DANGER_FACTORS_COUNT) {
                    threadData[0].w_king_danger_factors[i].midgame.gradient +=
                        threadData[t].w_king_danger_factors[i].midgame.gradient;
                    threadData[t].w_king_danger_factors[i].midgame.gradient = 0;
                }
                if (i < 6) {
                    threadData[0].w_attack_weights[i].midgame.gradient +=
                        threadData[t].w_attack_weights[i].midgame.gradient;
                    threadData[t].w_attack_weights[i].midgame.gradient = 0;
                }
            }
        }
    }

    void share_weights() {
        for (int t = 1; t < N_THREAD; t++) {
            for (int i = 0; i < 6; i++) {

                for (int n = 0; n < 2; n++) {
                    for (int j = 0; j < 64; j++) {
                        threadData[t].w_piece_square_table[i][n][j].midgame.value =
                                threadData[0].w_piece_square_table[i][n][j].midgame.value;
                        threadData[t].w_piece_square_table[i][n][j].endgame.value =
                                threadData[0].w_piece_square_table[i][n][j].endgame.value;

                    }
                }

                if (i < 5) {

                    for (int n = 0; n < 15 * 15; n++) {
                        threadData[t].w_piece_opp_king_square_table[i][n].midgame.value =
                                threadData[0].w_piece_opp_king_square_table[i][n].midgame.value;
                        threadData[t].w_piece_opp_king_square_table[i][n].endgame.value =
                                threadData[0].w_piece_opp_king_square_table[i][n].endgame.value;

                    }

                    for (int n = 0; n < mobEntryCount[i]; n++) {
                        threadData[t].w_mobility[i][n].midgame.value =
                                threadData[0].w_mobility[i][n].midgame.value;
                        threadData[t].w_mobility[i][n].endgame.value =
                                threadData[0].w_mobility[i][n].endgame.value;
                    }

                }
            }
            for (int i = 0; i < 1000; i++) {
                if (i < I_END) {
                    threadData[t].w_features[i].midgame.value =
                            threadData[0].w_features[i].midgame.value;
                    threadData[t].w_features[i].endgame.value =
                            threadData[0].w_features[i].endgame.value;
                }
                if (i < 9) {
                    threadData[t].w_bishop_pawn_e[i].midgame.value =
                            threadData[0].w_bishop_pawn_e[i].midgame.value;
                    threadData[t].w_bishop_pawn_e[i].endgame.value =
                            threadData[0].w_bishop_pawn_e[i].endgame.value;
                }
                if (i < 9) {

                    threadData[t].w_bishop_pawn_o[i].midgame.value =
                            threadData[0].w_bishop_pawn_o[i].midgame.value;
                    threadData[t].w_bishop_pawn_o[i].endgame.value =
                            threadData[0].w_bishop_pawn_o[i].endgame.value;

                }

                if (i < 16) {
                    threadData[t].w_passer[i].midgame.value =
                            threadData[0].w_passer[i].midgame.value;
                    threadData[t].w_passer[i].endgame.value =
                            threadData[0].w_passer[i].endgame.value;

                }
                if (i < 15) {

                    threadData[t].w_pinned[i].midgame.value =
                            threadData[0].w_pinned[i].midgame.value;
                    threadData[t].w_pinned[i].endgame.value =
                            threadData[0].w_pinned[i].endgame.value;

                }
                if (i < 5) {

                    threadData[t].w_hanging[i].midgame.value =
                            threadData[0].w_hanging[i].midgame.value;
                    threadData[t].w_hanging[i].endgame.value =
                            threadData[0].w_hanging[i].endgame.value;

                }
    
                if (i < KING_DANGER_FACTORS_COUNT) {
                    threadData[t].w_king_danger_factors[i].midgame.value =
                        threadData[0].w_king_danger_factors[i].midgame.value;
                }
                if (i < 6) {
                    threadData[t].w_attack_weights[i].midgame.value =
                        threadData[0].w_attack_weights[i].midgame.value;
                }

            }
        }
    }

    void adjust_weights(float eta) {

        merge_gradients();

        for (int i = 0; i < 6; i++) {

            for (int n = 0; n < 2; n++) {
                for (int j = 0; j < 64; j++) {
                    threadData[0].w_piece_square_table[i][n][j].midgame.update(eta);
                    threadData[0].w_piece_square_table[i][n][j].endgame.update(eta);
                }
            }

            if (i < 5) {

                if (i < 1) {
                    for (int n = 0; n < 15 * 15; n++) {
                        threadData[0].w_piece_opp_king_square_table[i][n].midgame.update(eta);
                        threadData[0].w_piece_opp_king_square_table[i][n].endgame.update(eta);

                        threadData[0].w_piece_our_king_square_table[i][n].midgame.update(eta);
                        threadData[0].w_piece_our_king_square_table[i][n].endgame.update(eta);
                    }
                }


                for (int n = 0; n < mobEntryCount[i]; n++) {
                    threadData[0].w_mobility[i][n].midgame.update(eta);
                    threadData[0].w_mobility[i][n].endgame.update(eta);
                }

            }
        }
        for (int i = 0; i < 1000; i++) {
            if (i < I_END) {
                threadData[0].w_features[i].midgame.update(eta);
                threadData[0].w_features[i].endgame.update(eta);
            }
            if (i < 9) {
                threadData[0].w_bishop_pawn_e[i].midgame.update(eta);
                threadData[0].w_bishop_pawn_e[i].endgame.update(eta);
            }
            if (i < 9) {
                threadData[0].w_bishop_pawn_o[i].midgame.update(eta);
                threadData[0].w_bishop_pawn_o[i].endgame.update(eta);
            }
            if (i < 16) {
                threadData[0].w_passer[i].midgame.update(eta);
                threadData[0].w_passer[i].endgame.update(eta);
            }
            if (i < 15) {
                threadData[0].w_pinned[i].midgame.update(eta);
                threadData[0].w_pinned[i].endgame.update(eta);
            }
            if (i < 5) {
                threadData[0].w_hanging[i].midgame.update(eta);
                threadData[0].w_hanging[i].endgame.update(eta);
            }
            if (i < KING_DANGER_FACTORS_COUNT){
//                std::cout << threadData[0].w_king_danger_factors[i].midgame.gradient << std::endl;
                threadData[0].w_king_danger_factors[i].midgame.update(eta);
            }
            if (i < 6){
                threadData[0].w_attack_weights[i].midgame.update(eta);
            }
        }

        share_weights();
    }

    void load_positions(const std::string &path, int count, int start=0) {

        positions.reserve(30000000);
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
                TrainEntry new_entry{&b, 0};
                if ((int) (new_entry.evalData.evaluate()) != evaluator.evaluate(&b)) {
                    std::cout << fen << std::endl;
                    std::cout << new_entry.evalData.evaluate() << std::endl;
                    std::cout << evaluator.evaluate(&b) << std::endl;
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
            sum += positions[i].evalData.train(positions[i].target, K, omp_get_thread_num());
        }
        return sum / positions.size();
        return 0;
    }

    float compute_error(float K) {
        double sum = 0;
#pragma omp parallel for schedule(static) num_threads(N_THREAD) reduction(+: sum)
        for (int i = 0; i < positions.size(); i++) {
//        const int threadID = omp_get_thread_num();
            sum += positions[i].evalData.error(positions[i].target, K);
        }
        return sum / positions.size();
    }

    void train(int iterations, float K, float eta) {
        for (int i = 0; i < iterations; i++) {
            startMeasure();
            std::cout << left;
            std::cout << "loss= " << setw(20) << compute_loss(K)
                      << " eps= " << setw(20) << positions.size() / std::max(0.0001,stopMeasure()* 1000.0) << std::endl;
            adjust_weights(eta);
        }
        for(int i = 0; i < 3; i++)
            std::cout << threadData[0].w_king_danger_factors[i].midgame.value << std::endl;
        for(int i = 0; i < 6; i++)
            std::cout << threadData[0].w_attack_weights[i].midgame.value << std::endl;
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
                "PAWN_PASSED",
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
                "MINOR_BEHIND_PAWN",};

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
        std::cout << "EvalScore passer_rank_n[16] = {";
        for (int n = 0; n < 16; n++) {
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
