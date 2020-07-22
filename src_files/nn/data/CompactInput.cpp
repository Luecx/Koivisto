//
// Created by finne on 7/22/2020.
//

#include <vector>
#include "CompactInput.h"
#include "../../Util.h"
#include "../../Bitboard.h"


using namespace bb;



std::vector<CompactInputEntry>* fromFen(std::string fen) {
    
    std::vector<CompactInputEntry> *entries = new std::vector<CompactInputEntry>{};
    
//<editor-fold desc="splitting/trimming string">
    std::vector<std::string> split{};
    std::string str{fen};
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
    
    return entries;
}

CompactTrainEntry::CompactTrainEntry(std::vector<CompactInputEntry> *input, float output) : input(input),
                                                                                            output(output) {}
