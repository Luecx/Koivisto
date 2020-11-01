//
// Created by Luecx on 18.10.2020.
//

#ifndef KOIVISTO_GRADIENTS_H
#define KOIVISTO_GRADIENTS_H

#include "Tuning.h"
#include "eval.h"

namespace tuning {

using namespace eval;

float mob_grads[6][28];

void gradient_mobility(Board* boards) {
    
    for (Piece p = KNIGHT; p <= QUEEN; p++) {
        for (Square s = 0; s < eval::mobEntryCount[p]; s++) {
            mob_grads[p][s] = 0;
        }
    }
    
    U64 whiteTeam  = boards->getTeamOccupied()[WHITE];
    U64 blackTeam  = boards->getTeamOccupied()[BLACK];
    U64 occupied   = *boards->getOccupied();
    U64 whitePawns = boards->getPieces()[WHITE_PAWN];
    U64 blackPawns = boards->getPieces()[BLACK_PAWN];
    
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    
    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);
    
    U64    k;
    Square s;
    k = boards->getPieces(WHITE, KNIGHT);
    while (k) {
        s = bitscanForward(k);
        mob_grads[KNIGHT][bitCount(KNIGHT_ATTACKS[s] & mobilitySquaresWhite)] += 1;
        k = lsbReset(k);
    }
    k = boards->getPieces(BLACK, KNIGHT);
    while (k) {
        s = bitscanForward(k);
        mob_grads[KNIGHT][bitCount(KNIGHT_ATTACKS[s] & mobilitySquaresBlack)] -= 1;
        k = lsbReset(k);
    }
    
    k = boards->getPieces(WHITE, BISHOP);
    while (k) {
        s = bitscanForward(k);
        mob_grads[BISHOP][bitCount(lookUpBishopAttack(s, occupied) & mobilitySquaresWhite)] += 1;
        
        k = lsbReset(k);
    }
    k = boards->getPieces(BLACK, BISHOP);
    while (k) {
        s = bitscanForward(k);
        mob_grads[BISHOP][bitCount(lookUpBishopAttack(s, occupied) & mobilitySquaresBlack)] -= 1;
        k = lsbReset(k);
    }
    
    k = boards->getPieces(WHITE, ROOK);
    while (k) {
        s = bitscanForward(k);
        mob_grads[ROOK][bitCount(lookUpRookAttack(s, occupied) & mobilitySquaresWhite)] += 1;
        k = lsbReset(k);
    }
    k = boards->getPieces(BLACK, ROOK);
    while (k) {
        s = bitscanForward(k);
        mob_grads[ROOK][bitCount(lookUpRookAttack(s, occupied) & mobilitySquaresBlack)] -= 1;
        k = lsbReset(k);
    }
    
    k = boards->getPieces(WHITE, QUEEN);
    while (k) {
        s = bitscanForward(k);
        mob_grads[QUEEN][bitCount(
            (lookUpRookAttack(s, occupied) | lookUpBishopAttack(s, occupied)) & mobilitySquaresWhite)] += 1;
        k = lsbReset(k);
    }
    k = boards->getPieces(BLACK, QUEEN);
    while (k) {
        s = bitscanForward(k);
        mob_grads[QUEEN][bitCount(
            (lookUpRookAttack(s, occupied) | lookUpBishopAttack(s, occupied)) & mobilitySquaresBlack)] -= 1;
        k = lsbReset(k);
    }
}

double tune_mobility_gradients(Board** boards, double* target, int count, double K) {
    float gradsMG[6][28];
    float gradsEG[6][28];
    for(int i = 0; i < count; i++) {
        
        Score q_i = eval::evaluate(boards[i]);
        double sig       = tuning::sigmoid(q_i, K);
        double sigPrime  = tuning::sigmoidPrime(q_i, K);
        double lossPrime = -2 * (target[i] - sig);
        
        float phase = eval::phase(boards[i]);
        
        gradient_mobility(boards[i]);
        for(Piece p = KNIGHT; p <= QUEEN; p++){
            for(int i = 0; i < eval::mobEntryCount[p]; i++){
                gradsMG[p][i] += mob_grads[p][i] * (1-phase) * sigPrime * lossPrime;
                gradsEG[p][i] += mob_grads[p][i] * (  phase) * sigPrime * lossPrime;
            }
        }
    }
    
    for(Piece p = KNIGHT; p <= QUEEN; p++){
        for(int i = 0; i < eval::mobEntryCount[p]; i++){
            int mv = gradsMG[p][i] > 0 ? 1 : gradsMG[p][i] < 0 ? -1:0;
            int ev = gradsEG[p][i] > 0 ? 1 : gradsEG[p][i] < 0 ? -1:0;
            eval::mobilities[p][i] += M(-mv,-ev);
        }
    }
    
    
    static std::string names[6]{
        "",
        "EvalScore       mobilityKnight[9]",
        "EvalScore       mobilityBishop[14]",
        "EvalScore       mobilityRook[15]",
        "EvalScore       mobilityQueen[28]",
        "",
    };
    
    
    for(Piece p = KNIGHT; p <= QUEEN; p++){
        std::cout << names[p] << "{" << std::endl;
        for(int i = 0; i < eval::mobEntryCount[p]; i++){
            std::cout << "M(" << std::setw(5) << MgScore(eval::mobilities[p][i]) << "," << std::setw(5)  << EgScore(eval::mobilities[p][i]) << "), ";
            
            if(i % 5 == 4){
                std::cout << std::endl;
            }
        }
        std::cout << "};" << std::endl;
        std::cout << std::endl;
    }
    return tuning::computeError(K);
}


}
#endif //KOIVISTO_GRADIENTS_H
