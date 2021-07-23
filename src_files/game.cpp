#include "game.hpp"
#include "movegen.h"
#include "search.h"

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

Game::Game()
    : m_CurrentPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
    m_Searcher = {};
    m_Searcher.init(16);
    m_Searcher.disableInfoStrings();
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
    std::ofstream OutputBook(GameOutputBook);

    if (!OutputBook)
        throw std::runtime_error("Couldn't open output file for saving game");
    
    for(auto const& position : m_SavedFens)
        OutputBook << position.first << result << position.second << '\n';

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

        // If this is the first move out of the book, discard
        // the game if score is above margin
        if (score >= AdjudicationWinScoreLimit && m_CurrentPly == RandomOpeningMoveCount)
            return;

        bool scoreIsDraw = std::abs(score) <= AdjudicationDrawScoreLimit;
        bool scoreIsWin  = std::abs(score) >= AdjudicationWinScoreLimit;

        // Update draw/win score counters 
        drawScoreCounter = scoreIsDraw ? drawScoreCounter + 1 : 0;
        winScoreCounter  = scoreIsWin  ? winScoreCounter  + 1 : 0;

        // Adjudicate game 
        if (drawScoreCounter >= AdjudicationDrawCount)
        {
            std::cout << "Game ends by adjudication\n";
            result = "[0.5]";
            break;
        }        

        if (winScoreCounter >= AdjudicationWinCount)
        {
            auto winningSide = whiteRelativeScore(&m_CurrentPosition, score) > 0 ? WHITE : BLACK;
            result = winningSide == WHITE ? "[1.0]"
                                          : "[0.0]";
            std::cout << "Game ends by adjudication\n";
            break;
        }

        if (positionIsFavourable(move))
            savePosition(score);
        
        m_CurrentPosition.move(move);
        m_CurrentPly++;
    }
    saveGame(result);
}