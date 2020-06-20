//
// Created by finne on 6/10/2020.
//

#ifndef KOIVISTO_TUNING_H
#define KOIVISTO_TUNING_H

#include "eval.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Util.h"
#include <unistd.h>


namespace tuning {

/**
 * loads a position file
 * @param path
 * @param count
 */
void loadPositionFile(std::string path, int count);

/**
 * used to clear all the loaded data
 */
void clearLoadedData();

/**
 * uses the loaded data to optimise the evaluator
 * @param evaluator
 */
double optimise(Evaluator *evaluator, double K, double learningRate);

/**
 * computes the error of the evaluator on the given set
 */
double computeError(Evaluator *evaluator, double K);

/**
 * computes the K value
 * @param evaluator
 * @param initK
 * @param rate
 * @param deviation
 * @return
 */
double computeK(Evaluator *evaluator, double initK, double rate, double deviation);


/**
 * generates a heat map for the given piece.
 * generates 2 seperate tables for early and late if requested. it will also split into 2 tables
 * for both sides if requested
 * @param piece
 * @param earlyAndLate
 * @param asymmetric
 */
void generateHeatMap(Piece piece, bool earlyAndLate, bool asymmetric);

}

#endif //KOIVISTO_TUNING_H
