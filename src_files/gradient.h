//
// Created by finne on 06.01.2021.
//

#ifndef KOIVISTO_GRADIENT_H
#define KOIVISTO_GRADIENT_H

#include "Board.h"

#include <cstdint>
#include <vector>
#include "eval.h"




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

struct param{
    param(){}
    param(float value) : value(value) {}

    float value;
    float gradient;
};

struct weight{
    param midgame;
    param endgame;
};

weight w_piece_square_table[6][2][64];
weight w_piece_opp_king_square_table[5][15*15];
weight w_piece_our_king_square_table[5][15*15];
weight w_mobility[5][28];
weight w_features[I_END];
weight w_bishop_pawn_e[9];
weight w_bishop_pawn_o[9];
weight w_king_safety[100];
weight w_passer[16];
weight w_pinned[15];
weight w_hanging[5];

void load_weights(){
    for(int i = 0; i < 6; i++){

        for(int n = 0; n < 2; n++){
            for(int j =0; j < 64; j++){
                float w1 = MgScore(piece_square_table[i][n][j] + piece_values[i]);
                float w2 = EgScore(piece_square_table[i][n][j] + piece_values[i]);
                w_piece_square_table[i][n][j] = {{w1},{w2}};
            }
        }

        if(i < 5){

            for(int n = 0; n < 15*15; n++){
                float w1 = MgScore(piece_opp_king_square_table[i][n]);
                float w2 = EgScore(piece_opp_king_square_table[i][n]);
                w_piece_opp_king_square_table[i][n] = {{w1},{w2}};

                w1 = MgScore(piece_our_king_square_table[i][n]);
                w2 = EgScore(piece_our_king_square_table[i][n]);
                w_piece_our_king_square_table[i][n] = {{w1},{w2}};
            }

            for(int n = 0; n < mobEntryCount[i]; n++){
                float w1 = MgScore(mobilities[i][n]);
                float w2 = EgScore(mobilities[i][n]);
                w_mobility[i][n] = {{w1},{w2}};
            }

        }
    }
    for(int i = 0; i < 1000; i++){
        if(i < I_END){
            float w1 = MgScore(*evfeatures[i]);
            float w2 = EgScore(*evfeatures[i]);
            w_features[i] = {{w1},{w2}};
        }if(i < 9){
            float w1 = MgScore(bishop_pawn_same_color_table_e[i]);
            float w2 = EgScore(bishop_pawn_same_color_table_e[i]);
            w_bishop_pawn_e[i] = {{w1},{w2}};
        }if(i < 9){
            float w1 = MgScore(bishop_pawn_same_color_table_o[i]);
            float w2 = EgScore(bishop_pawn_same_color_table_o[i]);
            w_bishop_pawn_o[i] = {{w1},{w2}};
        }if(i < 100){
            float w1 = MgScore(kingSafetyTable[i]);
            float w2 = EgScore(kingSafetyTable[i]);
            w_king_safety[i] = {{w1},{w2}};
        }if(i < 16){
            float w1 = MgScore(passer_rank_n[i]);
            float w2 = EgScore(passer_rank_n[i]);
            w_passer[i] = {{w1},{w2}};
        }if(i < 15){
            float w1 = MgScore(pinnedEval[i]);
            float w2 = EgScore(pinnedEval[i]);
            w_pinned[i] = {{w1},{w2}};
        }if(i < 5){
            float w1 = MgScore(hangingEval[i]);
            float w2 = EgScore(hangingEval[i]);
            w_hanging[i] = {{w1},{w2}};
        }
    }
}

