//
// Created by finne on 5/31/2020.
//

#include <fstream>
#include <thread>
#include <iostream>
#include "uci.h"
#include "syzygy/tbprobe.h"

#define MONTH (\
  __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? "01" : "06") \
: __DATE__ [2] == 'b' ? "02" \
: __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? "03" : "04") \
: __DATE__ [2] == 'y' ? "05" \
: __DATE__ [2] == 'l' ? "07" \
: __DATE__ [2] == 'g' ? "08" \
: __DATE__ [2] == 'p' ? "09" \
: __DATE__ [2] == 't' ? "10" \
: __DATE__ [2] == 'v' ? "11" \
: "12")
#define DAY (std::string(1,(__DATE__[4] == ' ' ?  '0' : (__DATE__[4]))) + (__DATE__[5]))
#define YEAR ((__DATE__[7]-'0') * 1000 + (__DATE__[8]-'0') * 100 + (__DATE__[9]-'0') * 10 + (__DATE__[10]-'0') * 1)


TimeManager *timeManager;
Board       *board;
std::thread *searchThread = nullptr;


void uci_loop(bool bench) {
    
    bb_init();
    search_init(16);
    
    if (bench) {
        uci_bench();
    }
    
    
    std::cout << "Koivisto 64 " << YEAR << MONTH << DAY << " by K. Kahre, F. Eggers" << std::endl;
    
    
    board = new Board();
    
    
    std::atexit(uci_quit);
    std::string line;
    
    while (true) {
        std::getline(std::cin, line);
        
        if (line == "quit") {
            exit(0);
        } else {
            uci_processCommand(line);
        }
    }
}

void uci_uci() {
    std::cout << "id name Koivisto 64 " << YEAR << MONTH << DAY << std::endl;
    std::cout << "id author K. Kahre, F. Eggers" << std::endl;
    std::cout << "option name Hash type spin default 16 min 1 max " << maxTTSize() << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max " << MAX_THREADS << std::endl;
    std::cout << "option name SyzygyPath type string default" << std::endl;
    
    std::cout << "uciok" << std::endl;
}

std::string uci_getValue(std::vector<std::string> &vec, const std::string &key) {
    int              index = 0;
    for (std::string s:vec) {
        if (s == key) {
            return vec.at(index + 1);
        }
        index++;
    }
    return "";
}


void uci_endThread() {
    if (searchThread != nullptr) {
        delete searchThread;
        searchThread = nullptr;
        delete timeManager;
        timeManager = nullptr;
    }
}

void uci_searchAndPrint(Depth maxDepth, TimeManager *p_timeManager) {
    Move m = bestMove(board, maxDepth, p_timeManager);
    std::cout << "bestmove " << toString(m) << std::endl;
    uci_endThread();
}

