#include "mcts.h"
#include "move.h"
#include "bitboard.h"
#include <unordered_map>

double Edge::UTC(double parentVisits) {
    return (eval / visits) + prior + sqrt(parentVisits) / visits;
}

double Edge::Eval() {
    return eval / visits;
}

Edge Node::bestUTCEdge() {
    Edge bestUTC = this->children[0];
    for (int i = 1; i < this->internalChildCount; i++) {
        if (bestUTC.UTC(this->visits) > this->children[i].UTC(this->visits)) {
            bestUTC = this->children[i];
        }
    }
    return bestUTC;
}

Edge Node::bestScoringEdge() {
    Edge bestUTC = this->children[0];
    for (int i = 1; i < this->internalChildCount; i++) {
        if (bestUTC.Eval() > this->children[i].Eval()) {
            bestUTC = this->children[i];
        }
    }
    return bestUTC;
}

Edge Node::bestVisitsEdge() {
    Edge bestUTC = this->children[0];
    for (int i = 1; i < this->internalChildCount; i++) {
        if (bestUTC.visits > this->children[i].visits) {
            bestUTC = this->children[i];
        }
    }
    return bestUTC;
}

void Node::expand(Board* b) {
    if (this->visits == 0) {
        return;
    } else {
        return;
    }
}

Node* Tree::getNode(bb::U64 hash) {
    return &this->HashMap[hash];
}

move::Move Tree::Search(Board* b, bb::U64 maxNodes) {
    Node* rootNode = this->getNode(b->zobrist());

    while (this->nodeCount < maxNodes) {

        rootNode->expand(b);

        if (nodeCount % 100 == 0 && nodeCount > 0)
            std::cout << this->nodeCount << std::endl;
           //printPv();
    }

    //printPv();

    return rootNode->bestVisitsEdge().move;
}