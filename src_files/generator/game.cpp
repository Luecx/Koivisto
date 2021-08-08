
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
int         Game::adjudicationDrawCount;
int         Game::adjudicationWinCount;
int         Game::engineGameSearchDepth;
int         Game::gameHashSize;
std::string Game::wdlPath;
bool        Game::useTbAdjudication;
bool        Game::useWinAdjudication;
bool        Game::useDrawAdjudication;
bool        Game::useTbSearch;

void        Game::init(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    auto                     setParam = [&](std::string const& option, int def) {
        auto s = getValue(args, option);
        return s.size() ? std::stoi(s) : def;
    };

    wdlPath                    = getValue(args, "-tbpath");
    engineGameSearchDepth      = setParam("-depth", 9);
    gameHashSize               = setParam("-hash", 4);
    randomOpeningMoveCount     = setParam("-bookdepth", 10);
    adjudicationDrawScoreLimit = setParam("-drawscore", 20);
    adjudicationDrawCount      = setParam("-drawply", 12);
    adjudicationWinScoreLimit  = setParam("-winscore", 1000);
    adjudicationWinCount       = setParam("-winply", 4);
    useTbAdjudication          = false;
    useDrawAdjudication        = false;
    useWinAdjudication         = false;
    useTbSearch                = false;
    
    auto set_choice = 
    [&](bool& choice, std::string_view option)
    {
        std::string_view value = getValue(args, option.data());
        if (value.size())
            choice = value == "true";
    };  

    set_choice(useTbAdjudication, "-tbadjudicate");
    set_choice(useDrawAdjudication, "-drawadjudicate");
    set_choice(useWinAdjudication, "-winadjudicate");
    set_choice(useTbSearch, "-searchusetb");
    
    if (wdlPath == "" && (useTbAdjudication || useTbSearch))
    {
        std::cerr << "TB path not given" << std::endl;
        std::terminate();
    }
        
    if (wdlPath != "") 
    {
        const char* data = wdlPath.data();
        if (!tb_init(data))
            throw std::runtime_error("tb_init");
    }

    std::cout << std::boolalpha;
    std::cout << "TB Adjudication  : " << useTbAdjudication      << '\n';
    std::cout << "Win Adjudication : " << useWinAdjudication     << '\n';
    std::cout << "Draw Adjudication: " << useDrawAdjudication    << '\n';
    std::cout << "Search use TB    : " << useTbSearch            << '\n';
    std::cout << "TB Path          : " << wdlPath                << '\n';
    std::cout << "Depth            : " << engineGameSearchDepth  << '\n';
    std::cout << "Hash size        : " << gameHashSize           << '\n';
    std::cout << "Book depth       : " << randomOpeningMoveCount << '\n';
    
    if (useWinAdjudication)
    {
        std::cout << "Win adjudication CP : " << adjudicationWinScoreLimit << '\n';
        std::cout << "Win adjudication ply: " << adjudicationWinCount      << '\n';
    }

    if (useDrawAdjudication)
    {
        std::cout << "Draw adjudication CP : " << adjudicationDrawScoreLimit << '\n';
        std::cout << "Draw adjudication ply: " << adjudicationDrawCount      << '\n';
    }

    std::cout << std::endl;
}

Game::Game(std::ofstream& out, std::mt19937& generator)
    : m_currentPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
      m_outputBook(out),
      m_randomGenerator(generator) {
    m_searcher = {};
    m_searcher.init(gameHashSize);
    m_searcher.disableInfoStrings();

    m_useTb = !!wdlPath.size();
    m_searcher.useTableBase(useTbSearch);
}

bool Game::isDrawn() {
    return m_currentPosition.getCurrent50MoveRuleCount() >= 50
           || m_currentPosition.getCurrentRepetitionCount() >= 3;
}

bool Game::positionIsFavourable(Move) {
    return m_searcher.qSearch(&m_currentPosition) == m_currentPosition.evaluate();
}

bool Game::makeBookMove() {
    MoveList movelist;
    movelist.clear();

    generateLegalMoves(&m_currentPosition, &movelist);

    if (movelist.getSize() == 0) return false;

    std::uniform_int_distribution<> dist(0, movelist.getSize() - 1);

    int moveIndex = dist(m_randomGenerator);

    m_currentPosition.move(movelist.getMove(moveIndex));
    m_currentPly++;
    return true;
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
    Move best = m_searcher.bestMove(&m_currentPosition, engineGameSearchDepth, &tm);
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
                result = m_currentPosition.getActivePlayer() == WHITE ? "[0.0]" : "[1.0]";
            } else
                result = "[0.5]";

            break;
        }

        if (useTbAdjudication) {
            int wdlScore       = m_useTb ? m_searcher.probeWDL(&m_currentPosition) : TB_FAILED;
            if (std::abs(wdlScore) <= TB_CURSED_SCORE) {
                result           = "[0.5]";
                break;
            }
        
            if (std::abs(wdlScore) == TB_WIN_SCORE) {
                auto winningSide = whiteRelativeScore(&m_currentPosition, wdlScore) > 0 ? WHITE : BLACK;
                result           = winningSide == WHITE ? "[1.0]" : "[0.0]";
                break;
            }
        }
        
        auto [move, score] = searchPosition();
    
        // If this is the first move out of the book, discard
        // the game if score is above margin
        if ((   score >= adjudicationWinScoreLimit
                || m_currentPosition.getCurrentRepetitionCount() != 1)
            && m_currentPly == randomOpeningMoveCount) {
            return;
        }
        
        // check if we want the position (e.g. if its quiet)
        if (positionIsFavourable(move))
            savePosition(whiteRelativeScore(&m_currentPosition, score));

        bool scoreIsDraw = std::abs(score) <= adjudicationDrawScoreLimit;
        bool scoreIsWin  = std::abs(score) >= adjudicationWinScoreLimit;

        // Update draw/win score counters
        drawScoreCounter = scoreIsDraw ? drawScoreCounter + 1 : 0;
        winScoreCounter  = scoreIsWin  ? winScoreCounter + 1 : 0;
    
        // Adjudicate game
        if (useDrawAdjudication && drawScoreCounter >= adjudicationDrawCount) {
            result           = "[0.5]";
            break;
        }

        if (useWinAdjudication && winScoreCounter >= adjudicationWinCount) {
            auto winningSide = whiteRelativeScore(&m_currentPosition, score) > 0 ? WHITE : BLACK;
            result           = winningSide == WHITE ? "[1.0]" : "[0.0]";
            break;
        }
        
        // apply the move
        m_currentPosition.move(move);
        m_currentPly++;
    }
    saveGame(result);
}