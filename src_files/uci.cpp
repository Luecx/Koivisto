
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

#include "uci.h"

#include "attacks.h"
#include "polyglot.h"
#include "search.h"
#include "syzygy/tbprobe.h"
#include "timer.h"
#include "uciassert.h"
#include "infostring.h"

#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace bb;
using namespace move;


TimeManager timeManager{};
Board       board{};
Search      searchObject;
std::thread searchThread;

/**
 * assuming the input to the engine has been split by spaces into the given vector, this function
 * retrieves the entry after the given key. e.g. [a, 10, b 20, 300] with key='b' will return 20 as a
 * string.
 * @param vec
 * @param key
 * @return
 */
std::string getValue(const std::vector<std::string>& vec, const std::string& key) {
    int index = 0;
    for(const std::string& s : vec) {
        if (s == key) {
            return vec.at(index + 1);
        }
        index++;
    }
    return "";
}

/**
 * this function will invoke the search with a given max depth and a time manager.
 * If no depth has been specified, it will be set to the maximum depth and the time manager will
 * handle all stop-management.
 *
 * @param maxDepth
 * @param p_timeManager
 */
void searchAndPrint(TimeManager* p_timeManager) {
    Move m = searchObject.bestMove(&board, p_timeManager);
    std::cout << "bestmove " << toString(m) << std::endl;
}

/**
 * the Main loop for received inputs from the user. Prints information about the engine (version,
 * authors) and continues reading the lines until 'quit' is parsed which will shut down the engine and
 * deallocate arrays.
 * @param bench
 */
void uci::mainloop(int argc, char* argv[]) {
    attacks::init();
    bb::init();
    nn::init();
    searchObject = {};
    searchObject.init(16);
    
    
    std::cout << "Koivisto "
              << MAJOR_VERSION << "."
              << MINOR_VERSION
              << " by K. Kahre, F. Eggers"
              << std::endl;
    
    board = Board();
    std::atexit(uci::quit);
    std::string line;

    // read given commands from shell
    for(int i = 1; i < argc; i++){
        processCommand(argv[i]);
        // OB requires us to give an exit command once the bench command is given
        if( strcmp(argv[i], "bench") == 0) {
            processCommand("exit");
        }
    }
    
    while (std::getline(std::cin, line)) {
        uci::processCommand(line);
    }
}

/**
 * Parses the uci command: uci
 * Displays engine version and the authors.
 * Also displays a list of all uci options which can be set. Finally, 'uciok' is sent back to receive further commands.
 */
void uci::uci() {
    std::cout << "id name Koivisto " << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
    std::cout << "id author K. Kahre, F. Eggers" << std::endl;
    std::cout << "option name Hash type spin default 16 min 1 max " << maxTTSize() << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max " << MAX_THREADS << std::endl;
    std::cout << "option name OwnBook type check default false" << std::endl;
    std::cout << "option name BookPath type string" << std::endl;
    std::cout << "option name SyzygyPath type string default" << std::endl;
    std::cout << "option name MoveOverhead type spin default 0 min 0 max 10000" << std::endl;
    std::cout << "option name MoveOverheadType type combo default PerMove var PerMove var PerGame" << std::endl;
    std::cout << "uciok" << std::endl;
}

/**
 * computes the evaluation and if specified a detailed analysis of how much each piece is worth
 * according to the static neural network evaluation
 */
