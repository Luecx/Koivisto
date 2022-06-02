
#include "game.h"
#include "../eval.h"
#include "../movegen.h"
#include "../search.h"
#include "../syzygy/tbprobe.h"
#include "../uci.h"
#ifdef GENERATOR

// opening settings
int  Game::openingBookPly;
int  Game::recordPly;
int  Game::maxPly;

// adjudicate wins if the score is above x for n ply
bool Game::adjudicateWins;
int  Game::adjudicateWinPly;
int  Game::adjudicateWinScore;
    
// adjudicate draws if the score is below x for n ply
bool Game::adjudicateDraws;
int  Game::adjudicateDrawPly;
int  Game::adjudicateDrawScore;
    
// adjudicate tablebase scores
bool Game::adjudicateTB;
    
// search settings
int  Game::searchNodes;
int  Game::searchHash;
bool Game::searchTB;

void Game::init(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    auto setParam = [&](std::string const& option, int def) {
        auto s = getValue(args, option);
        return s.size() ? std::stoi(s) : def;
    };

    auto setChoice = [&](std::string_view option, bool def) {
        for (const std::string& s : args) {
            if (s == option) {
                return true;
            }
        }
        return def;
    };
    
    std::string tb_path        = getValue(args, "-tbpath");
    
    Game::searchNodes          = setParam ("-nodes", 5000);
    Game::searchHash           = setParam ("-hash", 4);
    Game::searchTB             = setChoice("-search_use_tb", false);
    
    Game::adjudicateTB         = setChoice("-adjudicate_tb", false);
    
    Game::adjudicateDraws      = setChoice("-adjudicate_draws", true);
    Game::adjudicateDrawPly    = setParam ("-adjudicate_draw_ply", 8);
    Game::adjudicateDrawScore  = setParam ("-adjudicate_draw_score", 8);
    
    Game::adjudicateWins       = setChoice("-adjudicate_wins", true);
    Game::adjudicateWinPly     = setParam ("-adjudicate_win_ply", 4);
    Game::adjudicateWinScore   = setParam ("-adjudicate_win_score", 1000);
    
    Game::openingBookPly       = setParam ("-opening_book_ply", 8);
    Game::recordPly            = setParam ("-record_ply", 16);
    Game::maxPly               = setParam ("-max_ply", 400);

    if (tb_path.empty() && (Game::adjudicateTB || Game::searchTB)) {
        std::cerr << "TB path not given" << std::endl;
        std::terminate();
    }

    if (!tb_path.empty()) {
        const char* data = tb_path.data();
        if (!tb_init(data))
            throw std::runtime_error("tb_init");
    }
    
    std::cout << " opening book ply       : " << Game::openingBookPly      << "\n";
    std::cout << " max ply                : " << Game::maxPly              << "\n";
    std::cout << " recordBookPly          : " << Game::recordPly           << "\n\n";
    
    std::cout << " search nodes           : " << Game::searchNodes         << "\n";
    std::cout << " search hash            : " << Game::searchHash          << "\n";
    std::cout << " search TB              : " << Game::searchTB            << "\n\n";
    
    std::cout << " adjudicate TB          : " << Game::adjudicateTB        << "\n\n";
    
    std::cout << " adjudicate Wins        : " << Game::adjudicateWins      << "\n";
    std::cout << " adjudicate Win Ply     : " << Game::adjudicateWinPly    << "\n";
    std::cout << " adjudicate Win Score   : " << Game::adjudicateWinScore  << "\n\n";
    
    std::cout << " adjudicate Draws       : " << Game::adjudicateDraws     << "\n";
    std::cout << " adjudicate Draw Ply    : " << Game::adjudicateDrawPly   << "\n";
    std::cout << " adjudicate Draw Score  : " << Game::adjudicateDrawScore << "\n\n";
    
    std::cout << " Tablebase Path / Pieces: " << tb_path << " / " << TB_LARGEST << "\n\n";
    
    std::cout << std::endl;
}


// generate a list of pseudo legal moves
static void generateLegalMoves(Board* board, move::MoveList* movelist) {
    move::MoveList pseudo_legal;
    pseudo_legal.clear();
    generatePerftMoves(board, &pseudo_legal);

    for (int i = 0; i < pseudo_legal.getSize(); i++) {
        auto m = pseudo_legal.getMove(i);
        if (board->isLegal(m))
            movelist->add(m);
    }
}

static int whiteRelativeScore(Board* board, int score) {
    return board->getActivePlayer() == bb::WHITE ? score : -score;
}

Game::Game(std::ofstream& out, std::mt19937& generator)
    : m_currentPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
      m_outputBook(out),
      m_randomGenerator(generator) {
    
    m_searcher = {};
    m_searcher.init(Game::searchHash);
    m_searcher.useTableBase(Game::searchTB);
    m_searcher.disableInfoStrings();
}

/**
 * checks if the game is drawn
 * @return
 */
bool Game::isDrawn() {
    return    m_currentPosition.getCurrent50MoveRuleCount() >= 50
           || m_currentPosition.getCurrentRepetitionCount() >= 3;
}

/**
 * checks if the move shall be recorded
 * @param move
 * @return
 */
bool Game::positionIsFavourable(move::Move move) {
    
    if(move::isCapture(move)) return false;
    
    if(m_currentPosition.isInCheck(bb::WHITE) ||
       m_currentPosition.isInCheck(bb::BLACK)) return false;
    
    return true;
}

