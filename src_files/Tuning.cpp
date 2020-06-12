//
// Created by finne on 6/10/2020.
//

#include "Tuning.h"

using namespace std;

int dataCount;

Board** boards;
double* results;


double sigmoid(double s, double K){
    return (double)1 / (1 + exp(-K * s / 400));
}

double sigmoidPrime(double s, double K) {
    double ex = exp(-s * K / 400);
    return (K * ex) / (400 * (ex + 1) * (ex + 1));
}

int probabiltyToCentipawnAdvantage(double prob){
    return (int)(400 * log10(prob / (1-prob)));
}

double centipawnAdvantageToProbability(int centipawns){
    return (double)1 / (1+pow(10, - (double)centipawns/400));
}



void tuning::loadPositionFile(std::string path, int count) {
    
    clearLoadedData();
    results = new double[count];
    boards = new Board*[count];
    
    
    fstream newfile;
    newfile.open(path, ios::in);
    if (newfile.is_open()) {
        string tp;
        int lineCount = 0;
        int posCount = 0;
        while (getline(newfile, tp)) {
            
            
            
            
            //finding the first "c" to check where the fen ended
            auto firstC = tp.find_first_of('c');
            auto lastC = tp.find_last_of('c');
            if(firstC == string::npos || lastC == string::npos){
                continue;
            }
            
            //extracting the fen and result and removing bad characters.
            string fen = tp.substr(0, firstC);
            string res = tp.substr(lastC+2, string::npos);
            
            fen = trim(fen);
            res = findAndReplaceAll(res, "\"", "");
            res = findAndReplaceAll(res, ";", "");
            res = trim(res);
            
            
            //parsing the result to a usable value:
            //assuming that the result is given as : a-b
            if(res.find('-') != string::npos){
                if(res == "1/2-1/2"){
                    results[lineCount] = 0.5;
                }else if(res == "1-0"){
                    results[lineCount] = 1;
                }else if(res == "0-1"){
                    results[lineCount] = 0;
                }else{
                    continue;
                }
            }
            //trying to read the result as a decimal
            else{
                try {
                    double actualResult = stod(res);
                    results[lineCount] = actualResult;
                }
                catch(std::invalid_argument& e){
                    continue;
                }
            }
            Board* b = new Board(fen);
            
            boards[posCount] = b;
            
//            std::cout << "-------------------------------------------------------------------------" << std::endl;
//
//            std::cout << fen << std::endl;
//            std::cout << *boards[lineCount] << std::endl;
//
//            std::cout << results[lineCount] << std::endl;
//            cout << tp << "\n";
    
            lineCount ++;
            posCount++;
    
            
            if(posCount % 10000 == 0){
    
                std::cout << "\r" << loadingBar(posCount, count, "Loading data") << std::flush;
            }
            
            if(posCount >= count) break;
    
            
            
            
        }
        
        std::cout << std::endl;
        
        dataCount = lineCount;
        
        newfile.close();
    }
    
}

void tuning::clearLoadedData(){
    if(boards != nullptr){
        
        for(int i = 0; i < dataCount; i++){
            delete boards[i];
        }
        
        delete boards;
        delete results;
    }
}

double tuning::optimise(Evaluator *evaluator, double K, double learningRate) {
    int paramCount = evaluator->paramCount();
    
    
    auto* earlyGrads = new double[paramCount]{};
    auto* lateGrads = new double[paramCount]{};
    
    double score = 0;
    
    for(int i = 0; i < dataCount; i++){
        Score       q_i             = evaluator->evaluate(boards[i]);
        double      expected        = results[i];
        
        double      sig             = sigmoid(q_i, K);
        double      sigPrime        = sigmoidPrime(q_i, K);
        double      lossPrime       = - 2 * (expected - sig);
        
        double*     features        = evaluator->getFeatures();
        double      phase           = evaluator->getPhase();
        
        for(int p = 0; p < paramCount; p++){
       
            
            earlyGrads[p] += features[p] * phase * sigPrime * lossPrime;
            lateGrads[p] += features[p] * (1-phase) * sigPrime * lossPrime;
        }
        
        score += (expected - sig) * (expected - sig);
    }
    
    for(int p = 0; p < paramCount; p++){
        
//        std::cout << "adjusting early " << p << " by " << (earlyGrads[p] * learningRate / dataCount)<< std::endl;
//        std::cout << "adjusting late  " << p << " by " << (lateGrads[p] * learningRate / dataCount) << std::endl;
        
        evaluator->getEarlyGameParams() [p] -= earlyGrads[p] * learningRate / dataCount;
        evaluator->getLateGameParams()  [p] -= lateGrads [p] * learningRate / dataCount;
    }
    
    
    return score / dataCount;
    
}

double tuning::computeError(Evaluator *evaluator, double K) {
    double score = 0;
    for(int i = 0; i < dataCount; i++){
        
        Score       q_i             = evaluator->evaluate(boards[i]);
        double      expected        = results[i];
        
        double      sig             = sigmoid(q_i, K);
        
        
//        std::cout << sig << std::endl;
        
        
        score += (expected - sig) * (expected - sig);
    }
    return score / dataCount;
}

double tuning::computeK(Evaluator *evaluator, double initK, double rate, double deviation) {
    
    double K = initK;
    double dK = 0.01;
    double dEdK = 1;
    
    
    while(abs(dEdK) > deviation){
        dEdK = (computeError(evaluator, K + dK) - computeError(evaluator, K - dK)) / (2 * dK);
        
        std::cout << "K:" << K << " Error: " << computeError(evaluator, K)  << " dev: " << abs(dEdK) << std::endl;
        
        //System.out.format("K: %-2.6f  Error: %-2.6f  dE/dK: %-1.2E\n", K,errorMultithreaded(evaluator, K, pool),dEdK);
        K -= dEdK * rate;
    }
    
    
    return K;
}



