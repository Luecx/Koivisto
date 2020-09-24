//
// Created by finne on 9/24/2020.
//

#include "Generator.h"

TranspositionTable* generator::searchedPosition;
std::ofstream*      generator::outFile;
Evaluator           generator::evaluator {};

void generator::generate(const string& outpath) {
    srand(time(0));

    bb_init();
    search_init(8);
    search_disable_infoStrings();
    outFile          = new std::ofstream(outpath, std::ios_base::app);
    searchedPosition = new TranspositionTable(64);

    int    min_ply              = 16;
    int    max_ply              = 500;
    int    adjudicate           = 1000;
    int    legalityCheckCounter = 10;
    double king_walk_p          = 0.1;

    int totalCount = 0;

    while (true) {

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
            bool stopIteration = 0;
            while (!b.isLegal(r)) {
                r = selectRandomMove(ml, king_walk_p);
                if (checks++ > legalityCheckCounter) {
                    stopIteration = true;
                }
            }

            // if there have only been a few legal moves, exit this iteration
            if (stopIteration) {
                break;
            }

            // do the move
            b.move(r);

            if (i >= min_ply) {

                // for this position, run a qsearch to collect all quiet positions
                std::vector<Board*> leafs;
                collectAllQuietPositions(&b, leafs);
                std::cout << "collected " << leafs.size() << " positions" << std::endl;

                // iterate over each position and run a quick search
                for (Board* b : leafs) {

                    double eval = evalPosition(b);
                    search_clearHash();

                    if (abs(eval) < adjudicate) {
                        (*outFile) << b->fen() << ";" << eval << "\n";
                        totalCount++;

                        if (totalCount % 100 == 0) {
                            std::cout << totalCount << std::endl;
                        }
                    }
                }
            }
        }
    }
}
Score generator::collectAllQuietPositions(Board* b, std::vector<Board*>& leafs, Score alpha, Score beta) {

    // if its a draw (3-fold), return.
    if (b->isDraw())
        return 0;
    
    double stand_pat = evaluator.evaluate(b);

    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;

    // create a movelist which contains
    MoveList mv {};

    // collect all non quiet moves (not regarding checks)
    b->getNonQuietMoves(&mv);

    // if there are no non quiet moves, consider this position quiet and return
    if (mv.getSize() == 0) {

        // we are at a leaf.

        if (searchedPosition->get(b->zobrist()).zobrist != 0)
            return alpha;

        searchedPosition->put(b->zobrist(), 0, 0, PV_NODE, 0);
        leafs.push_back(new Board(b));
        return alpha;
    }

    MoveOrderer orderer {};
    orderer.setMovesQSearch(&mv, b);

    // we loop over all moves to get those positions
    for (int i = 0; i < mv.getSize(); i++) {

        
        
        // get the current move
        Move m = orderer.next();
    
    
        if(b->staticExchangeEvaluation(m) < 0)
            continue;
        
        // dont do illegal moves
        if (!b->isLegal(m))
            continue;

        // do the move
        b->move(m);

        // collect all quiet positions for this position
        Score s = collectAllQuietPositions(b, leafs);

        if (s > beta)
            return beta;
        if (s > alpha)
            alpha = s;

        // undo the move
        b->undoMove();
    }
    return alpha;
}

// select a random move.
// if there is a king in the movelist, return a king move with a probability of king_walk_p
Move generator::selectRandomMove(MoveList& moveList, double king_walk_p) {
    
    
    // track all king moves
    MoveList kingMoves{};

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
        return moveList.getMove(static_cast<int>(static_cast<double>(rand()) / RAND_MAX * moveList.getSize()));
    }
    // select a move from the king moves
    else {
        return kingMoves.getMove(static_cast<int>(static_cast<double>(rand()) / RAND_MAX * kingMoves.getSize()));
    }
}

// gets the evaluation for the position
Score generator::evalPosition(Board* b) {
    TimeManager manager {};
    bestMove(b, 8, &manager);
    SearchOverview ov = search_overview();
    return ov.score;
}
