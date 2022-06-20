#ifndef KOIVISTO_MCTS_H
#define KOIVISTO_MCTS_H

#include "move.h"
#include "bitboard.h"
#include "board.h"
#include <unordered_map>
#include <iostream>

#define CHILD_COUNT

class Edge {
    public:
    double visits;
    double eval;
    double prior;
    move::Move move;

    double UTC(double parentVisits);
    double Eval();
    Edge() {};
};

class Node {
    public:
    move::Move move;
    double visits;
    double eval;

    int internalChildCount;
    Edge children[CHILD_COUNT];

    void expand(Board* b);
    void backup(Board* b);
    Edge bestUTCEdge();
    Edge bestScoringEdge();
    Edge bestVisitsEdge();
};

class Tree {
    Node* getNode(bb::U64 hash);
    std::unordered_map<bb::U64, Node> HashMap = {};

    bb::U64 nodeCount;

    public:
    move::Move Search(Board* b, bb::U64 maxNodes);
};

#endif