#ifndef KOIVISTO_MCTS_H
#define KOIVISTO_MCTS_H

#include "move.h"
#include "bitboard.h"
#include <unordered_map>
#include <iostream>

#define CHILD_COUNT

class Edge {
    public:
    move::Move move;
    double visits;
    double eval;
    double prior;

    double UTC(double parentVisits);
    Edge() {};
};

class Node {
    public:
    move::Move move;
    double visits;
    double eval;

    int internalChildCount;
    Edge children[CHILD_COUNT];
};

class Tree {
    Node* getNode(bb::U64 hash);

    public:
    std::unordered_map<bb::U64, Node> HashMap = {};
};

#endif