void clear_gradients() {
    for(int i = 0; i < 6; i++){

        for(int n = 0; n < 2; n++){
            for(int j =0; j < 64; j++){
                w_piece_square_table[i][n][j].midgame.gradient = 0;
                w_piece_square_table[i][n][j].endgame.gradient = 0;
            }
        }

        if(i < 5){

            for(int n = 0; n < 15*15; n++){
                w_piece_opp_king_square_table[i][n].midgame.gradient = 0;
                w_piece_opp_king_square_table[i][n].endgame.gradient = 0;

                w_piece_our_king_square_table[i][n].midgame.gradient = 0;
                w_piece_our_king_square_table[i][n].endgame.gradient = 0;
            }

            for(int n = 0; n < mobEntryCount[i]; n++){
                w_mobility[i][n].midgame.gradient = 0;
                w_mobility[i][n].endgame.gradient = 0;
            }

        }
    }
    for(int i = 0; i < 1000; i++){
        if(i < I_END){
            w_features[i].midgame.gradient = 0;
            w_features[i].endgame.gradient = 0;
        }if(i < 9){
            w_bishop_pawn_e[i].midgame.gradient = 0;
            w_bishop_pawn_e[i].endgame.gradient = 0;
        }if(i < 9){
            w_bishop_pawn_o[i].midgame.gradient = 0;
            w_bishop_pawn_o[i].endgame.gradient = 0;
        }if(i < 100){
            w_king_safety[i].midgame.gradient = 0;
            w_king_safety[i].endgame.gradient = 0;
        }if(i < 16){
            w_passer[i].midgame.gradient = 0;
            w_passer[i].endgame.gradient = 0;
        }if(i < 15){
            w_pinned[i].midgame.gradient = 0;
            w_pinned[i].endgame.gradient = 0;
        }if(i < 5){
            w_hanging[i].midgame.gradient = 0;
            w_hanging[i].endgame.gradient = 0;
        }
    }
}

struct meta_data{
    // it can happen that the final evaluation is reduced by a given scalar

    float evalReduction = 1;
    float phase = 0;
    float matingMaterialWhite = false;
    float matingMaterialBlack = false;

    meta_data(Board* b) {
        phase = (24.0f + phaseValues[5] - phaseValues[0] * bitCount(b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN])
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

    void evaluate(float &res){
        if (res > 0 ? !matingMaterialWhite : !matingMaterialBlack)
            evalReduction = 1.0 / 10;
        else{
            evalReduction = 1;
        }
        res *= evalReduction;
    }
};

struct pst_data_64{
    // to quickly recompute the values for the piece square tables, we require the indices of each square
    // and how often is being used. We also need to know if we deal with same side or opposite side castling
    bool sameside_castle;

    // we use 8 bits (256 values). Can have 32 pieces at most -> 256 bit = 32byte
    std::vector<int8_t> indices_white[6]{};
    std::vector<int8_t> indices_black[6]{};

    pst_data_64(Board* b) {

        bool   wKSide            = (fileIndex(bitscanForward(b->getPieces(WHITE, KING))) > 3 ? 0 : 1);
        bool   bKSide            = (fileIndex(bitscanForward(b->getPieces(BLACK, KING))) > 3 ? 0 : 1);
               sameside_castle   = wKSide == bKSide;

        for(Piece p = PAWN; p <= KING; p++){
            for(Color c = WHITE; c <= BLACK; c++){
                U64 k = b->getPieces(c, p);
                while(k){
                    Square s = bitscanForward(k);

                    if(p == KING){
                        if(c == WHITE) {
                            indices_white[p].push_back(pst_index_white_s(s));
                        }else{
                            indices_black[p].push_back(pst_index_black_s(s));
                        }
                    }else{
                        if(c == WHITE) {
                            indices_white[p].push_back(pst_index_white(s, wKSide));
                        }else{
                            indices_black[p].push_back(pst_index_black(s, bKSide));
                        }
                    }


                    k = lsbReset(k);
                }
            }
        }
    }

    void evaluate(float& midgame, float& endgame){
        for(Piece p = PAWN; p <= KING; p++){

            if(p == BISHOP){
                for(int8_t w:indices_white[p]){
                    midgame += w_piece_square_table[p][sameside_castle][w].midgame.value;
                    endgame += w_piece_square_table[p][sameside_castle][w].endgame.value;
                }
                for(int8_t b:indices_black[p]){
                    midgame -= w_piece_square_table[p][sameside_castle][b].midgame.value;
                    endgame -= w_piece_square_table[p][sameside_castle][b].endgame.value;
                }
            }else{
                for(int8_t w:indices_white[p]){
                    midgame += w_piece_square_table[p][sameside_castle][w].midgame.value;
                    endgame += w_piece_square_table[p][sameside_castle][w].endgame.value;
                }

                for(int8_t b:indices_black[p]){
                    midgame -= w_piece_square_table[p][sameside_castle][b].midgame.value;
                    endgame -= w_piece_square_table[p][sameside_castle][b].endgame.value;
                }
            }


        }

    }
};

struct pst_data_225{
    // to quickly recompute the values for the piece square tables, we require the indices of each square
    // and how often is being used. We also need to know if we deal with same side or opposite side castling
    bool sameside_castle;

