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
