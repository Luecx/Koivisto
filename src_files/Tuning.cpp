
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

#include "Tuning.h"

#include "Bitboard.h"

#include <iomanip>

using namespace std;

int dataCount;

Board** boards;
double* results;

double sigmoid(double s, double K) { return (double) 1 / (1 + exp(-K * s / 400)); }

double sigmoidPrime(double s, double K) {
    double ex = exp(-s * K / 400);
    return (K * ex) / (400 * (ex + 1) * (ex + 1));
}

void tuning::loadPositionFile(std::string path, int count, int start) {

    clearLoadedData();
    results = new double[count];
    boards  = new Board*[count];

    fstream newfile;
    newfile.open(path, ios::in);
    if (newfile.is_open()) {
        string tp;
        int    lineCount = 0;
        int    posCount  = 0;
        while (getline(newfile, tp)) {

            if (lineCount < start) {
                lineCount++;
                continue;
            }

            // finding the first "c" to check where the fen ended
            auto firstC = tp.find_first_of('c');
            auto lastC  = tp.find_last_of('c');
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

            // parsing the result to a usable value:
            // assuming that the result is given as : a-b
            if (res.find('-') != string::npos) {
                if (res == "1/2-1/2") {
                    results[posCount] = 0.5;
                } else if (res == "1-0") {
                    results[posCount] = 1;
                } else if (res == "0-1") {
                    results[posCount] = 0;
                } else {
                    continue;
                }
            }
            // trying to read the result as a decimal
            else {
                try {
                    double actualResult = stod(res);
                    results[posCount]   = actualResult;
                } catch (std::invalid_argument& e) { continue; }
            }
            Board* b = new Board(fen);

            boards[posCount] = b;

            lineCount++;
            posCount++;

            if (posCount % 10000 == 0) {

                std::cout << "\r" << loadingBar(posCount, count, "Loading data") << std::flush;
            }

            if (posCount >= count)
                break;
        }

        std::cout << std::endl;

        dataCount = posCount;

        newfile.close();
    }
}

void tuning::clearLoadedData() {
    if (boards != nullptr) {

        for (int i = 0; i < dataCount; i++) {
            delete boards[i];
        }

        delete[] boards;
        delete[] results;
    }
}

double tuning::optimiseBlackBox(Evaluator* evaluator, double K, float* params, int paramCount, float lr) {

    for (int p = 0; p < paramCount; p++) {

        std::cout << "\r  param: " << p << std::flush;

        double er = computeError(evaluator, K);

        params[p] += lr;
        double erUpper = computeError(evaluator, K);

        if (erUpper < er)
            continue;

        params[p] -= 2 * lr;
        double erLower = computeError(evaluator, K);

        if (erLower < er)
            continue;

        params[p] += lr;
    }
    std::cout << std::endl;

    return computeError(evaluator, K);
}

double tuning::optimisePSTBlackBox(Evaluator* evaluator, double K, EvalScore* evalScore, int count, int lr){
    double er;
    
    for(int p = 0; p < count; p++){
        
        std::cout << "\r  param: " << p << std::flush;
        
        er = computeError(evaluator, K);
//        std::cout << er << std::endl;
        evalScore[p] += M(+lr,0);
        eval_init();
//        showScore(M(+lr,0));
        
        double upper = computeError(evaluator, K);
//        std::cout << upper << std::endl;
        if(upper >= er){
            evalScore[p] += M(-2*lr,0);
            eval_init();
//            showScore(evalScore[p]);
            
            double lower = computeError(evaluator, K);
            
            if(lower >= er){
                evalScore[p] += M(+lr,0);
//                showScore(evalScore[p]);
                eval_init();
            }
        }
        
        
        er = computeError(evaluator, K);
        evalScore[p] += M(0,+lr);
        eval_init();
        
        upper = computeError(evaluator, K);
        if(upper >= er){
            evalScore[p] += M(0,-2*lr);
            eval_init();
            
            double lower = computeError(evaluator, K);
            
            if(lower >= er){
                evalScore[p] += M(0,+lr);
                eval_init();
            }
        }
    }
    std::cout << std::endl;
    return er;
}

double tuning::optimisePSTBlackBox(Evaluator* evaluator, double K, EvalScore** evalScore, int count, int lr) {
    double er;
    
    for(int p = 0; p < count; p++){
        
        std::cout << "\r  param: " << p << std::flush;
        
        er = computeError(evaluator, K);
        *evalScore[p] += M(+lr,0);
        eval_init();
        
        double upper = computeError(evaluator, K);
        if(upper >= er){
            *evalScore[p] += M(-2*lr,0);
            eval_init();
            
            double lower = computeError(evaluator, K);
            
            if(lower >= er){
                *evalScore[p] += M(+lr,0);
                eval_init();
            }
        }
        
        
        er = computeError(evaluator, K);
        *evalScore[p] += M(0,+lr);
        eval_init();
        
        upper = computeError(evaluator, K);
        if(upper >= er){
            *evalScore[p] += M(0,-2*lr);
            eval_init();
            
            double lower = computeError(evaluator, K);
            
            if(lower >= er){
                *evalScore[p] += M(0,+lr);
                eval_init();
            }
        }
    }
    std::cout << std::endl;
    return er;
}

double tuning::computeError(Evaluator* evaluator, double K) {
    double score = 0;
    for (int i = 0; i < dataCount; i++) {

        Score  q_i      = evaluator->evaluate(boards[i]);
        double expected = results[i];

        double sig = sigmoid(q_i, K);

        //        std::cout << sig << std::endl;

        score += (expected - sig) * (expected - sig);
    }
    return score / dataCount;
}

double tuning::computeK(Evaluator* evaluator, double initK, double rate, double deviation) {

    double K    = initK;
    double dK   = 0.01;
    double dEdK = 1;

    while (abs(dEdK) > deviation) {

        double Epdk = computeError(evaluator, K + dK);
        double Emdk = computeError(evaluator, K - dK);

        dEdK = (Epdk - Emdk) / (2 * dK);

        std::cout << "K:" << K << " Error: " << (Epdk + Emdk) / 2 << " dev: " << abs(dEdK) << std::endl;

        // System.out.format("K: %-2.6f  Error: %-2.6f  dE/dK: %-1.2E\n", K,errorMultithreaded(evaluator, K,
        // pool),dEdK);
        K -= dEdK * rate;
    }

    return K;
}

void tuning::evalSpeed() {

    Evaluator evaluator {};

    startMeasure();
    U64 sum = 0;
    for (int i = 0; i < dataCount; i++) {
        sum += evaluator.evaluate(boards[i]);
    }

    int ms = stopMeasure();
    std::cout << ms << "ms for " << dataCount << " positions = " << (1000 * dataCount / ms) / 1e6 << "Mnps"
              << " Checksum = " << sum << std::endl;
}
