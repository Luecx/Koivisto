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

#ifndef KOIVISTO_EVALRME_H
#define KOIVISTO_EVALRME_H

#include "bitboard.h"
#include "eval.h"

#include <cstdint>

typedef int32_t EvalScore;
#define M(mg, eg)    ((EvalScore)((unsigned int) (eg) << 16) + (mg))
#define MgScore(s)   ((bb::Score)((uint16_t)((unsigned) ((s)))))
#define EgScore(s)   ((bb::Score)((uint16_t)((unsigned) ((s) + 0x8000) >> 16)))

extern EvalScore mobilityKnight[9];
extern EvalScore mobilityBishop[14];
extern EvalScore mobilityRook[15];
extern EvalScore mobilityQueen[28];


class RMEEvaluator {
    public:
    bb::Score evaluate(Board* board, float phase);
};


#endif    // KOIVISTO_EVALRME_H
