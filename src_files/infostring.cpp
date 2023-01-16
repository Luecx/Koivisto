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

//
// Created by Luecx on 16.01.2023.
//

#include "infostring.h"

#include "move.h"
#include "movegen.h"
#include "search.h"
#include "syzygy/tbprobe.h"

using namespace bb;
using namespace move;


bool usePrettyInfoString = false;

void printInfoStringUCI(Search* search, bb::Depth depth, bb::Score score, PVLine& pvLine){
    if (!search->printInfo)
        return;
    
    // extract nodes, seldepth and nps
    U64 nodes       = search->totalNodes();
    U64 sel_depth   = search->selDepth();
    U64 tb_hits     = search->tbHits();
    U64 time        = search->timeManager->elapsedTime();
    U64 nps         = static_cast<U64>(nodes * 1000) /
                      static_cast<U64>(time + 1);
    U64 hashfull    = static_cast<int>(search->table->usage() * 1000);
    
    // print basic info string including depth and seldepth
    std::cout << "info"
              << " depth "          << static_cast<int>(depth)
              << " seldepth "       << static_cast<int>(sel_depth);
    
    // print the score. if its a mate score, show mate xx instead of cp xx
    if (abs(score) > MIN_MATE_SCORE) {
        std::cout << " score mate " << (MAX_MATE_SCORE - abs(score) + 1) / 2 * (score > 0 ? 1 : -1);
    } else {
        std::cout << " score cp "   << score;
    }
    // show tablebase hits if tablebase has been hit
    if (tb_hits != 0) {
        std::cout << " tbhits "     << tb_hits;
    }
    
    // show remaining information (nodes, nps, time, hash usage)
    std::cout << " nodes "          << nodes
              << " nps "            << nps
              << " time "           << time
              << " hashfull "       << hashfull;
    
    // print "pv" to shell
    std::cout << " pv";
    
    // go through each move in the PVLine
    for (int i = 0; i < pvLine.length; i++) {
        // transform move to a string and append it
        std::cout << " " << toString(pvLine(i));
    }
    
    // new line
    std::cout << std::endl;
}

void printInfoStringPretty(Search* search, bb::Depth depth, bb::Score score, PVLine& pvLine){
    if (!search->printInfo)
        return;
    
    // extract nodes, seldepth and nps
    U64 nodes       = search->totalNodes();
    U64 sel_depth   = search->selDepth();
    U64 tb_hits     = search->tbHits();
    U64 time        = search->timeManager->elapsedTime();
    U64 nps         = static_cast<U64>(nodes * 1000) /
                      static_cast<U64>(time + 1);
    
    float time_value;
    std::string time_specifier;
    
    float node_value;
    std::string node_specifier;
    int node_precision = 1;
    
    float score_value;
    std::string score_specifier;
    int score_precision = 2;
    
    // detect time format
    if(time < 1000){
        time_value = time;
        time_specifier = "ms";
    }else if(time < 1000 * 60){
        time_value = time / 1000.0f;
        time_specifier = "s";
    }else if(time < 1000 * 60 * 60){
        time_value = time / 1000.0f / 60.0f;
        time_specifier = "m";
    }else{
        time_value = time / 1000.0f / 60.0f / 60.0f;
        time_specifier = "h";
    }
    
    // detect node format
    if(nodes < 1e3){
        node_value = nodes;
        node_specifier = "n";
        node_precision = 0;
    }else if(nodes < 1e6){
        node_value = nodes / 1e3f;
        node_specifier = "kn";
    }else if(nodes < 1e9){
        node_value = nodes / 1e6f;
        node_specifier = "Mn";
    }else{
        node_value = nodes / 1e9f;
        node_specifier = "Gn";
    }
    
    // detect score format
    if (abs(score) > MIN_MATE_SCORE) {
        score_specifier = (score > 0 ? "+M" : "-M");
        score_precision = 0;
        score_value     = (MAX_MATE_SCORE - abs(score) + 1) / 2;
    } else if(abs(score) > TB_WIN_SCORE_MIN) {
        score_specifier = (score > TB_WIN_SCORE_MIN ? "+TB" : (score < -TB_WIN_SCORE_MIN ? "-TB" : "TBD"));
        score_precision = 0;
        score_value     = (TB_WIN_SCORE - abs(score) + 1) / 2;
    } else {
        score_specifier = score > 0 ? "+" : "";
        score_value = score / 100.0;
    }
    
    // convert time to string
    std::stringstream time_stream;
    time_stream << std::setprecision(3) << time_value;
    std::string time_string = time_stream.str() + time_specifier;
    
    // convert nodes to string
    std::stringstream node_stream;
    node_stream << std::fixed << std::setprecision(node_precision) << node_value;
    std::string node_string = node_stream.str() + node_specifier;
    
    // convert score to string
    std::stringstream score_stream;
    score_stream << std::fixed << std::setprecision(score_precision) << score_value;
    std::string score_string = score_specifier + score_stream.str();
    
    // print basic info string including depth and seldepth
    std::cout << std::setw(3)  << std::right << (int) depth << "/"
              << std::setw(3)  << std::left  << (int) sel_depth;
    std::cout << std::setw(8)  << std::right << time_string;
    std::cout << std::setw(10) << std::right << node_string;
    std::cout << std::setw(8)  << std::right << std::fixed   << std::setprecision(0) << nps / 1000.0 << "kn/s";
    std::cout << std::setw(8)  << std::right << score_string << "\t";
    
    for(int i = 0; i < pvLine.length; i++){
        std::cout << move::moveToSAN(pvLine(i)) << " ";
    }
    std::cout << std::endl;
}