    // we use 8 bits (256 values). Can have 32 pieces at most -> 256 bit = 32byte
    std::vector<int8_t> indices_white_wk[6]{};
    std::vector<int8_t> indices_black_wk[6]{};
    std::vector<int8_t> indices_white_bk[6]{};
    std::vector<int8_t> indices_black_bk[6]{};

    pst_data_225(Board* b) {

        Square wKingSq = bitscanForward(b->getPieces(WHITE, KING));
        Square bKingSq = bitscanForward(b->getPieces(BLACK, KING));

        bool   wKSide            = (fileIndex(wKingSq) > 3 ? 0 : 1);
        bool   bKSide            = (fileIndex(bKingSq) > 3 ? 0 : 1);
        sameside_castle   = wKSide == bKSide;

        for(Piece p = PAWN; p <= KING; p++){
            for(Color c = WHITE; c <= BLACK; c++){
                U64 k = b->getPieces(c, p);
                while(k){
                    Square s = bitscanForward(k);

                    if(c == WHITE) {
                        indices_white_wk[p].push_back(pst_index_relative_white(s, wKingSq));
                        indices_white_bk[p].push_back(pst_index_relative_white(s, bKingSq));
                    }else{
                        indices_black_wk[p].push_back(pst_index_relative_black(s, wKingSq));
                        indices_black_bk[p].push_back(pst_index_relative_black(s, bKingSq));
                    }

                    k = lsbReset(k);
                }
            }
        }
    }

    void evaluate(float& midgame, float& endgame){
        for(Piece p = PAWN; p <= KING; p++){

            for(int8_t w:indices_white_wk[p]){
                midgame += w_piece_our_king_square_table[p][w].midgame.value;
                endgame += w_piece_our_king_square_table[p][w].endgame.value;
            }

            for(int8_t w:indices_white_bk[p]){
                midgame += w_piece_opp_king_square_table[p][w].midgame.value;
                endgame += w_piece_opp_king_square_table[p][w].endgame.value;
            }

            for(int8_t b:indices_black_bk[p]){
                midgame -= w_piece_our_king_square_table[p][b].midgame.value;
                endgame -= w_piece_our_king_square_table[p][b].endgame.value;
            }

            for(int8_t b:indices_black_wk[p]){
                midgame -= w_piece_opp_king_square_table[p][b].midgame.value;
                endgame -= w_piece_opp_king_square_table[p][b].endgame.value;
            }
        }
    }
};

struct king_safety_data{
    //we only need to store one index for the white and black king

    int8_t wkingsafety_index;
    int8_t bkingsafety_index;

    king_safety_data(Board* b) {

        U64 k;
        Square square;
        U64 attacks;
        U64 occupied = *(b->getOccupied());

        static int factors[6] = {0,2,2,3,4};

        Square whiteKingSquare = bitscanForward(b->getPieces(WHITE, KING));
        Square blackKingSquare = bitscanForward(b->getPieces(BLACK, KING));

        U64 whiteKingZone = KING_ATTACKS[whiteKingSquare];
        U64 blackKingZone = KING_ATTACKS[blackKingSquare];

        int wkingSafety_attPiecesCount = 0;
        int wkingSafety_valueOfAttacks = 0;

        int bkingSafety_attPiecesCount = 0;
        int bkingSafety_valueOfAttacks = 0;

        for(Piece p = KNIGHT; p <= QUEEN;p++){
            for(Color c = WHITE; c<= BLACK; c++){
                k = b->getPieces(c, p);
                while(k){
                    square  = bitscanForward(k);
                    attacks = ZERO;
                    switch (p) {
                        case KNIGHT: attacks  = KNIGHT_ATTACKS[square]; break;
                        case BISHOP: attacks  = lookUpBishopAttack(square, occupied); break;
                        case QUEEN:  attacks  = lookUpBishopAttack(square, occupied);
                        case ROOK:   attacks |= lookUpRookAttack  (square, occupied); break;
                    }
                    if(c == WHITE){
                        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, factors[p]);
                    }else{
                        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, factors[p]);
                    }

                    k = lsbReset(k);
                }
            }
        }

