//
// Created by finne on 6/10/2020.
//

#ifndef KOIVISTO_TUNING_H
#define KOIVISTO_TUNING_H

#include "Bitboard.h"
#include "Util.h"
#include "eval.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

namespace tuning {

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
 * uses the loaded data to optimise the evaluator
 * @param evaluator
 */
double optimiseGD(Evaluator* evaluator, double K, double learningRate);

/**
 * does blackbox tuning on the given data. This is usually very inefficient.
 * @param evaluator
 * @param K
 * @return
 */
double optimiseBlackBox(Evaluator* evaluator, double K, float* params, int paramCount, float lr);

/**
 * uses the loaded data to optimise the evaluator
 */
double optimiseAdaGrad(Evaluator* evaluator, double K, double learningRate, int iterations);

/**
 * uses the loaded data to optimise the evaluator
 * @param evaluator
 */
double optimisePST(Evaluator* evaluator, double K, double learningRate);

/**
 * computes the error of the evaluator on the given set
 */
double computeError(Evaluator* evaluator, double K);

/**
 * finds the count positions which have the largest difference to the calculated outcome of the evaluator.
 * @param evaluator
 * @param K
 * @param count
 */
void findWorstPositions(Evaluator* evaluator, double K, int count);

/**
 * computes the K value
 * @param evaluator
 * @param initK
 * @param rate
 * @param deviation
 * @return
 */
double computeK(Evaluator* evaluator, double initK, double rate, double deviation);

/**
 * generates a heat map for the given piece.
 * generates 2 seperate tables for early and late if requested. it will also split into 2 tables
 * for both sides if requested
 * @param piece
 * @param earlyAndLate
 * @param asymmetric
 */
void generateHeatMap(Piece piece, bool earlyAndLate, bool asymmetric);

/**
 * computes the average time for evaluation
 */
void evalSpeed();

}    // namespace tuning

#endif    // KOIVISTO_TUNING_H