void uci::eval() {
    nn::Evaluator evaluator{};
    evaluator.reset(&board);
    
    auto base_eval = evaluator.evaluate(board.getActivePlayer());
    std::cout << "eval=" << base_eval << std::endl;
    
    
    const std::string h_sep = "+-------+-------+-------+-------+-------+-------+-------+-------+";
    const std::string e_sep = "|       |       |       |       |       |       |       |       |";
    const std::string empty = "|       ";
    
    std::cout << h_sep << "\n";
    
    for (Rank r = 7; r >= 0; r--) {
        
        for (File f = 0; f <= 7; ++f) {
            const Square sq = bb::squareIndex(r, f);
            const Piece  pc = board.getPiece(sq);
            
            if(pc < 0){
                std::cout << empty;
            }else{
                std::cout << "|   " << PIECE_IDENTIFIER[pc] << "   ";
            }
        }
        
        std::cout << "|\n";
        
        for (File f = 0; f <= 7; ++f) {
            const Square sq = bb::squareIndex(r, f);
            const Piece  pc = board.getPiece(sq);
            
            if(pc >= 0 && getPieceType(pc) != KING){
                
                board.unsetPiece(sq);
                auto eval = board.evaluate();
                board.setPiece(sq, pc);
                
                auto diff = base_eval - eval;
                auto diff_string = std::to_string(diff);
                
                auto l_zeros = (7 - diff_string.size()) / 2;
                auto r_zeros = (7 - diff_string.size() - l_zeros);
                
                std::cout << "|";
                for(size_t i = 0; i < l_zeros; i++){
                    std::cout << " ";
                }
                std::cout << diff;
                for(size_t i = 0; i < r_zeros; i++){
                    std::cout << " ";
                }
            }
            else{
                std::cout << empty;
            }
        }
        
        std::cout << "|\n" << h_sep << "\n";
    }
    std::cout << "fen: " << board.fen() << std::endl;
    
}

/**
 * processes a single command.
 */
void uci::processCommand(std::string str) {
    // we trim all white spaces on both sides first
    str = trim(str);

    // next we need to split the input by spaces.
    std::vector<std::string> split = splitString(str);

    if (split.at(0) == "ucinewgame") {
        searchObject.clearHash();
        searchObject.clearHistory();
    }
    if (split.at(0) == "uci") {
        uci::uci();
    } else if (split.at(0) == "setoption") {
        if (split.size() < 5)
            return;

        const std::string name  = getValue(split, "name");
        const std::string value = getValue(split, "value");

        uci::set_option(name, value);
    } else if (split.at(0) == "go") {
        go(split, str);
    } else if (split.at(0) == "stop") {
        uci::stop();
    } else if (split.at(0) == "isready") {
        uci::isReady();
    } else if (split.at(0) == "debug") {
        uci::debug(getValue(split, "debug") == "on");
    } else if (split.at(0) == "setvalue") {
        if (str.find("FUTILITY_MARGIN") != std::string::npos) {
            FUTILITY_MARGIN = std::stoi(getValue(split, "FUTILITY_MARGIN"));
        }
        if (str.find("RAZOR_MARGIN") != std::string::npos) {
            RAZOR_MARGIN = std::stoi(getValue(split, "RAZOR_MARGIN"));
        }
        if (str.find("SE_MARGIN_STATIC") != std::string::npos) {
            SE_MARGIN_STATIC = std::stoi(getValue(split, "SE_MARGIN_STATIC"));
        }
        if (str.find("LMR_DIV") != std::string::npos) {
            LMR_DIV = std::stoi(getValue(split, "LMR_DIV"));
            initLMR();
        }
    } else if (split.at(0) == "position") {
        auto fenPos  = str.find("fen");
        auto movePos = str.find("moves");

        std::string moves;
        if (movePos != std::string::npos) {
            moves = str.substr(movePos + 5);
            moves = trim(moves);
        }

        if (fenPos != std::string::npos) {
            std::string fen = str.substr(fenPos + 3);
            fen             = trim(fen);
            uci::position_fen(fen, moves);
        } else {
            uci::position_startpos(moves);
        }
    } else if (split.at(0) == "print") {
        std::cout << board << std::endl;
    } else if (split.at(0) == "eval") {
        uci::eval();
        
    } else if (split.at(0) == "bench"){
        bench();
    } else if (split.at(0) == "exit" || split.at(0) == "quit"){
        exit(0);
    } else if (split.at(0) == "-i"){
        usePrettyInfoString = true;
    }
}

/**
 * parses the uci command: go perft [depth].
 * It is also possible to specify the hash usage like: go perft 6 hash.
 * A hash size of 128 will be used which cannot be adjusted.
 *
 * @param depth
 * @param hash
 */
void uci::go_perft(int depth, bool hash) {
    perft_init(hash);
    
    Timer timer{};
    timer.tick();
    auto nodes = perft(&board, depth, true, true, hash);
    timer.tock();
    auto time = timer.elapsed();
    
    std::cout << "nodes: " << nodes << " nps: " << nodes / (time + 1) * 1000 << std::endl;

    perft_cleanUp();
}

/**
 * parses the uci command: stop
 * stops the current search. This will usually print a last info string and the best move.
 */
