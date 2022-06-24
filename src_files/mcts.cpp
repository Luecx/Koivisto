#include "mcts.h"
#include "move.h"
#include "bitboard.h"
#include "movegen.h"
#include <unordered_map>

move::MoveList generator = {};
std::unordered_map<uint64_t, Node>* HashMap;

double fastS(double eval) {
    return eval / (100 + abs(eval));
}

void copy(Edge* e, Node* n) {
    e->eval = -1.0 * n->eval;
    e->visits = n->visits;
}

double Edge::UTC(double parentVisits) {
    return (this->eval / (this->visits + 1)) + this->prior + log(parentVisits) / (this->visits + 1);
}

double Edge::Eval() {
    return this->eval / this->visits;
}

Edge* Node::bestUTCEdge() {
    Edge* best = &this->children[0];
    for (int i = 1; i < this->internalChildCount; i++) {
        if (best->UTC(this->visits) < this->children[i].UTC(this->visits)) {
            best = &this->children[i];
        }
    }
    return best;
}

Edge* Node::bestScoringEdge() {
    Edge* best = &this->children[0];
    for (int i = 1; i < this->internalChildCount; i++) {
        if (best->Eval() < this->children[i].Eval()) {
            best = &this->children[i];
        }
    }
    return best;
}

Edge* Node::bestVisitsEdge() {
    Edge* best = &this->children[0];
    for (int i = 1; i < this->internalChildCount; i++) {
        if (best->visits < this->children[i].visits) {
            best = &this->children[i];
        }
    }
    return best;
}

double Node::calculatePriors(Board* b, Search* search, bb::Depth depth) {
    generateMoves(b, &generator, 0, &search->tds[0].searchData);
    int size            = generator.getSize();
    int bestScore = -10000;
    for (int i = 0; i < size; i++) {
        move::Move m    = generator.next();
        if (!b->isLegal(m))
            continue;
        b->move(m);
        int s = -search->pvSearch(b, -10000, 10000, 0, depth, &search->tds[0], 0, 2);
        s = std::min(1000, std::max(-10000, s));
        b->undoMove();
        this->children[this->internalChildCount] = Edge(m, fastS(1.0 * s));
        this->internalChildCount++;
        bestScore   = std::max(bestScore, s);
    }
    if (this->internalChildCount == 0) {
        this->terminal = true;
        this->eval = b->isInCheck(b->getActivePlayer()) ? -1.0 : 0.0;
        return this->eval;
    }
    return fastS(1.0 * bestScore);
}

double Node::expand(Board* b, Search* search, bb::Depth depth) {
    if (this->terminal) {
        this->visits++;
        if (b->isInCheck(b->getActivePlayer())) {
            this->eval += -1.0;
            return -1.0;
        }
        return 0.0;
    }
    if (b->isDraw()) {
        this->visits++;
        return 0.0;
    }
    if (this->visits == 0 || this->internalChildCount == 0) {
        double s   = this->calculatePriors(b, search, depth);
        this->visits += 1;
        this->eval += s;
        return s * -1.0;
    } else {
        Edge* e  = this->bestUTCEdge();
        b->move(e->move);
        Node* n  = getNode(b->zobrist());
        double s = n->expand(b, search, depth + 1);
        copy(e, n);
        b->undoMove();
        this->visits++;
        this->eval += s;
        return s * -1.0;
    }
}

Node* getNode(bb::U64 hash) {
    auto search = HashMap->find(hash);
    if (search != HashMap->end()) {
        return &search->second;
    } else {
        HashMap->try_emplace(hash, Node());
        return &HashMap->find(hash)->second;
    }
}

move::Move Tree::mctsSearch(Board* b, bb::U64 maxNodes, Search* search) {
    Board searchBoard {*b};
    std::unordered_map<uint64_t, Node> map = {};
    HashMap = &map;
    Node* rootNode = getNode(b->zobrist());
    search->resetTd();

    while (this->nodeCount < maxNodes) {

        rootNode->expand(&searchBoard, search, 0);

        this->nodeCount++;

        if (this->nodeCount % 500 == 0 && this->nodeCount > 0)
            std::cout << "info depth 1 seldepth 1 score cp " << (int)(100 * rootNode->eval / rootNode->visits) << " nodes " << search->tds[0].nodes << std::endl;
           //printPv();
        if (this->nodeCount % 500 == 0 && this->nodeCount > 0) {
        for (int i = 0; i < rootNode->internalChildCount; i++) {
            std::cout << move::toString(rootNode->children[i].move) << " ... " << rootNode->children[i].Eval() << std::endl;
        }
        }
    }

    //printPv();

    return rootNode->bestVisitsEdge()->move;
}