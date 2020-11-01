
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
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
 
#ifndef KOIVISTO_TUNING_H
#define KOIVISTO_TUNING_H

#include "Bitboard.h"
#include "Util.h"
#include "eval.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace eval;

namespace tuning {

extern int dataCount;
extern Board** boards;
extern double* results;

double sigmoid(double s, double K);

double sigmoidPrime(double s, double K);

/**
 * loads a position file
 * @param path
 * @param count
 */
void loadPositionFile(std::string path, int count, int start = 0);

/**
 * used to clear all the loaded data
 */
void clearLoadedData();

/**
 * does blackbox tuning on the given data. This is usually very inefficient.
 * @param evaluator
 * @param K
 * @return
 */
double optimiseBlackBox(double K, float* params, int paramCount, float lr);

/**
 * does blackbox tuning on the given data. This is usually very inefficient.
 * @param evaluator
 * @param K
 * @return
 */
double optimisePSTBlackBox(double K, EvalScore* evalScore, int count, int lr);

/**
 * does blackbox tuning on the given data. This is usually very inefficient.
 * @param evaluator
 * @param K
 * @return
 */
double optimisePSTBlackBox(double K, EvalScore** evalScore, int count, int lr);

/**
 * computes the error of the evaluator on the given set
 */
double computeError(double K);

/**
 * computes the K value
 * @param evaluator
 * @param initK
 * @param rate
 * @param deviation
 * @return
 */
double computeK(double initK, double rate, double deviation);

/**
 * computes the average time for evaluation
 */
void evalSpeed();

}    // namespace tuning

#endif    // KOIVISTO_TUNING_H
