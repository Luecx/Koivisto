
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
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

#include "syzygy/tbprobe.h"

#include <fstream>
#include <iostream>
#include <thread>


TimeManager* timeManager;
Board*       board;
std::thread* searchThread = nullptr;

/**
 * the Main loop for received inputs from the user. Prints information about the engine (version, authors)
 * and continues reading the lines until 'quit' is parsed which will shut down the engine and deallocate arrays.
 * @param bench
 */
void uci_loop(bool bench) {

    bb_init();
    search_init(16);
    eval_init();

    if (bench) {
        uci_bench();

        search_cleanUp();
        bb_cleanUp();
    } else {
        std::cout << "Koivisto 64 " << MAJOR_VERSION << "." << MINOR_VERSION << " by K. Kahre, F. Eggers, E. Bruno"
                  << std::endl;

        board = new Board();

        std::atexit(uci_quit);
        std::string line;

        while (true) {
            getline(cin, line);

            if (line == "quit") {
                exit(0);
            } else {
                uci_processCommand(line);
            }
        }
    }
}

/**
 * Parses the uci command: uci
 * Displays engine version and the authors.
 * Also displays a list of all uci options which can be set. Finally, 'uciok' is sent back to receive further commands.
 */
void uci_uci() {
    std::cout << "id name Koivisto 64 " << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
    std::cout << "id author K. Kahre, F. Eggers, E. Bruno" << std::endl;
    std::cout << "option name Hash type spin default 16 min 1 max " << maxTTSize() << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max " << MAX_THREADS << std::endl;
    std::cout << "option name SyzygyPath type string default" << std::endl;

    std::cout << "uciok" << std::endl;
}

/**
 * assuming the input to the engine has been split by spaces into the given vector, this function
 * retrieves the entry after the given key. e.g. [a, 10, b 20, 300] with key='b' will return 20 as a string.
 * @param vec
 * @param key
 * @return
 */
std::string uci_getValue(std::vector<std::string>& vec, std::string key) {
    int index = 0;
    for (std::string s : vec) {
        if (s == key) {
            return vec.at(index + 1);
        }
        index++;
    }
    return "";
}

/**
 * stops and deallocates the running thread. This can be used to abruptly stop the search although this usage
 * will not display uci info strings. On the other hand, once the search is finished, this function will be called.
 */
void uci_endThread() {
    if (searchThread != nullptr) {
        delete searchThread;
        searchThread = nullptr;
        delete timeManager;
        timeManager = nullptr;
    }
}

/**
 * this function will invoke the search with a given max depth and a time manager.
 * If no depth has been specified, it will be set to the maximum depth and the time manager will handle
 * all stop-management.
 *
 * @param maxDepth
 * @param p_timeManager
 */
void uci_searchAndPrint(Depth maxDepth, TimeManager* p_timeManager) {
    Move m = bestMove(board, maxDepth, p_timeManager);
    std::cout << "bestmove " << toString(m) << std::endl;
    uci_endThread();
}

/**
 * processes a single command.
 */
