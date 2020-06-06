//
// Created by kimka on 6.6.2020.
//

#ifndef KOIVISTO_HISTORY_H
#define KOIVISTO_HISTORY_H

struct SearchData{

    //history table (from-to)
    int history[64][64] = {0};

    void addHistoryScore(int from, int to, int depth);
    void subtractHistoryScore(int from, int to, int depth);

};


#endif //KOIVISTO_HISTORY_H