        wkingsafety_index = wkingSafety_valueOfAttacks;
        bkingsafety_index = bkingSafety_valueOfAttacks;
    }

    void evaluate(float& midgame, float& endgame){
        midgame += w_king_safety[wkingsafety_index].midgame.value - w_king_safety[bkingsafety_index].midgame.value;
        endgame += w_king_safety[wkingsafety_index].endgame.value - w_king_safety[bkingsafety_index].endgame.value;
    }

};

struct bishop_pawn_table_data{
    int8_t count_e[9]{};
    int8_t count_o[9]{};

    bishop_pawn_table_data(Board* b) {
        U64 k;
        Square square;

        U64 whitePawns = b->getPieces(WHITE, PAWN);
        U64 blackPawns = b->getPieces(BLACK, PAWN);

        k = b->getPieces()[WHITE_BISHOP];
        while (k) {
            square  = bitscanForward(k);
            count_e[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] += 1;
            count_o[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] += 1;
            k = lsbReset(k);
        }

        k = b->getPieces()[BLACK_BISHOP];
        while (k) {
            square  = bitscanForward(k);
            count_e[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] -= 1;
            count_o[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))] -= 1;
            k = lsbReset(k);
        }
    }

    void evaluate(float& midgame, float& endgame){
        for(int i = 0; i < 9; i++){
            midgame += count_e[i] * w_bishop_pawn_e[i].midgame.value;
            endgame += count_e[i] * w_bishop_pawn_e[i].endgame.value;

            midgame += count_o[i] * w_bishop_pawn_o[i].midgame.value;
            endgame += count_o[i] * w_bishop_pawn_o[i].endgame.value;
        }
    }
};

struct passer_data{

    int8_t count[16]{};

    passer_data(Board* b) {
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

    void evaluate(float& midgame, float& endgame){
        for(int i = 0; i < 16; i++){
            midgame += count[i] * w_passer[i].midgame.value;
            endgame += count[i] * w_passer[i].endgame.value;
        }
    }
};

struct feature_data{
    // we assume that these features are linear which means that we only need their count

    std::vector<int8_t> count{};

    feature_data(Board* b) {

        count.resize(I_END);

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
        U64 openFiles      = openFilesBlack & openFilesWhite;

        U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
        U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
        U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
        U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;

        U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
        U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);

