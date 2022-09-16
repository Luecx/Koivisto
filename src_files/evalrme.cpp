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

//
// Created by Luecx on 16.09.2022.
//

#include "evalrme.h"
#include "attacks.h"
#include "board.h"


EvalScore mobilityKnight[9] = {
    M(   20,  -50), M(   35,    0), M(   41,   28), M(   47,   37), M(   54,   44), M(   59,   53), M(   65,   57), M(   74,   51),
    M(   83,   39), };

EvalScore mobilityBishop[14] = {
    M(   20,  -52), M(   27,   11), M(   35,   44), M(   41,   61), M(   49,   73), M(   55,   88), M(   56,   99), M(   57,  107),
    M(   61,  111), M(   66,  112), M(   74,  111), M(   87,  105), M(   91,  125), M(  133,   84), };

EvalScore mobilityRook[15] = {
    M(   28,   22), M(   33,   40), M(   38,   58), M(   39,   76), M(   42,   87), M(   49,   93), M(   54,  101), M(   60,  102),
    M(   66,  106), M(   70,  111), M(   74,  113), M(   79,  116), M(   87,  114), M(  102,  101), M(  171,   67), };

EvalScore mobilityQueen[28] = {
    M(  -10, -160), M(    3,  -85), M(   12,   36), M(   15,  108), M(   18,  150), M(   20,  176), M(   23,  201), M(   25,  219),
    M(   27,  234), M(   30,  241), M(   34,  247), M(   37,  254), M(   38,  261), M(   38,  268), M(   37,  276), M(   36,  278),
    M(   37,  282), M(   34,  283), M(   37,  284), M(   44,  278), M(   51,  270), M(   48,  269), M(   39,  264), M(   62,  252),
    M(  -15,  286), M(   94,  223), M(   17,  291), M(   18,  299), };


// evaluates mobility for given side
template<bb::Color side> EvalScore evaluateMobility(Board& board) {
    
    EvalScore res{};
    
    const bb::U64 opp_pawn = board.getPieceBB<!side, bb::PAWN>();
    const bb::U64 covered  = side == bb::WHITE ? bb::shiftSouthEast(opp_pawn) |
                                                 bb::shiftSouthWest(opp_pawn) :
                                                 bb::shiftNorthEast(opp_pawn) |
                                                 bb::shiftNorthWest(opp_pawn);
    const bb::U64 occupied = board.getOccupiedBB();
    for(bb::PieceType pt : {bb::KNIGHT, bb::BISHOP, bb::ROOK, bb::QUEEN}){
        bb::U64 bb = board.template getPieceBB<side>(pt);
        while(bb){
            bb::Square sq = bb::bitscanForward(bb);
            bb::U64 attacks;
            switch (pt) {
                case bb::KNIGHT: 
                    attacks  = attacks::KNIGHT_ATTACKS[sq];
                    break;
                case bb::BISHOP:
                    attacks  = attacks::lookUpBishopAttacks(sq, occupied);
                    break;
                case bb::ROOK  :
                    attacks  = attacks::lookUpRookAttacks(sq, occupied);
                    break;
                case bb::QUEEN :
                    attacks  = attacks::lookUpRookAttacks(sq, occupied);
                    attacks |= attacks::lookUpBishopAttacks(sq, occupied);
                    break;
            }
            attacks &= ~covered;
            switch (pt) {
                case bb::KNIGHT:
                    res += mobilityKnight[bb::bitCount(attacks)];
                    break;
                case bb::BISHOP:
                    res += mobilityBishop[bb::bitCount(attacks)];
                    break;
                case bb::ROOK  :
                    res += mobilityRook[bb::bitCount(attacks)];
                    break;
                case bb::QUEEN :
                    res += mobilityQueen[bb::bitCount(attacks)];
                    break;
            }
            
            bb = bb::lsbReset(bb);
        }
    }
    return res;
}


// full rme evaluation for given side
template<bb::Color side> EvalScore evaluateSide(Board& board) {
    return evaluateMobility<side>(board);
}


// main evaluate function
bb::Score RMEEvaluator::evaluate(Board* board, float phase) {
    EvalScore eval_score = evaluateSide<bb::WHITE>(*board) - evaluateSide<bb::BLACK>(*board);
    
    // flip score for black
    if (board->getActivePlayer() == bb::BLACK) {
        eval_score = -eval_score;
    }
    
    // adjust by phase
    return MgScore(eval_score) * (1 - phase) + EgScore(eval_score) * phase;
}
