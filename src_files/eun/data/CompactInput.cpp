//
// Created by finne on 7/22/2020.
//

#include <vector>
#include "CompactInput.h"
#include "../nntools.h"
#include <fstream>
#include "../../Bitboard.h"
#include "../../Util.h"


using namespace bb;



TrainEntry* generateTrainEntry(std::string fen) {
    
    std::vector<std::string> splitFenAndEval{};
    splitString(fen, splitFenAndEval, ';');
    
    
//    std::cout << splitFenAndEval.at(0) << std::endl;
    
    float evalScore = 0;
    std::string eval{splitFenAndEval[1]};
    eval = trim(eval);
    
    
    if(eval.at(0) == '#'){
        return nullptr;
    }else{
        evalScore = stof(eval);
    }
    if(abs(evalScore) > 50) return nullptr;
    

//<editor-fold desc="splitting/trimming string">
    Board* b = new Board{splitFenAndEval.at(0)};
    
    TrainEntry* entry = new TrainEntry(b, evalScore * (b->getActivePlayer() == WHITE ? 1:-1));
    
    return entry;
}




std::vector<TrainEntry*>* generateTrainData(std::string file, int max_count, std::vector<TrainEntry*>* vec) {
    
    
    
    std::vector<TrainEntry*>* ve;
    if(vec != nullptr){
        ve = vec;
    }else{
        ve = new std::vector<TrainEntry*>{};
    }
    

    std::ifstream infile(file);
    
    std::string line;
    int posCount = 0;
    while (std::getline(infile, line)) {
    
        if(posCount % 10000 == 0){
            std::cout << "\r" << loadingBar(posCount, max_count, "Loading data") << std::flush;
        }
        
        
        TrainEntry* en = generateTrainEntry(line);
        if(en != nullptr)
            ve->push_back(generateTrainEntry(line));
    
        posCount++;
        if(posCount >= max_count){
            break;
        }
    }
    
    std::cout << std::endl;
    
    return ve;
}

void writeInSparseFormat(std::vector<TrainEntry *> *data, std::string file) {
    
    
    std::ofstream out(file);
    for(int i = 0; i < data->size(); i++){
        
        
        Board* board = data->at(i)->board;
        
        Color activePlayer = board->getActivePlayer();
        Square whiteKing = bitscanForward(board->getPieces()[WHITE_KING]);
        Square blackKing = bitscanForward(board->getPieces()[BLACK_KING]);
        for(Piece p = WHITE_PAWN; p <= BLACK_KING; p++){
            U64 k = board->getPieces()[p];
            while(k){
                Square s = bitscanForward(k);
                
                if(p / 6 == WHITE){
                    out << whiteInputIndex(activePlayer, p, s, blackKing) << " ";
                }else{
                    out << blackInputIndex(activePlayer, p, s, whiteKing) << " ";
                }
                k = lsbReset(k);
            }
        }
        
        
        
        out << data->at(i)->output << "\n";
        
    }
    out.close();
}



void writeInSparseFormat(std::string inputFile, std::string outputFile) {
    std::ofstream out(outputFile);
    std::ifstream infile(inputFile);
    
    std::string line;
    int posCount = 0;
    while (std::getline(infile, line)) {
        
        
        if(posCount % 10000 == 0){
            std::cout << "\r" << loadingBar(posCount, 1e9, "Loading data") << std::flush;
        }
        
        
        TrainEntry* en = generateTrainEntry(line);
        if(en == nullptr) continue;
        
        
        
        
        
        
        Board* board = en->board;
        
        Color activePlayer = board->getActivePlayer();
        Square whiteKing = bitscanForward(board->getPieces()[WHITE_KING]);
        Square blackKing = bitscanForward(board->getPieces()[BLACK_KING]);
        for(Piece p = WHITE_PAWN; p <= BLACK_KING; p++){
            U64 k = board->getPieces()[p];
            while(k){
                Square s = bitscanForward(k);
                
                if(p / 6 == WHITE){
                    out << whiteInputIndex(activePlayer, p, s, blackKing) << " ";
                }else{
                    out << blackInputIndex(activePlayer, p, s, whiteKing) << " ";
                }
                k = lsbReset(k);
            }
        }
        out << en->output << "\n";
        
        
        
        
        
        
        posCount++;
        
        delete en;
    }
    infile.close();
    out.close();
    
    
}


TrainEntry::TrainEntry(Board *board, float output) : board(board), output(output) {}

TrainEntry::~TrainEntry() {
    delete board;
}
