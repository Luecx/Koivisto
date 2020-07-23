//
// Created by finne on 7/22/2020.
//

#include <vector>
#include "CompactInput.h"
#include <fstream>
#include "../../Bitboard.h"
#include "../../Util.h"


using namespace bb;



CompactTrainEntry* trainDataFromFen(std::string fenAndEval) {
    
    std::vector<CompactInputEntry> *entries = new std::vector<CompactInputEntry>{};
    
    
    
    std::vector<std::string> splitFenAndEval{};
    splitString(fenAndEval, splitFenAndEval, ';');
    
    
    float evalScore = 0;
    std::string eval{splitFenAndEval[1]};
    eval = trim(eval);
    
    
    if(eval.at(0) == '#'){
        return nullptr;
    }else{
        evalScore = stof(eval);
    }


//<editor-fold desc="splitting/trimming string">
    std::vector<std::string> split{};
    std::string str{splitFenAndEval[0]};
    str = trim(str);
    findAndReplaceAll(str, "  ", " ");
    splitString(str, split, ' ');
    //</editor-fold>
    
    
    //<editor-fold desc="parsing pieces">
    File x{0};
    Rank y{7};
    for (char c : split[0]) {
        
        if (c == '/') {
            x = 0;
            y--;
            continue;
        }
        
        if (c < '9') {
            x += (c - '0');
            continue;
        } else {
            int offset = (c >= 'a') ? 6 : 0;
            
            Square sq = squareIndex(y, x);
            
            switch (toupper(c)) {
                case 'P':
                    entries->emplace_back(CompactInputEntry{offset * 64 + 64 * 0 + sq,1});
                    break;
                case 'N':
                    entries->emplace_back(CompactInputEntry{offset * 64 + 64 * 1 + sq,1});
                    break;
                case 'B':
                    entries->emplace_back(CompactInputEntry{offset * 64 + 64 * 2 + sq,1});
                    break;
                case 'R':
                    entries->emplace_back(CompactInputEntry{offset * 64 + 64 * 3 + sq,1});
                    break;
                case 'Q':
                    entries->emplace_back(CompactInputEntry{offset * 64 + 64 * 4 + sq,1});
                    break;
                case 'K':
                    entries->emplace_back(CompactInputEntry{offset * 64 + 64 * 5 + sq,1});
                    break;
            }
            
            x++;
        }
    }
    //</editor-fold>
    CompactTrainEntry* c = new CompactTrainEntry(entries, evalScore);
    return c;
}

CompactTrainEntry::CompactTrainEntry(std::vector<CompactInputEntry> *input, float output) : input(input),
                                                                                            output(output) {}



std::vector<CompactTrainEntry*>* trainDataFromFile(std::string file, int maxcount,std::vector<CompactTrainEntry*>* v) {
    
    
    
    std::vector<CompactTrainEntry*>* vec;
    if(v != nullptr){
        vec = v;
    }else{
        vec = new std::vector<CompactTrainEntry*>{};
    }
    

    std::ifstream infile(file);
    
    std::string line;
    int posCount = 0;
    while (std::getline(infile, line)) {
    
        if(posCount % 10000 == 0){
            std::cout << "\r" << loadingBar(posCount, maxcount, "Loading data") << std::flush;
        }
        
        
        CompactTrainEntry* en = trainDataFromFen(line);
        if(en != nullptr)
            vec->push_back(trainDataFromFen(line));
    
        posCount++;
        if(posCount >= maxcount){
            break;
        }
    }
    
    return vec;
}


