#ifndef KOIVISTO_MCTS_H
#define KOIVISTO_MCTS_H

#include "move.h"
#include "bitboard.h"
#include "board.h"
#include "search.h"
#include <unordered_map>
#include <iostream>

#define CHILD_COUNT 100

#define VALUE_SCALE 0.01
#define VALUE_US    100.0
#define VISIT       1.0 * VALUE_SCALE
#define LOSS       -1.0 * VALUE_SCALE
#define WIN         1.0 * VALUE_SCALE
#define DRAW        0.0 * VALUE_SCALE

class Edge {
    public:
    double     visits = 0;
    double     eval   = DRAW;
    double     prior  = DRAW;
    move::Move move   = 0;
    double     abScore= LOSS;

    double UTC(double parentVisits);
    double Eval();
    Edge() {};
    Edge(move::Move m, double p) {
        move  = m;
        prior = p;
    };
};

class Node {
    public:
    double     visits   = 0;
    double     eval     = DRAW;
    bool       terminal = false;
    double     abScore  = WIN;

    int internalChildCount     = 0;
    Edge children[CHILD_COUNT] = {};

    double calculatePriors(Board* b, Search* search, bb::Depth depth);
    double expand(Board* b, Search* search, bb::Depth depth);
    Edge*  bestUTCEdge();
    Edge*  bestScoringEdge();
    Edge*  bestABEdge();
    Edge*  bestVisitsEdge();
};

Node* getNode(bb::U64 hash);

class Tree {
    bb::U64 nodeCount = 0;
    void printPv(Board* b);

    public:
    move::Move mctsSearch(Board* b, bb::U64 maxNodes, Search* search, TimeManager* tm);
};

#endif