#include "uci.h"
#include "game.h"
#include "movegen.h"
#include "search.h"
#include "syzygy/tbprobe.h"

static void generateLegalMoves(Board* board, MoveList* movelist)
{
    MoveList pseudolegal;
    pseudolegal.clear();
    generatePerftMoves(board, &pseudolegal);

    for(int i = 0;i < pseudolegal.getSize();i++)
    {
        auto m = pseudolegal.getMove(i);
        if (board->isLegal(m))
            movelist->add(m);
    }
}

static int whiteRelativeScore(Board* board, int score)
{
    return board->getActivePlayer() == WHITE ? score : -score;
}

int Game::RandomOpeningMoveCount;
int Game::AdjudicationWinScoreLimit;
int Game::AdjudicationDrawScoreLimit;
int Game::AdjudicationDrawCount;
int Game::AdjudicationWinCount;
int Game::EngineGameSearchDepth;
int Game::GameHashSize;
std::string Game::WDLPath;

void Game::init(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);

    auto setParam = 
    [&](std::string const& option, int def)
    {
        auto s = getValue(args, option);
        return s.size() ? std::stoi(s) : def;
    };

    WDLPath                   = getValue(args, "-tbpath");
    EngineGameSearchDepth     = setParam("-depth", 9);
    GameHashSize              = setParam("-hash" , 32);
    RandomOpeningMoveCount    = setParam("-bookdepth", 8);
    AdjudicationDrawScoreLimit= setParam("-drawscore", 20);
    AdjudicationDrawCount     = setParam("-drawply"  , 8);
    AdjudicationWinScoreLimit = setParam("-winscore" , 1000);
    AdjudicationWinCount      = setParam("-winply"   , 2);
}

Game::Game()
    : m_CurrentPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
    m_Searcher = {};
    m_Searcher.init(16);
    m_Searcher.disableInfoStrings();

    m_UseTb = WDLPath.size();

    if (WDLPath.size() && !tb_init(WDLPath.data()))
        throw std::runtime_error("Couldn't open given TB path");
}

bool Game::isDrawn()
{
    return m_CurrentPosition.getCurrent50MoveRuleCount() >= 25
        || m_CurrentPosition.getCurrentRepetitionCount() >= 2;
}

bool Game::positionIsFavourable(Move best)
{
    return !isCapture(best) && !m_CurrentPosition.isInCheck(m_CurrentPosition.getActivePlayer());
}

void Game::makeBookMove()
{
    MoveList movelist;
    movelist.clear();

    generateLegalMoves(&m_CurrentPosition, &movelist);

    int moveIndex = rand() % movelist.getSize();

    m_CurrentPosition.move(movelist.getMove(moveIndex));
    m_CurrentPly++;
}

bool Game::hasLegalLeft() 
{
    MoveList movelist;
    movelist.clear();

    generateLegalMoves(&m_CurrentPosition, &movelist);
    return movelist.getSize() != 0;
}

void Game::savePosition(int score)
{
    m_SavedFens.push_back({ m_CurrentPosition.fen(), score });
}

std::tuple<Move, int> Game::searchPosition()
{
    auto tm = TimeManager();
    Move best = m_Searcher.bestMove(&m_CurrentPosition, EngineGameSearchDepth, &tm);
    return { best, m_Searcher.overview().score };
}

void Game::saveGame(std::string_view result)
{
    std::ofstream OutputBook("generate_fens.txt", std::ios_base::app);

    if (!OutputBook)
        throw std::runtime_error("Couldn't open output file for saving game");
    
    for(auto const& position : m_SavedFens)
        OutputBook << position.first << ' ' << result << ' ' << position.second << '\n';

    OutputBook.close();
}

void Game::run()
{
    std::string result;
    int drawScoreCounter = 0;
    int winScoreCounter  = 0;
    
    while(true)
    {
        // Make first N random moves
        if (m_CurrentPly < RandomOpeningMoveCount)
        {
            makeBookMove();
            continue;
        }

        // Check if game ends in repetition or by halfmoves rule
        if (isDrawn())
        {
            result = "[0.5]";
            break;
        }

        // Check if game ends in stalemate / checkmate
        if (!hasLegalLeft())
        {
            if (m_CurrentPosition.isInCheck(m_CurrentPosition.getActivePlayer()))
            {
                result = m_CurrentPosition.getActivePlayer() == WHITE ? "[0.0]"
                                                                      : "[1.0]";
            }
            else 
                result = "[0.5]";

            break;
        }
        
        auto[move, score] = searchPosition();

        int wdlScore = m_UseTb ? m_Searcher.probeWDL(&m_CurrentPosition) : 0;

        // If this is the first move out of the book, discard
        // the game if score is above margin
        if (score >= AdjudicationWinScoreLimit && m_CurrentPly == RandomOpeningMoveCount)
        {
            return;
        }

        bool scoreIsDraw = std::abs(score) <= AdjudicationDrawScoreLimit;
        bool scoreIsWin  = std::abs(score) >= AdjudicationWinScoreLimit;

        // Update draw/win score counters 
        drawScoreCounter = scoreIsDraw ? drawScoreCounter + 1 : 0;
        winScoreCounter  = scoreIsWin  ? winScoreCounter  + 1 : 0;

        // Adjudicate game 
        if (drawScoreCounter >= AdjudicationDrawCount)
        {
            result = "[0.5]";
            break;
        }        

        if (winScoreCounter >= AdjudicationWinCount || std::abs(wdlScore) >= TB_WIN_SCORE)
        {
            auto winningSide = whiteRelativeScore(&m_CurrentPosition, score) > 0 ? WHITE : BLACK;
            result = winningSide == WHITE ? "[1.0]"
                                          : "[0.0]";
            break;
        }

        if (positionIsFavourable(move))
            savePosition(score);
        
        m_CurrentPosition.move(move);
        m_CurrentPly++;
    }
    saveGame(result);
}