        // clang-format off
        count[I_PAWN_DOUBLED_AND_ISOLATED] = (
            + bitCount(whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & blackDoubledPawns));
        count[I_PAWN_DOUBLED] += (
            + bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(~blackIsolatedPawns & blackDoubledPawns));
        count[I_PAWN_ISOLATED] += (
            + bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
        count[I_PAWN_PASSED] += (
            + bitCount(whitePassers)
            - bitCount(blackPassers));
        count[I_PAWN_STRUCTURE] += (
            + bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover));
        count[I_PAWN_OPEN] += (
            + bitCount(whitePawns & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~fillNorth(whitePawns)));
        count[I_PAWN_BACKWARD] += (
            + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & blackPawnCover) & whitePawns)
            - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & whitePawnCover) & blackPawns));
        count[I_PAWN_BLOCKED] += (
            + bitCount(whiteBlockedPawns)
            - bitCount(blackBlockedPawns));
        count[I_MINOR_BEHIND_PAWN] += (
            + bitCount(shiftNorth(b->getPieces()[WHITE_KNIGHT]|b->getPieces()[WHITE_BISHOP])&(b->getPieces()[WHITE_PAWN]|b->getPieces()[BLACK_PAWN]))
            - bitCount(shiftSouth(b->getPieces()[BLACK_KNIGHT]|b->getPieces()[BLACK_BISHOP])&(b->getPieces()[WHITE_PAWN]|b->getPieces()[BLACK_PAWN])));

        k = b->getPieces()[WHITE_KNIGHT];
        while (k) {
            square  = bitscanForward(k);
            count[I_KNIGHT_OUTPOST]    += isOutpost(square, WHITE, blackPawns, whitePawnCover);
            count[I_KNIGHT_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
            k = lsbReset(k);
        }

        k = b->getPieces()[BLACK_KNIGHT];
        while (k) {
            square  = bitscanForward(k);
            count[I_KNIGHT_OUTPOST]    -= isOutpost(square, BLACK, whitePawns, blackPawnCover);
            count[I_KNIGHT_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
            k = lsbReset(k);
        }



        k = b->getPieces()[WHITE_BISHOP];
        while (k) {
            square  = bitscanForward(k);

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
            square  = bitscanForward(k);
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
            + (bitCount(b->getPieces()[WHITE_BISHOP]) == 2)
            - (bitCount(b->getPieces()[BLACK_BISHOP]) == 2));


        count[I_ROOK_KING_LINE] += (
            + bitCount(lookUpRookAttack(blackKingSquare, *b->getOccupied()) & b->getPieces(WHITE, ROOK))
            - bitCount(lookUpRookAttack(whiteKingSquare, *b->getOccupied()) & b->getPieces(BLACK, ROOK)));
        count[I_ROOK_OPEN_FILE] += (
            + bitCount(openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFiles & b->getPieces(BLACK, ROOK)));
        count[I_ROOK_HALF_OPEN_FILE] += (
            + bitCount(openFilesBlack & ~openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFilesWhite & ~openFiles & b->getPieces(BLACK, ROOK)));


        k = b->getPieces()[WHITE_QUEEN];
        while (k) {
            square  = bitscanForward(k);
            count[I_QUEEN_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
            k = lsbReset(k);
        }

        k = b->getPieces()[BLACK_QUEEN];
        while (k) {
            square  = bitscanForward(k);
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
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
        count[I_SIDE_TO_MOVE] += (b->getActivePlayer() == WHITE ? 1 : -1);
    }

    void evaluate(float& midgame, float& endgame){
        for(int i = 0; i < I_END; i++){
            midgame += count[i] * w_features[i].midgame.value;
            endgame += count[i] * w_features[i].endgame.value;
        }
    }
};

struct pinned_data{
    int8_t count[15]{};

    pinned_data(Board* b){


        for(Color color= WHITE; color<= BLACK; color++){

            Color us = color;
            Color them = 1 - color;

            // figure out where the opponent has pieces
            U64 opponentOcc = b->getTeamOccupied()[them];
            U64      ourOcc = b->getTeamOccupied()[us];

            // get the pieces which can pin our king
            U64 bishops = b->getPieces(them, BISHOP);
            U64   rooks = b->getPieces(them, ROOK);
            U64  queens = b->getPieces(them, QUEEN);

            // get the king positions
            Square kingSq = bitscanForward(b->getPieces(us, KING));

            // get the potential pinners for rook/bishop attacks
            U64 rookAttacks   = lookUpRookAttack  (kingSq, opponentOcc) & (rooks   | queens);
            U64 bishopAttacks = lookUpBishopAttack(kingSq, opponentOcc) & (bishops | queens);

            // get all pinners (either rook or bishop attackers)
            U64 potentialPinners = (rookAttacks | bishopAttacks);

            while(potentialPinners){

                Square pinnerSquare = bitscanForward(potentialPinners);

                // get all the squares in between the king and the potential pinner
                U64 inBetween = inBetweenSquares[kingSq][pinnerSquare];

                // if there is exactly one of our pieces in the way, consider it pinned. Otherwise, continue
                U64 potentialPinned = ourOcc & inBetween;
                if (potentialPinned == 0 || lsbIsolation(potentialPinned) != potentialPinned){
                    potentialPinners = lsbReset(potentialPinners);
                    continue;
                }

                // extract the pinner pieces and the piece that pins
                Piece pinnedPiece = b->getPiece(bitscanForward(potentialPinned));
                Piece pinnerPiece = b->getPiece(pinnerSquare) - BISHOP;

                // normalise the values (black pieces will be made to white pieces)
                if(us == WHITE){
                    pinnerPiece -= 6;
                }else{
                    pinnedPiece -= 6;
                }


                // add to the result indexing using pinnedPiece for which there are 5 different pieces and the pinner
                count[pinnedPiece * 3 + pinnerPiece] += color == WHITE ? 1:-1;

                // reset the lsb
                potentialPinners = lsbReset(potentialPinners);
            }
        }

    }

    void evaluate(float& midgame, float& endgame){
        for(int i = 0; i < 15; i++){
            midgame += count[i] * w_pinned[i].midgame.value;
            endgame += count[i] * w_pinned[i].endgame.value;
        }
    }
};

struct hanging_data{
    int8_t count[5]{};

    hanging_data(Board* b){
        U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
        U64 BnotAttacked = ~b->getAttackedSquares(BLACK);

        for (int i = PAWN; i <= QUEEN; i++) {
            count[i] =
                + bitCount(b->getPieces(WHITE, i) & WnotAttacked)
                - bitCount(b->getPieces(BLACK, i) & BnotAttacked);
        }
    }

    void evaluate(float& midgame, float& endgame){
        for(int i = 0; i < 5; i++){
            midgame += count[i] * w_hanging[i].midgame.value;
            endgame += count[i] * w_hanging[i].endgame.value;
        }
    }

};

struct mobility_data{
    // one for each piece, first one is empty
    std::vector<uint8_t> indices_white[5]{};
    std::vector<uint8_t> indices_black[5]{};

    mobility_data(Board* b){

        U64 k, attacks;
        Square square;

        U64 whitePawnCover = shiftNorthEast(b->getPieces(WHITE, PAWN)) | shiftNorthWest(b->getPieces(WHITE, PAWN));
        U64 blackPawnCover = shiftSouthEast(b->getPieces(BLACK, PAWN)) | shiftSouthWest(b->getPieces(BLACK, PAWN));

        U64 mobilitySquaresWhite = ~(b->getTeamOccupied()[WHITE]) & ~(blackPawnCover);
        U64 mobilitySquaresBlack = ~(b->getTeamOccupied()[BLACK]) & ~(whitePawnCover);

        U64 occupied = *b->getOccupied();

        for(Piece p = KNIGHT; p <= QUEEN;p++){
            for(Color c = WHITE; c<= BLACK; c++){
                k = b->getPieces(c, p);
                while(k){
                    square  = bitscanForward(k);
                    attacks = ZERO;
                    switch (p) {
                        case KNIGHT: attacks  = KNIGHT_ATTACKS[square]; break;
                        case BISHOP: attacks  = lookUpBishopAttack(square, occupied); break;
                        case QUEEN:  attacks  = lookUpBishopAttack(square, occupied);
                        case ROOK:   attacks |= lookUpRookAttack  (square, occupied); break;
                    }
                    if(c == WHITE){
                        indices_white[p].push_back(bitCount(attacks & mobilitySquaresWhite));
                    }else{
                        indices_black[p].push_back(bitCount(attacks & mobilitySquaresBlack));
                    }

                    k = lsbReset(k);
                }
            }
        }
    }

    void evaluate(float& midgame, float& endgame){
        for(Piece p = PAWN; p <= QUEEN; p++){
            for(int8_t w:indices_white[p]){
                midgame += w_mobility[p][w].midgame.value;
                endgame += w_mobility[p][w].endgame.value;
            }

            for(int8_t b:indices_black[p]){
                midgame -= w_mobility[p][b].midgame.value;
                endgame -= w_mobility[p][b].endgame.value;
            }
        }

    }

};

struct eval_data{
    feature_data            *features;
    mobility_data           *mobility;
    hanging_data            *hanging;
    pinned_data             *pinned;
    passer_data             *passed;
    bishop_pawn_table_data  *bishop_pawn;
    king_safety_data        *king_safety;
    pst_data_64             *pst64;
    pst_data_225            *pst225;
    meta_data               *meta;

    eval_data(Board* b){
        features        = new feature_data(b);
        mobility        = new mobility_data(b);
        hanging         = new hanging_data(b);
        pinned          = new pinned_data(b);
        passed          = new passer_data(b);
        bishop_pawn     = new bishop_pawn_table_data(b);
        king_safety     = new king_safety_data(b);
        pst64           = new pst_data_64(b);
        pst225          = new pst_data_225(b);
        meta            = new meta_data(b);
    }

    virtual ~eval_data(){
        delete features;
        delete mobility;
        delete hanging;
        delete pinned;
        delete passed;
        delete bishop_pawn;
        delete king_safety;
        delete pst64;
        delete pst225;
        delete meta;
    }

    float evaluate(){
        float midgame = 0;
        float endgame = 0;

        features    ->evaluate(midgame, endgame);
        mobility    ->evaluate(midgame, endgame);
        hanging     ->evaluate(midgame, endgame);
        pinned      ->evaluate(midgame, endgame);
        passed      ->evaluate(midgame, endgame);
        bishop_pawn ->evaluate(midgame, endgame);
        king_safety ->evaluate(midgame, endgame);
        pst64       ->evaluate(midgame, endgame);
        pst225      ->evaluate(midgame, endgame);

        showScore(M((int)midgame, (int)endgame))
        float res = (int)(meta->phase * endgame) + (int)((1-meta->phase) * midgame);
        meta->evaluate(res);

        return res;
    }

};

#endif    // KOIVISTO_GRADIENT_H
