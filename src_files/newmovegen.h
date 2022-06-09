
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

#include "board.h"
#include "history.h"
#include "move.h"
#include "bitboard.h"

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

    move::Move      quiets[MAX_QUIET]       = {0};
    move::Move      noisy[MAX_NOISY]        = {0};
    move::Move      searched[MAX_QUIET]     = {0};
    int             quietScores[MAX_QUIET]  = {0};
    int             noisyScores[MAX_NOISY]  = {0};
    int             noisySee[MAX_NOISY]     = {0};
    int             quietSize;
    int             noisySize;
    int             goodNoisyCount;
    int             noisy_index;
    int             quiet_index;
    int             searched_index;
    
    bool            m_skip;
    Board*          m_board;
    SearchData*     m_sd;
    bb::Depth       m_ply;
    move::Move      m_hashMove;
    move::Move      m_killer1;
    move::Move      m_killer2;
    move::Move      m_previous;
    move::Move      m_followup;
    int*            m_th;
    int*            m_cmh;
    int*            m_fmh;
    bb::Square      m_threatSquare;
    bb::U64         m_checkerSq;
    bb::Color       c;
    int             m_mode;
    

    public:
    int lastSee;
    void                     init(SearchData* sd, Board* b, bb::Depth ply, move::Move hashMove, move::Move previous,
                                  move::Move followup, int mode, bb::Square threatSquare, bb::U64 checkerSq = 0);
    [[nodiscard]] move::Move next();
    void                     addNoisy(move::Move m);
    void                     addQuiet(move::Move m);
    [[nodiscard]] move::Move nextNoisy();
    [[nodiscard]] move::Move nextQuiet();
    void                     addSearched(move::Move m);
    void                     generateNoisy();
    void                     generateQuiet();
    void                     generateEvasions();
    void                     updateHistory(int weight);
    void                     skip();
    [[nodiscard]] bool       shouldSkip() const;
};

#endif