void printInfoString(Search* search, bb::Depth depth, bb::Score score, PVLine& pvLine) {
    if(usePrettyInfoString){
        printInfoStringPretty(search, depth, score, pvLine);
    }else{
        printInfoStringUCI(search, depth, score, pvLine);
    }
}

Move printInfoStringDTZUCI(Search* search, Board* board, unsigned int tb_result, bb::Score score, int dtz){
    
    // get the promotion piece if the target move is a promotion (this does not yet work the way it
    // should)
    PieceType promo  = 5 - TB_GET_PROMOTES(tb_result);
    
    // gets the square from and square to for the move which should be played
    Square    sqFrom = TB_GET_FROM(tb_result);
    Square    sqTo   = TB_GET_TO(tb_result);
    
    // we generate all pseudo legal moves and check for equality between the moves to make sure the
    // bits are correct.
    MoveList mv {};
    generatePerftMoves(board, &mv);
    
    for (int i = 0; i < mv.getSize(); i++) {
        // get the current move from the movelist
        Move m = mv.getMove(i);

        // check if it's the same.
        if (getSquareFrom(m) == sqFrom && getSquareTo(m) == sqTo) {
            if (   (    promo == 5
                 && !isPromotion(m))
                || (isPromotion(m)
                    && promo < 5
                    && getPromotionPieceType(m) == promo)) {
                std::cout << "info"
                          << " depth "      << static_cast<int>(dtz)
                          << " seldepth "   << static_cast<int>(search->selDepth());
                std::cout << " score cp "   << score;

                if (search->tbHits() != 0) {
                    std::cout << " tbhits " << 1;
                }

                std::cout << " nodes "      << 1
                          << " nps "        << 1
                          << " time "       << search->timeManager->elapsedTime()
                          << " hashfull "   << static_cast<int>(search->table->usage() * 1000);
                std::cout << std::endl;

                return m;
            }
        }
    }
    return 0;
}

Move printInfoStringDTZPretty(Search* search, Board* board, unsigned int tb_result, bb::Score score, int dtz){
    // get the promotion piece if the target move is a promotion (this does not yet work the way it
    // should)
    PieceType promo  = 5 - TB_GET_PROMOTES(tb_result);
    
    // gets the square from and square to for the move which should be played
    Square    sqFrom = TB_GET_FROM(tb_result);
    Square    sqTo   = TB_GET_TO(tb_result);
    
    // we generate all pseudo legal moves and check for equality between the moves to make sure the
    // bits are correct.
    MoveList mv {};
    generatePerftMoves(board, &mv);
    
    for (int i = 0; i < mv.getSize(); i++) {
        // get the current move from the movelist
        Move m = mv.getMove(i);
        
        // check if it's the same.
        if (getSquareFrom(m) == sqFrom && getSquareTo(m) == sqTo) {
            if (   (    promo == 5
                 && !isPromotion(m))
                || (isPromotion(m)
                    && promo < 5
                    && getPromotionPieceType(m) == promo)) {
                
                std::cout << std::setw(3)  << std::right << (int) static_cast<int>(dtz) << "/"
                          << std::setw(3)  << std::left  << (int) static_cast<int>(dtz);
                std::cout << std::setw(8)  << std::right << search->timeManager->elapsedTime() << "ms";
                std::cout << std::setw(10) << std::right << 1;
                std::cout << std::setw(8)  << std::right << std::fixed   << std::setprecision(0) << 1 / 1000.0 << "n/s";
                std::cout << std::setw(8)  << std::right << score << "\t" << moveToSAN(m);
                std::cout << std::endl;
                return m;
            }
        }
    }
    return 0;
}

Move printInfoStringDTZ(Search* search, Board* board, unsigned int tb_result, bb::Score score, int dtz) {
    if(usePrettyInfoString){
        return printInfoStringDTZPretty(search, board, tb_result, score, dtz);
    }else{
        return printInfoStringDTZUCI(search, board, tb_result, score, dtz);
    }
}