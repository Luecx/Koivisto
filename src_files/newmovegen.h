
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                                   by. Kim Kahre and Finn Eggers                                  *
 *                                                                                                  *
 *                 Koivisto is free software: you can redistribute it and/or modify                 *
 *               it under the terms of the GNU General Public License as published by               *
 *                 the Free Software Foundation, either version 3 of the License, or                *
 *                                (at your option) any later version.                               *
 *                    Koivisto is distributed in the hope that it will be useful,                   *
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 *                           GNU General Public License for more details.                           *
 *                 You should have received a copy of the GNU General Public License                *
 *                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
 *                                                                                                  *
 ****************************************************************************************************/

#ifndef KOIVISTO_NEWMOVEGEN_H
#define KOIVISTO_NEWMOVEGEN_H

#include "Board.h"
#include "History.h"
#include "Move.h"
#include "Bitboard.h"

constexpr int MAX_QUIET = 128;
constexpr int MAX_NOISY = 32;

constexpr int MAX_HIST  = 512;

enum {
    PV_SEARCH,
    Q_SEARCH,
    Q_SEARCHCHECK,
};

enum {
    GET_HASHMOVE,
    GEN_NOISY,
    GET_GOOD_NOISY,
    KILLER1,
    KILLER2,
    GEN_QUIET,
    GET_QUIET,
    GET_BAD_NOISY,
    END,
    QS_EVASIONS,
};

class moveGen {
    private:

    int             stage;

    Move            quiets[MAX_QUIET]       = {0};
    Move            noisy[MAX_NOISY]        = {0};
    Move            searched[MAX_QUIET]     = {0};
    int             quietScores[MAX_QUIET]  = {0};
    int             noisyScores[MAX_NOISY]  = {0};
    int             quietSize;
    int             noisySize;
    int             goodNoisyCount;
    int             noisy_index;
    int             quiet_index;
    int             searched_index;
    
    bool            m_skip;
    Board*          m_board;
    SearchData*     m_sd;
    Depth           m_ply;
    Move            m_hashMove;
    Move            m_killer1;
    Move            m_killer2;
    Move            m_previous;
    Move            m_followup;
    Square          m_threatSquare;
    U64             m_checkerSq;
    Color           c;
    int             m_mode;
    

    public: 
    void init(SearchData* sd, Board* b, Depth ply, Move hashMove, Move previous, Move followup, int mode, Square threatSquare, U64 checkerSq = 0);
    Move next();
    void addNoisy(Move m);
    void addQuiet(Move m);
    Move nextNoisy();
    Move nextQuiet();
    void addSearched(Move m);
    void generateNoisy();
    void generateQuiet();
    void generateEvasions();
    void updateHistory(int weight);
    void skip();
    bool shouldSkip();
};

#endif