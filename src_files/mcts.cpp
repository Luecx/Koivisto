#include "mcts.h"
#include "move.h"
#include "bitboard.h"
#include <unordered_map>

double Edge::UTC(double parentVisits) {
    return (eval / visits) + prior + sqrt(parentVisits) / visits;
}

Node* Tree::getNode(bb::U64 hash) {
    return &this->HashMap[hash];
}