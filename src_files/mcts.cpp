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
    e->eval    = -1.0 * n->eval;
    e->abScore = -1.0 * n->abScore;
    e->visits  = n->visits;
}

double Edge::UTC(double parentVisits) {
    return (this->eval / (this->visits + VALUE_SCALE)) + (this->visits ? this->abScore : this->prior) * VALUE_US + log(parentVisits * VALUE_US) / (this->visits + VALUE_SCALE) * VALUE_SCALE;
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

Edge* Node::bestABEdge() {
    Edge* best = &this->children[0];
    for (int i = 1; i < this->internalChildCount; i++) {
        if (best->abScore < this->children[i].abScore) {
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
    int size         = generator.getSize();
    int bestScore    = -10000;
    for (int i = 0; i < size; i++) {
        move::Move m = generator.next();
        if (!b->isLegal(m))
            continue;
        b->move(m);
        int s = -search->pvSearch(b, -10000, 10000, 0, depth, &search->tds[0], 0, 2);
        s = std::min(10000, std::max(-10000, s));
        b->undoMove();
        this->children[this->internalChildCount] = Edge(m, fastS(1.0 * s) * VALUE_SCALE);
        this->internalChildCount++;
        bestScore = std::max(bestScore, s);
    }
    if (this->internalChildCount == 0) {
        this->terminal = true;
        this->eval = b->isInCheck(b->getActivePlayer()) ? LOSS : DRAW;
        return this->eval;
    }
    return fastS(1.0 * bestScore) * VALUE_SCALE;
}

double Node::expand(Board* b, Search* search, bb::Depth depth) {
    if (this->terminal) {
        this->visits += VALUE_SCALE;
        if (b->isInCheck(b->getActivePlayer())) {
            this->eval   += LOSS;
            this->abScore = LOSS;
            return LOSS;
        }
        this->abScore = DRAW;
        return DRAW;
    }
    if (b->isDraw()) {
        this->visits += VALUE_SCALE;
        this->abScore = DRAW;
        return DRAW;
    }
    if (this->visits == 0 || this->internalChildCount == 0) {
        double s      = this->calculatePriors(b, search, depth);
        this->visits += VALUE_SCALE;
        this->eval   += s;
        this->abScore = s;
        return s * -1.0;
    } else {
        Edge* e  = this->bestUTCEdge();
        b->move(e->move);
        Node* n  = getNode(b->zobrist());
        double s = n->expand(b, search, depth + 1);
        copy(e, n);
        b->undoMove();
        this->visits += VALUE_SCALE;
        this->eval += s;
        e = this->bestABEdge();
        this->abScore = e->abScore;
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

void Tree::printPv(Board* b) {
    Node* n = getNode(b->zobrist());
    if (n->terminal || n->internalChildCount == 0 || b->isDraw()) {
        std::cout << std::endl;
        return;
    }
    std::cout << " " << move::toString(n->bestABEdge()->move);
    b->move(n->bestABEdge()->move);
    printPv(b);
    b->undoMove();
}

move::Move Tree::mctsSearch(Board* b, bb::U64 maxNodes, Search* search, TimeManager* tm) {
    Board searchBoard {*b};
    std::unordered_map<uint64_t, Node> map = {};
    HashMap = &map;
    Node* rootNode = getNode(b->zobrist());
    search->resetTd();

    while (this->nodeCount < maxNodes && tm->isTimeLeft(&search->tds[0].searchData)) {

        rootNode->expand(&searchBoard, search, 0);

        this->nodeCount++;

        if (this->nodeCount % 500 == 0 && this->nodeCount > 0) {
            std::cout << "info depth 1 seldepth 1 score cp " << (int)(100 * rootNode->abScore * VALUE_US) << " nodes " << search->tds[0].nodes << " pv";
           this->printPv(&searchBoard);
        }
    }

    this->printPv(&searchBoard);
    return rootNode->bestABEdge()->move;
}