void uci_processCommand(std::string str) {
    
    str = trim(str);
    
    std::vector<std::string> split{};
    splitString(str, split, ' ');
    
    
    
    /**
     * used for input debugging to check what information has been given to the engine.
     */
    if (false) {
        std::ofstream myfile;
        myfile.open("input.debug", std::fstream::app);
        myfile << str << "\n";
        myfile.close();
    }
    
    
    if (split.at(0) == "ucinewgame") {
        search_clearHash();
    }
    if (split.at(0) == "uci") {
        uci_uci();
    } else if (split.at(0) == "setoption") {
        
        std::string name  = uci_getValue(split, "name");
        std::string value = uci_getValue(split, "value");
        
        //std::cout<< "-" << name << "-  -"<<value<<"-"<<std::endl;
        
        uci_set_option(name, value);
    } else if (split.at(0) == "go") {
        if (str.find("wtime") != std::string::npos) {
            std::string wtime = uci_getValue(split, "wtime");
            std::string btime = uci_getValue(split, "btime");
            std::string wincr = uci_getValue(split, "winc");
            std::string bincr = uci_getValue(split, "binc");
            std::string mvtog = uci_getValue(split, "movestogo");
            std::string depth = uci_getValue(split, "depth");
            
            uci_go_match(
                    (wtime.empty()) ? 60000000 : stoi(wtime),
                    (btime.empty()) ? 60000000 : stoi(btime),
                    (wincr.empty()) ? 0 : stoi(wincr),
                    (bincr.empty()) ? 0 : stoi(bincr),
                    (mvtog.empty()) ? 40 : stoi(mvtog),
                    (depth.empty()) ? MAX_PLY : stoi(depth));
            
        } else if (str.find("depth") != std::string::npos) {
            uci_go_depth(stoi(uci_getValue(split, "depth")));
        } else if (str.find("nodes") != std::string::npos) {
            uci_go_nodes(stoi(uci_getValue(split, "nodes")));
        } else if (str.find("movetime") != std::string::npos) {
            uci_go_time(stoi(uci_getValue(split, "movetime")));
        } else if (str.find("infinite") != std::string::npos) {
            uci_go_infinite();
        } else if (str.find("mate") != std::string::npos) {
            uci_go_mate(stoi(uci_getValue(split, "mate")));
        } else if (str.find("perft") != std::string::npos) {
            uci_go_perft(stoi(uci_getValue(split, "perft")), str.find("hash") != std::string::npos);
        }
    } else if (split.at(0) == "stop") {
        uci_stop();
    } else if (split.at(0) == "isready") {
        uci_isReady();
    } else if (split.at(0) == "debug") {
        uci_debug(uci_getValue(split, "debug") == "on");
    } else if (split.at(0) == "setvalue") {
        if (str.find("FUTILITY_MARGIN") != std::string::npos) {
            FUTILITY_MARGIN = stoi(uci_getValue(split, "FUTILITY_MARGIN"));
        }
        if (str.find("RAZOR_MARGIN") != std::string::npos) {
            RAZOR_MARGIN = stoi(uci_getValue(split, "RAZOR_MARGIN"));
        }
        if (str.find("SE_MARGIN_STATIC") != std::string::npos) {
            SE_MARGIN_STATIC = stoi(uci_getValue(split, "SE_MARGIN_STATIC"));
        }
        if (str.find("LMR_DIV") != std::string::npos) {
            LMR_DIV = stoi(uci_getValue(split, "LMR_DIV"));
        }
    } else if (split.at(0) == "position") {
        
        auto fenPos  = str.find("fen");
        auto movePos = str.find("moves");
        
        std::string moves{};
        if (movePos != std::string::npos) {
            moves = str.substr(movePos + 5);
            moves = trim(moves);
        }
        
        if (fenPos != std::string::npos) {
            std::string fen = str.substr(fenPos + 3);
            fen = trim(fen);
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

void uci_go_perft(int depth, bool hash) {
    
    
    perft_init(hash);
    
    startMeasure();
    auto nodes = perft(board, depth, true, true, hash);
    auto time  = stopMeasure();
    
    std::cout << "nodes: " << nodes << " nps: " << nodes / (time + 1) * 1000 << std::endl;
    
    perft_cleanUp();
}

void uci_go_match(int wtime, int btime, int winc, int binc, int movesToGo, int depth) {
    
    if (searchThread != nullptr) {
        return;
    }
    
    timeManager = new TimeManager(wtime, btime, winc, binc, movesToGo, board);
    
    searchThread = new std::thread(uci_searchAndPrint, depth, timeManager);
    searchThread->detach();
    
}

void uci_go_depth(int depth) {
    
    if (searchThread != nullptr) {
        return;
    }
    
    timeManager = new TimeManager();
    
    searchThread = new std::thread(uci_searchAndPrint, depth, timeManager);
    searchThread->detach();
}

void uci_go_nodes(int nodes) {
    //TODO implement node limit
    std::cout << "go nodes " << nodes << " not supported" << std::endl;
}

void uci_go_time(int movetime) {
    if (searchThread != nullptr) {
        return;
    }
    
    
    timeManager = new TimeManager(movetime);
    
    searchThread = new std::thread(uci_searchAndPrint, MAX_PLY, timeManager);
    searchThread->detach();
}

void uci_go_infinite() {
    uci_go_depth(MAX_PLY);
}

void uci_go_mate(int depth) {
    
    //TODO implement mate search
    std::cout << "go mate " << depth << " not supported" << std::endl;
}

void uci_stop() {
    search_stop();
}

void uci_set_option(const std::string &name, const std::string &value) {
    if (name == "Hash") {
        search_setHashSize(stoi(value));
    } else if (name == "SyzygyPath") {
        if (value.empty()) return;
        
        char path[value.length()];
        strcpy(path, value.c_str());
        tb_init(path);
        
        std::cout << "using syzygy table with " << TB_LARGEST << " pieces" << std::endl;
        
        /*
         * only use TB if loading was successful
         */
        search_useTB(TB_LARGEST > 0);
    }else if (name == "Threads"){
        int count = stoi(value);
        if (count<1) count = 1;
        if (count>MAX_THREADS) count = MAX_THREADS;

        threadCount = count;
    }
    
}

void uci_isReady() {
    //TODO check if its running
    
    std::cout << "readyok" << std::endl;
}

void uci_debug(bool mode) {
    //TODO enable debug
    std::cout << "debug=" << mode << std::endl;
}

void uci_position_fen(std::string fen, std::string moves) {
    if (board != nullptr) delete board;
    board = new Board{fen};
    
    if (moves.empty()) return;
    std::vector<std::string> mv{};
    splitString(moves, mv, ' ');
    
    for (std::string s:mv) {
        
        std::string str1 = s.substr(0, 2);
        std::string str2 = s.substr(2, 4);
        Square s1   = squareIndex(str1);
        Square s2   = squareIndex(str2);
        
        Piece moving   = board->getPiece(s1);
        Piece captured = board->getPiece(s2);
        
        assert(moving >= 0);
        
        Type type;
        
        if (s.size() > 4) {
            
            Piece promo = QUEEN;
            switch (s.at(4)) {
                case 'q':
                    promo = QUEEN;
                    break;
                case 'r':
                    promo = ROOK;
                    break;
                case 'b':
                    promo = BISHOP;
                    break;
                case 'n':
                    promo = KNIGHT;
                    break;
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

void uci_position_startpos(const std::string &moves) {
    uci_position_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", moves);
}

void uci_quit() {
    
    delete board;
    board = nullptr;
    
    
    bb_cleanUp();
    search_cleanUp();
    
    
}

void uci_bench() {
    
    //positions from Ethereal
    static const char *Benchmarks[] = {
#include "bench.csv"
            
            ""
    };
    
    
    int nodes = 0;
    int time  = 0;
    
    search_disable_inforStrings();
    for (int i = 0; strcmp(Benchmarks[i], ""); i++) {
        
        Board b(Benchmarks[i]);
        
        TimeManager manager;
        bestMove(&b, 13, &manager, 1);
        SearchOverview overview = search_overview();
        
        nodes += overview.nodes;
        time += overview.time;
        
        printf("Bench [# %2d] %5d cp  Best:%6s  %12d nodes %8d nps\n", i + 1, overview.score,
               toString(overview.move).c_str(), static_cast<int>(overview.nodes),
               static_cast<int>(1000.0f * overview.nodes / (overview.time + 1)));
        
        search_clearHash();
    }
    search_enable_infoStrings();
    
    printf("OVERALL: %53d nodes %8d nps\n", static_cast<int>(nodes), static_cast<int>(1000.0f * nodes / (time + 1)));
    
    
}
