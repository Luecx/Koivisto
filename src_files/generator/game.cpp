#ifdef GENERATOR
#include "game.h"

#include "../eval.h"
#include "../movegen.h"
#include "../search.h"
#include "../syzygy/tbprobe.h"
#include "../uci.h"

static void generateLegalMoves(Board* board, MoveList* movelist) {
    MoveList pseudolegal;
    pseudolegal.clear();
    generatePerftMoves(board, &pseudolegal);

    for (int i = 0; i < pseudolegal.getSize(); i++) {
        auto m = pseudolegal.getMove(i);
        if (board->isLegal(m))
            movelist->add(m);
    }
}

static int whiteRelativeScore(Board* board, int score) {
    return board->getActivePlayer() == WHITE ? score : -score;
}

int         Game::randomOpeningMoveCount;
int         Game::adjudicationWinScoreLimit;
int         Game::adjudicationDrawScoreLimit;
int         Game::ajudicationDrawCount;
int         Game::adjudicationWinCount;
int         Game::angineGameSearchDepth;
int         Game::gameHashSize;
std::string Game::wdlPath;

void        Game::init(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    auto                     setParam = [&](std::string const& option, int def) {
        auto s = getValue(args, option);
        return s.size() ? std::stoi(s) : def;
    };

    wdlPath                    = getValue(args, "-tbpath");
    angineGameSearchDepth      = setParam("-depth", 9);
    gameHashSize               = setParam("-hash", 4);
    randomOpeningMoveCount     = setParam("-bookdepth", 10);
    adjudicationDrawScoreLimit = setParam("-drawscore", 20);
    ajudicationDrawCount       = setParam("-drawply", 12);
    adjudicationWinScoreLimit  = setParam("-winscore", 1000);
    adjudicationWinCount       = setParam("-winply", 2);

    if (wdlPath != "") {
        const char* data = wdlPath.data();
        if (!tb_init(data))
            throw std::runtime_error("tb_init");
    }
}

Game::Game(std::ofstream& out)
    : m_currentPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
      m_outputBook(out) {
    m_searcher = {};
    m_searcher.init(16);
    m_searcher.disableInfoStrings();

    m_useTb = wdlPath.size();
}

bool Game::isDrawn() {
    return m_currentPosition.getCurrent50MoveRuleCount() >= 50
           || m_currentPosition.getCurrentRepetitionCount() >= 3;
}

bool Game::positionIsFavourable(Move) {
    return m_searcher.qSearch(&m_currentPosition) == m_currentPosition.evaluate();
}

void Game::makeBookMove() {
    MoveList movelist;
    movelist.clear();

    generateLegalMoves(&m_currentPosition, &movelist);

    int moveIndex = rand() % movelist.getSize();

    m_currentPosition.move(movelist.getMove(moveIndex));
    m_currentPly++;
}

bool Game::hasLegalLeft() {
    MoveList movelist;
    movelist.clear();

    generateLegalMoves(&m_currentPosition, &movelist);
    return movelist.getSize() != 0;
}

void Game::reset() {
    m_currentPly      = 0;
    m_currentPosition = Board();
    m_savedFens.clear();
    m_searcher.clearHash();
    m_searcher.clearHistory();
}

void Game::savePosition(int score) { m_savedFens.push_back({m_currentPosition.fen(), score}); }

std::tuple<Move, int> Game::searchPosition() {
    auto tm   = TimeManager();
    Move best = m_searcher.bestMove(&m_currentPosition, angineGameSearchDepth, &tm);
    return {best, m_searcher.overview().score};
}

void Game::saveGame(std::string_view result) {
    for (auto const& position : m_savedFens)
        m_outputBook << position.first << ' ' << result << ' ' << position.second << '\n';
}

void Game::run() {
    std::string result;
    int         drawScoreCounter = 0;
    int         winScoreCounter  = 0;

    while (true) {
        // Make first N random moves
        if (m_currentPly < randomOpeningMoveCount) {
            makeBookMove();
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
                result = m_currentPosition.getActivePlayer() == WHITE ? "[0.0]" : "[1.0]";
            } else
                result = "[0.5]";

            break;
        }

        auto [move, score] = searchPosition();

        int wdlScore       = m_useTb ? m_searcher.probeWDL(&m_currentPosition) : 0;

        // If this is the first move out of the book, discard
        // the game if score is above margin
        if ((   score >= adjudicationWinScoreLimit
             || m_currentPosition.getCurrentRepetitionCount() != 1)
            && m_currentPly == randomOpeningMoveCount) {
            return;
        }

        bool scoreIsDraw = std::abs(score) <= adjudicationDrawScoreLimit;
        bool scoreIsWin  = std::abs(score) >= adjudicationWinScoreLimit;

        // Update draw/win score counters
        drawScoreCounter = scoreIsDraw ? drawScoreCounter + 1 : 0;
        winScoreCounter  = scoreIsWin  ? winScoreCounter + 1 : 0;

        // Adjudicate game
        if (drawScoreCounter >= ajudicationDrawCount) {
            result = "[0.5]";
            break;
        }
        
        if (std::abs(wdlScore) <= TB_CURSED_SCORE) {
            result           = "[0.5]";
            break;
        }
        
        if (std::abs(wdlScore) == TB_WIN_SCORE) {
            auto winningSide = whiteRelativeScore(&m_currentPosition, wdlScore) > 0 ? WHITE : BLACK;
            result           = winningSide == WHITE ? "[1.0]" : "[0.0]";
            break;
        }
        
        if (winScoreCounter >= adjudicationWinCount) {
            auto winningSide = whiteRelativeScore(&m_currentPosition, score) > 0 ? WHITE : BLACK;
            result           = winningSide == WHITE ? "[1.0]" : "[0.0]";
            break;
        }

        if (positionIsFavourable(move))
            savePosition(whiteRelativeScore(&m_currentPosition, score));

        m_currentPosition.move(move);
        m_currentPly++;
    }
    saveGame(result);
}
#endif