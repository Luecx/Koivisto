//
// Created by finne on 9/24/2020.
//

#include "Generator.h"

#include <fstream>
#include <unistd.h>

std::ofstream* generator::outFile;
Evaluator      generator::evaluator {};

unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
    a = a - b;
    a = a - c;
    a = a ^ (c >> 13);
    b = b - c;
    b = b - a;
    b = b ^ (a << 8);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 13);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 12);
    b = b - c;
    b = b - a;
    b = b ^ (a << 16);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 5);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 3);
    b = b - c;
    b = b - a;
    b = b ^ (a << 10);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 15);
    return c;
}

int legalNonQuietMoves(Board* b) {

    MoveList mv {};
    b->getNonQuietMoves(&mv);

    int legal = 0;

    for (int i = 0; i < mv.getSize(); i++) {
        if (b->isLegal(mv.getMove(i))) {
            legal++;
        }
    }

    return legal;
}

Score collectAllQuietPositions(Board* b, std::vector<Board>* leafs, Score alpha, Score beta, Depth ply) {

    if (b->isDraw())
        return 0;

    double stand_pat = generator::evaluator.evaluate(b) * (b->getActivePlayer() == WHITE ? 1 : -1);

    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat)
        alpha = stand_pat;

    // create a movelist which contains all non quiet moves
    MoveList mv {};

    // collect all non quiet moves (not regarding checks)
    b->getNonQuietMoves(&mv);

    MoveOrderer orderer {};
    orderer.setMovesQSearch(&mv, b);

    int legalMoves = 0;

    // we loop over all moves to get those positions
    while (orderer.hasNext()) {
        // get the current move
        Move m = orderer.next();

        if (b->staticExchangeEvaluation(m) < 0)
            continue;

        // dont do illegal moves
        if (!b->isLegal(m))
            continue;

        legalMoves++;

        // do the move
        b->move(m);

        // collect all quiet positions for this position
        Score s = collectAllQuietPositions(b, leafs, -beta, -alpha, ply + 1);

        // undo the move
        b->undoMove();

        if (s >= beta)
            return beta;
        if (s > alpha) {
            alpha = s;
        }
    }

    if (legalMoves == 0 && !b->isInCheck(b->getActivePlayer())) {
        leafs->emplace_back(b);
    }

    return alpha;
}

// select a random move.
// if there is a king in the movelist, return a king move with a probability of king_walk_p
Move selectRandomMove(MoveList& moveList, double king_walk_p) {

    // track all king moves
    MoveList kingMoves {};

    // go through each move and find king moves
    for (int i = 0; i < moveList.getSize(); i++) {

        // extract the move first for better readability
        Move m = moveList.getMove(i);

        // get the moving piece. %6 will ignore the color so that both kings will map to the white king
        Piece movingPiece = getMovingPiece(m) % 6;

        // if the piece is a king, add it to the king moves list
        if (movingPiece == KING) {
            kingMoves.add(m);
        }
    }
    // if there are no king moves or a random value in between 0 and 1 is larger than king_walk_p,
    // select randomly from all moves
    if (kingMoves.getSize() == 0 || static_cast<double>(rand()) / RAND_MAX > king_walk_p) {
        int index = rand() % moveList.getSize();
        return moveList.getMove(index);
    }
    // select a move from the king moves
    else {
        int index = rand() % kingMoves.getSize();
        return kingMoves.getMove(index);
    }
}

// gets the evaluation for the position
Score evalPosition(Board* b) {
    TimeManager manager {};
    bestMove(b, 8, &manager);
    SearchOverview ov = search_overview();
    return ov.score;
}

void generator::generate(const string& outpath) {
    bb_init();
    search_init(8);
    eval_init();
    search_disable_infoStrings();
    outFile = new std::ofstream(outpath, std::ios_base::app);

    int    min_ply              = 8;
    int    max_ply              = 500;
    int    adjudicate           = 1000;
    int    legalityCheckCounter = 10;
    double king_walk_p          = 0.1;

    int totalCount = 0;

    unsigned long seed = mix(clock(), time(NULL), getpid());
    srand(seed);

    while (true) {
        //        search_clearHash();
        Board b {};

        for (int i = 0; i < max_ply; i++) {

            // stop the search if:
            // A: the game is a draw
            // B: the eval is static eval is very large
            if (b.isDraw() || abs(evaluator.evaluate(&b)) > adjudicate) {
                break;
            }

            // create a movelist which stores all moves for the current position
            MoveList ml {};

            // get all pseudo legal moves
            b.getPseudoLegalMoves(&ml);

            // get a random move
            Move r = selectRandomMove(ml, king_walk_p);

            // the move can be illegal, so we need to do this until the move is legal.
            // in some mating positions, there might be no legal moves. we solve this by counting the amount of times we
            // run this function and stop if this is done too often.
            int  checks        = 0;
            bool stopIteration = false;
            while (!b.isLegal(r) || (legalNonQuietMoves(&b) <= 1 && b.isInCheck(b.getActivePlayer()))) {
                r = selectRandomMove(ml, king_walk_p);
                if (checks++ > legalityCheckCounter) {
                    stopIteration = true;
                    break;
                }
            }

            // if there have only been a few legal moves, exit this iteration
            if (stopIteration) {
                break;
            }

            // do the move
            b.move(r);

            // only start at min_ply
            if (i < min_ply) {
                continue;
            }

            // for this position, run a qsearch to collect the pv
            std::vector<Board> leafs {};
            collectAllQuietPositions(&b, &leafs, -MAX_MATE_SCORE, MAX_MATE_SCORE, 0);

            if (leafs.size() == 0) {
                (*outFile) << b.fen() << "\n";
            } else {
                (*outFile) << leafs[rand() % leafs.size()].fen() << "\n";
            }

            totalCount++;
            std::cout << totalCount << "  " << leafs.size() << std::endl;
        }
    }
}
void generator::evaluatePositions(const string& inpath, const string& outpath) {
    ifstream fens;
    fens.open(inpath);

    bb_init();
    search_init(1);
    eval_init();
    search_disable_infoStrings();
    outFile = new std::ofstream(outpath, std::ios_base::app);
    
    startMeasure();
    std::string line;
    int count = 0;
    while (std::getline(fens, line)) {
        Board b {line};
        search_clearHash();
        count ++;
        Score ev = evalPosition(&b);
        *outFile << b.fen().c_str() << "; " << ev << "\n";
        
        if(count % 1000 == 999){
            std::cout << 1000000 / stopMeasure() << " nps" << std::endl;
            startMeasure();
        }
//        std::cout <<count << std::endl;
    
    }
}