void uci_processCommand(std::string str) {

    // we trim all white spaces on both sides first
    str = trim(str);

    // next we need to split the input by spaces.
    std::vector<std::string> split {};
    splitString(str, split, ' ');

    // it is possible to output all uci inputs to the engine to a file. Useful for debugging as many
    // clients to not display the uci commands they sent to the engine themself.
    if (false) {
        ofstream myfile;
        myfile.open("input.debug", fstream::app);
        myfile << str << "\n";
        myfile.close();
    }

    if (split.at(0) == "ucinewgame") {
        search_clearHash();
    }
    if (split.at(0) == "uci") {
        uci_uci();
    } else if (split.at(0) == "setoption") {

        if (split.size() < 5)
            return;

        string name  = uci_getValue(split, "name");
        string value = uci_getValue(split, "value");

        uci_set_option(name, value);

    } else if (split.at(0) == "go") {
        if (str.find("wtime") != string::npos) {
            string wtime = uci_getValue(split, "wtime");
            string btime = uci_getValue(split, "btime");
            string wincr = uci_getValue(split, "winc");
            string bincr = uci_getValue(split, "binc");
            string mvtog = uci_getValue(split, "movestogo");
            string depth = uci_getValue(split, "depth");

            uci_go_match((wtime.empty()) ? 60000000 : stoi(wtime), (btime.empty()) ? 60000000 : stoi(btime),
                         (wincr.empty()) ? 0 : stoi(wincr), (bincr.empty()) ? 0 : stoi(bincr),
                         (mvtog.empty()) ? 35 : stoi(mvtog), (depth.empty()) ? MAX_PLY : stoi(depth));

        } else if (str.find("depth") != string::npos) {
            uci_go_depth(stoi(uci_getValue(split, "depth")));
        } else if (str.find("nodes") != string::npos) {
            uci_go_nodes(stoi(uci_getValue(split, "nodes")));
        } else if (str.find("movetime") != string::npos) {
            uci_go_time(stoi(uci_getValue(split, "movetime")));
        } else if (str.find("infinite") != string::npos) {
            uci_go_infinite();
        } else if (str.find("mate") != string::npos) {
            uci_go_mate(stoi(uci_getValue(split, "mate")));
        } else if (str.find("perft") != string::npos) {
            uci_go_perft(stoi(uci_getValue(split, "perft")), str.find("hash") != string::npos);
        }
    } else if (split.at(0) == "stop") {
        uci_stop();
    } else if (split.at(0) == "isready") {
        uci_isReady();
    } else if (split.at(0) == "debug") {
        uci_debug(uci_getValue(split, "debug") == "on");
    } else if (split.at(0) == "setvalue") {
        if (str.find("FUTILITY_MARGIN") != string::npos) {
            FUTILITY_MARGIN = stoi(uci_getValue(split, "FUTILITY_MARGIN"));
        }
        if (str.find("RAZOR_MARGIN") != string::npos) {
            RAZOR_MARGIN = stoi(uci_getValue(split, "RAZOR_MARGIN"));
        }
        if (str.find("SE_MARGIN_STATIC") != string::npos) {
            SE_MARGIN_STATIC = stoi(uci_getValue(split, "SE_MARGIN_STATIC"));
        }
        if (str.find("LMR_DIV") != string::npos) {
            LMR_DIV = stoi(uci_getValue(split, "LMR_DIV"));
        }
    } else if (split.at(0) == "position") {

        auto fenPos  = str.find("fen");
        auto movePos = str.find("moves");

        string moves {};
        if (movePos != string::npos) {
            moves = str.substr(movePos + 5);
            moves = trim(moves);
        }

        if (fenPos != string::npos) {
            string fen = str.substr(fenPos + 3);
            fen        = trim(fen);
            uci_position_fen(fen, moves);
        } else {
            uci_position_startpos(moves);
        }
    } else if (split.at(0) == "print") {
        std::cout << *board << std::endl;
    } else if (split.at(0) == "eval") {
        printEvaluation(board);
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
void uci_go_perft(int depth, bool hash) {

    perft_init(hash);

    startMeasure();
    auto nodes = perft(board, depth, true, true, hash);
    auto time  = stopMeasure();

    std::cout << "nodes: " << nodes << " nps: " << nodes / (time + 1) * 1000 << std::endl;

    perft_cleanUp();
}

/**
 * parses the uci command: go match wtime [wtime] btime [btime] winc [winc] binc [binc] ...
 * This function is the main starting point for the search if real games are played.
 * If movestogo has not been given, it hash already been adjusted to a fixed value of 40 above.
 * The depth is usually set to the maximum depth (128)
 * @param wtime
 * @param btime
 * @param winc
 * @param binc
 * @param movesToGo
 * @param depth
 */
void uci_go_match(int wtime, int btime, int winc, int binc, int movesToGo, int depth) {

    if (searchThread != nullptr) {
        return;
    }

    timeManager = new TimeManager(wtime, btime, winc, binc, movesToGo, board);

    searchThread = new std::thread(uci_searchAndPrint, depth, timeManager);
    searchThread->detach();
}

/**
 * parses the uci command: go depth [depth].
 * It does a search on the current position until the specified depth has been reached.
 * @param depth
 */
void uci_go_depth(int depth) {

    if (searchThread != nullptr) {
        return;
    }

    timeManager = new TimeManager();

    searchThread = new std::thread(uci_searchAndPrint, depth, timeManager);
    searchThread->detach();
}

/**
 * parsed the uci command: go nodes [nodecount]
 * Not yet implemented
 * @param nodes
 */
void uci_go_nodes(int nodes) {
    // TODO implement node limit
    std::cout << "go nodes " << nodes << " not supported" << std::endl;
}

/**
 * parses the uci command: go movetime [movetime].
 * It does a search on the current position until the specified movetime has been reached.
 * The search will not extend the time, even if critical positions are analysed.
 * @param depth
 */
void uci_go_time(int movetime) {
    if (searchThread != nullptr) {
        return;
    }

    timeManager = new TimeManager(movetime);

    searchThread = new std::thread(uci_searchAndPrint, MAX_PLY, timeManager);
    searchThread->detach();
}

/**
 * parses the uci command: go infinite
 * It does a search on the current position until night and dawn have passed.
 */
void uci_go_infinite() { uci_go_depth(MAX_PLY); }

/**
 * parsed the uci command: go mate [mate_in_ply]
 * Not yet implemented
 * @param nodes
 */
void uci_go_mate(int depth) {

    // TODO implement mate search
    std::cout << "go mate " << depth << " not supported" << std::endl;
}

/**
 * parses the uci command: stop
 * stops the current search. This will usually print a last info string and the best move.
 */
void uci_stop() { search_stop(); }

/**
 * parses the uci command: setoption name [name] value [value]
 * Sets internal search options.
 * - SyzygyPath
 * - Threads
 * - Hash
 * @param name
 * @param value
 */
void uci_set_option(std::string& name, std::string& value) {
    if (name == "Hash") {
        search_setHashSize(stoi(value));
    } else if (name == "SyzygyPath") {
        if (value.empty())
            return;

        char path[value.length()];
        strcpy(path, value.c_str());
        tb_init(path);

        std::cout << "using syzygy table with " << TB_LARGEST << " m_pieces" << std::endl;

        /*
         * only use TB if loading was successful
         */
        search_useTB(TB_LARGEST > 0);
    } else if (name == "Threads") {
        int count           = stoi(value);
        int processor_count = (int) std::thread::hardware_concurrency();
        if (processor_count == 0)
            processor_count = MAX_THREADS;
        if (processor_count < count)
            count = processor_count;
        if (count < 1)
            count = 1;
        if (count > MAX_THREADS)
            count = MAX_THREADS;

        threadCount = count;
    }
}

/**
 * parses the uci command: isready
 * The engine is supposed to return readyok after checking that its ready to receive further commands.
 */
void uci_isReady() {
    // TODO check if its running

    std::cout << "readyok" << std::endl;
}

/**
 * parses the non-uci command: debug [true, false]
 * it is possible to enable debugging soon.
 * @param mode
 */
void uci_debug(bool mode) {
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
void uci_position_fen(std::string fen, std::string moves) {
    if (board != nullptr)
        delete board;
    board = new Board {fen};

    if (moves.empty())
        return;
    std::vector<string> mv {};
    splitString(moves, mv, ' ');

    for (string s : mv) {

        string str1 = s.substr(0, 2);
        string str2 = s.substr(2, 4);
        Square s1   = squareIndex(str1);
        Square s2   = squareIndex(str2);

        Piece moving   = board->getPiece(s1);
        Piece captured = board->getPiece(s2);

        assert(moving >= 0);

        Type type;

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
            if ((moving % 6) == KING) {
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
            } else if ((moving % 6) == PAWN) {
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

        assert(board->isLegal(m));
        board->move(m);
    }
}

/**
 * parses the uci command: position fen [fen] moves [m1, m2,...]
 * If the fen is not specified, the start position will be used which can also be invoked using 'startpos' instead of
 * fen ...
 * @param fen
 * @param moves
 */
void uci_position_startpos(std::string moves) {
    uci_position_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", moves);
}

/**
 * cleans up all allocated data
 */
void uci_quit() {

    delete board;
    board = nullptr;

    bb_cleanUp();
    search_cleanUp();
}

/**
 * performs a bench
 */
void uci_bench() {

    // positions from Ethereal
    static const char* Benchmarks[] = {
#include "bench.csv"

        ""};

    int nodes = 0;
    int time  = 0;

    search_disable_infoStrings();
    for (int i = 0; strcmp(Benchmarks[i], ""); i++) {

        Board b(Benchmarks[i]);

        TimeManager manager;
        bestMove(&b, 13, &manager, 0);
        SearchOverview overview = search_overview();

        nodes += overview.nodes;
        time += overview.time;

        printf("Bench [# %2d] %5d cp  Best:%6s  %12d nodes %8d nps", i + 1, overview.score,
               toString(overview.move).c_str(), (int) overview.nodes,
               (int) (1000.0f * overview.nodes / (overview.time + 1)));
        std::cout << std::endl;

        search_clearHash();
    }
    search_enable_infoStrings();

    printf("OVERALL: %53d nodes %8d nps\n", (int) nodes, (int) (1000.0f * nodes / (time + 1)));
}