void uci::stop() {
    searchObject.stop();
    if (searchThread.joinable()) {
        searchThread.join();
    }
}

/**
 * parses the uci command: setoption name [name] value [value]
 * Sets internal search options.
 * - SyzygyPath
 * - Threads
 * - Hash
 * @param name
 * @param value
 */
void uci::set_option(const std::string& name, const std::string& value) {
    if (name == "Hash") {
        searchObject.setHashSize(stoi(value));
    } else if (name == "SyzygyPath") {
        if (value.empty())
            return;

        char path[value.length()];
        strcpy(path, value.c_str());
        tb_init(path);

        std::cout << "using syzygy table with " << TB_LARGEST << " pieces" << std::endl;

        /*
         * only use TB if loading was successful
         */
        searchObject.useTableBase(TB_LARGEST > 0);
    } else if (name == "Threads") {
        int count           = stoi(value);
        searchObject.setThreads(count);
    } else if (name == "OwnBook") {
        polyglot::book.enabled = (value == "true");
    } else if (name == "BookPath") {
        polyglot::book.open(value);
    } else if (name == "MoveOverhead"){
        timeManager.setMoveOverhead(std::stoi(value));
    } else if (name == "MoveOverheadType"){
        if(value == "PerMove"){
            timeManager.setMoveOverheadType(PER_MOVE);
        }
        if(value == "PerGame"){
            timeManager.setMoveOverheadType(PER_GAME);
        }
    }
}

/**
 * parses the uci command: isready
 * The engine is supposed to return readyok after checking that its ready to receive further commands.
 */
void uci::isReady() {
    std::cout << "readyok" << std::endl;
}

/**
 * parses the non-uci command: debug [true, false]
 * it is possible to enable debugging soon.
 * @param mode
 */
void uci::debug(bool mode) {
    // TODO enable debug
    std::cout << "debug=" << mode << std::endl;
}

/**
 * parses the uci command: position fen [fen] moves [m1, m2,...]
 * If the fen is not specified, the start position will be used which can also be invoked using 'startpos' instead of
 * fen ...
 * @param fen
 * @param moves
 */
void uci::position_fen(const std::string& fen, const std::string& moves) {
    board = Board {fen};

    if (moves.empty())
        return;
    std::vector<std::string> mv = splitString(moves);

    for (std::string s : mv) {
        std::string str1 = s.substr(0, 2);
        std::string str2 = s.substr(2, 4);
        Square s1   = squareIndex(str1);
        Square s2   = squareIndex(str2);

        Piece moving   = board.getPiece(s1);
        Piece captured = board.getPiece(s2);

        MoveType type;

        if (s.size() > 4) {
            Piece promo = QUEEN;
            switch (s.at(4)) {
                case 'q': promo = QUEEN; break;
                case 'r': promo = ROOK; break;
                case 'b': promo = BISHOP; break;
                case 'n': promo = KNIGHT; break;
            }

            if (captured >= 0) {
                type = 11 + promo;
            } else {
                type = 7 + promo;
            }
        } else {
            if ((moving % 8) == KING) {
                if (abs(s2 - s1) == 2) {
                    if (s2 > s1) {
                        type = KING_CASTLE;
                    } else {
                        type = QUEEN_CASTLE;
                    }
                } else {
                    if (captured >= 0) {
                        type = CAPTURE;
                    } else {
                        type = QUIET;
                    }
                }
            } else if ((moving % 8) == PAWN) {
                if (abs(s2 - s1) == 16) {
                    type = DOUBLED_PAWN_PUSH;
                } else if (abs(s2 - s1) != 8) {
                    if (captured >= 0) {
                        type = CAPTURE;
                    } else {
                        type = EN_PASSANT;
                    }
                } else {
                    type = QUIET;
                }
            } else {
                if (captured >= 0) {
                    type = CAPTURE;
                } else {
                    type = QUIET;
                }
            }
        }
        Move m = genMove(s1, s2, type, moving, captured);

        UCI_ASSERT(board.isLegal(m));
        board.move(m);
    }
}

/**
 * parses the uci command: position fen [fen] moves [m1, m2,...]
 * If the fen is not specified, the start position will be used which can also be invoked using 'startpos' instead of
 * fen ...
 * @param fen
 * @param moves
 */