/**
 * makes a book move. returns false if its not possible to make a move (maybe checkmate) ?
 * @return
 */
bool Game::makeBookMove() {
    
    // list to store all moves
    move::MoveList movelist{};
    movelist.clear();
    
    // generate all the moves
    generateLegalMoves(&m_currentPosition, &movelist);

    // return false if no moves available
    if (movelist.getSize() == 0) return false;

    // create a uniform distribution
    std::uniform_int_distribution<> dist(0, movelist.getSize() - 1);
    
    // use the generator to compute the move idnex
    int moveIndex = dist(m_randomGenerator);
//    int moveIndex = (rand()  + 2)% movelist.getSize();
    
    // do the move
    m_currentPosition.move(movelist.getMove(moveIndex));
    m_currentPly++;
    return true;
}

/**
 * checks if there are any legal moves left
 * @return
 */
bool Game::hasLegalLeft() {
    // list to store all moves
    move::MoveList movelist{};
    movelist.clear();

    // generate all legal moves
    generateLegalMoves(&m_currentPosition, &movelist);
    return movelist.getSize() != 0;
}

/**
 * resets and allows for a new game
 */
void Game::reset() {
    m_currentPly      = 0;
    m_currentPosition = Board();
    m_savedFens.clear();
    m_searcher.clearHash();
    m_searcher.clearHistory();
}

/**
 * writes the game to the saved fens which will later be written to the file
 * @param score
 */
void Game::savePosition(int score) { m_savedFens.emplace_back(m_currentPosition.fen(), score); }

/**
 * runs a search on the current position
 * @return
 */
std::tuple<move::Move, int> Game::searchPosition() {
    // create a new time manager and assign the node limit
    auto tm   = TimeManager();
    tm.setNodeLimit(Game::searchNodes);
//    tm.setDepthLimit(3);
    // get the best move and the score and return that
    move::Move best = m_searcher.bestMove(&m_currentPosition, &tm);
    return {best, m_searcher.overview().score};
}

/**
 * writes the game to the disk
 * @param result
 */
void Game::saveGame(std::string_view result) {
    for (auto const& position : m_savedFens)
        m_outputBook << position.first << ' ' << result << ' ' << position.second << '\n';
}

/**
 * runs a game
 */
void Game::run() {
    std::string result;
    int         drawScoreCounter = 0;
    int         winScoreCounter  = 0;

    while (m_currentPly < Game::maxPly) {
        
        // Make first N random moves
        if (m_currentPly < Game::openingBookPly) {
            if (!makeBookMove())
                break;
            continue;
        }

        // Check if game ends in repetition or by halfmoves rule
        if (isDrawn()) {
            result = "[0.5]";
            break;
        }
        
        // Check if game ends in stalemate / checkmate
        if (!hasLegalLeft()) {
            if (m_currentPosition.isInCheck(m_currentPosition.getActivePlayer())) {
                result = m_currentPosition.getActivePlayer() == bb::WHITE ? "[0.0]" : "[1.0]";
            } else
                result = "[0.5]";

            break;
        }

        if (Game::adjudicateTB) {
            int wdlScore       = m_useTb ? m_searcher.probeWDL(&m_currentPosition) : bb::TB_FAILED;
            if (std::abs(wdlScore) <= bb::TB_CURSED_SCORE) {
                result           = "[0.5]";
                break;
            }
        
            if (std::abs(wdlScore) == bb::TB_WIN_SCORE) {
                auto winningSide = whiteRelativeScore(&m_currentPosition, wdlScore) > 0 ? bb::WHITE : bb::BLACK;
                result           = winningSide == bb::WHITE ? "[1.0]" : "[0.0]";
                break;
            }
        }
        
        auto [move, score] = searchPosition();
    
        // If this is the first move out of the book, discard
        // the game if score is above margin
        if ((abs(score) >= Game::adjudicateWinScore
                || m_currentPosition.getCurrentRepetitionCount() != 1)
                && m_currentPly == Game::openingBookPly) {
            return;
        }
        
        // check if we want the position
        if (m_currentPly > Game::recordPly
            && positionIsFavourable(move)
            && std::abs(score) < (bb::TB_WIN_SCORE - bb::MAX_INTERNAL_PLY))
            savePosition(whiteRelativeScore(&m_currentPosition, score));
        
        bool scoreIsDraw = std::abs(score) <= Game::adjudicateDrawScore;
        bool scoreIsWin  = std::abs(score) >= Game::adjudicateWinScore;
        
        // Update draw/win score counters
        drawScoreCounter = scoreIsDraw ? drawScoreCounter + 1 : 0;
        winScoreCounter  = scoreIsWin  ? winScoreCounter  + 1 : 0;
    
        // Adjudicate game
        if (Game::adjudicateDraws && drawScoreCounter >= Game::adjudicateDrawPly) {
            result           = "[0.5]";
            break;
        }

        if (Game::adjudicateWins && winScoreCounter >= Game::adjudicateWinPly) {
            auto winningSide = whiteRelativeScore(&m_currentPosition, score) > 0 ? bb::WHITE : bb::BLACK;
            result           = winningSide == bb::WHITE ? "[1.0]" : "[0.0]";
            break;
        }
        
        // apply the move
        m_currentPosition.move(move);
        m_currentPly++;
    }
    saveGame(result);
}
#endif