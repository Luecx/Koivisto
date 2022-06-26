#ifndef KOIVISTO_MCTS_H
#define KOIVISTO_MCTS_H

#include "move.h"
#include "bitboard.h"
#include "board.h"
#include "search.h"
#include <unordered_map>
#include <iostream>

#define CHILD_COUNT 100

class Edge {
    public:
    double     visits = 0;
    double     eval   = 0;
    double     prior  = 0;
    move::Move move   = 0;
    double     abScore= -1.0;

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
    double     eval     = 0;
    bool       terminal = false;
    double     abScore  = 1.0;

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

    public:
    move::Move mctsSearch(Board* b, bb::U64 maxNodes, Search* search);
};

#endif