void uci::position_startpos(const std::string& moves) {
    uci::position_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", moves);
}

/**
 * cleans up all allocated data
 */
void uci::quit() {
    uci::stop();
    searchObject.cleanUp();
}

/**
 * performs a bench
 */
void uci::bench() {
    // positions from Ethereal
    static const char* Benchmarks[] = {
#include "bench.csv"
        ""};

    int nodes = 0;
    int time  = 0;

    searchObject.disableInfoStrings();
    for (int i = 0; strcmp(Benchmarks[i], ""); i++) {
        Board b(Benchmarks[i]);

        TimeManager manager{};
        manager.setDepthLimit(13);
        searchObject.bestMove(&b, &manager);
        SearchOverview overview = searchObject.overview();

        nodes += overview.nodes;
        time += overview.time;

        printf("Bench [# %2d] %5d cp  Best:%6s  %12d nodes %8d nps", i + 1, overview.score,
               toString(overview.move).c_str(), static_cast<int>(overview.nodes),
               static_cast<int>(1000.0f * overview.nodes / (overview.time + 1)));
        std::cout << std::endl;

        searchObject.clearHash();
        searchObject.clearHistory();
    }
    printf("OVERALL: %39d nodes %8d nps\n", static_cast<int>(nodes), static_cast<int>(1000.0f * nodes / (time + 1)));
    std::cout << std::flush;
    searchObject.enableInfoStrings();
}

/**
 * parses any go command
 * Format: go [option 1] [value] [option 2] [value] ....
 * possible options:
 *   - nodes
 *   - movetime
 *   - depth
 * Some commands will be assume to be some default values if one of the following is given:
 *   - wtime
 *   - btime
 *   - winc
 *   - binc
 *   - movestogo
 * @param split
 * @param str
 */
void uci::go(const std::vector<std::string>& split, const std::string& str) {
    // Stop any running search
    uci::stop();
    
    // check for perft first since it will not be working with the remaining options
    if (str.find("perft") != std::string::npos) {
        // If the command is perft, get the depth value and whether or not to use hash table
        int depth = stoi(getValue(split, "perft"));
        bool useHash = str.find("hash") != std::string::npos;
        uci::go_perft(depth, useHash);
        return;
    }
    
    // Reset the time manager
    timeManager.reset();
    
    // parse match time.
    // check if anything like wtime, btime, winc or binc is given
    if (   str.find("wtime") != std::string::npos
        || str.find("btime") != std::string::npos
        || str.find("winc")  != std::string::npos
        || str.find("binc")  != std::string::npos
        || str.find("binc")  != std::string::npos) {
        // Get the values of the options if they are given
        std::string wtime_str = getValue(split, "wtime");
        std::string btime_str = getValue(split, "btime");
        std::string wincr_str = getValue(split, "winc");
        std::string bincr_str = getValue(split, "binc");
        std::string mvtog_str = getValue(split, "movestogo");
        
        U64 wtime = (wtime_str.empty()) ? 60000000 : stoi(wtime_str);
        U64 btime = (btime_str.empty()) ? 60000000 : stoi(btime_str);
        U64 wincr = (wincr_str.empty()) ?        0 : stoi(wincr_str);
        U64 bincr = (bincr_str.empty()) ?        0 : stoi(bincr_str);
        int mvtog = (mvtog_str.empty()) ?       20 : stoi(mvtog_str);
        
        timeManager.setMatchTimeLimit(board.getActivePlayer() == WHITE ? wtime : btime,
                                      board.getActivePlayer() == WHITE ? wincr : bincr,
                                      mvtog);
    }
    if (str.find("depth") != std::string::npos) {
        timeManager.setDepthLimit(stoi(getValue(split, "depth")));
    }
    if (str.find("nodes") != std::string::npos) {
        timeManager.setNodeLimit (stoi(getValue(split, "nodes")));
    }
    if (str.find("movetime") != std::string::npos) {
        timeManager.setMoveTimeLimit(stoi(getValue(split, "movetime")));
    }
    if (str.find("infinite") != std::string::npos) {
        // don't do anything since infinite is assumed as default
    }
    if (str.find("mate") != std::string::npos) {
        // don't do anything since we don't support it
    }
    // start the search
    searchThread = std::thread(searchAndPrint, &timeManager);
}
