//
// Created by finne on 5/31/2020.
//

#include "uci.h"


Board *board;

void uci_loop(){

    bb_init();
    search_init(128);
//    perft_init(false);
    
    board = new Board();
    
    
    std::atexit(uci_quit);
    std::string line;
    
    uci_processCommand("position startpos moves e2e4 a7a6 d2d4 a6a5 c2c4 a5a4 f2f4 a4a3 b2b3 b7b6 d4d5 b6b5 e4e5 b5c4 f1c4 c7c6 d5c6 d7c6 d1d8 e8d8 c1e3");
    
    
    std::cout << *board;
    printBitmap(board->getPieces()[BLACK_QUEEN]);
    printBitmap(board->getPieces()[WHITE_QUEEN]);
    
    
    while(true){
        getline(cin, line);
        
        if(line == "quit"){
            exit(0);
        }else{
            uci_processCommand(line);
        }
    }
}

void uci_uci() {
    std::cout << "id name Koivisto" << std::endl;
    std::cout << "id author Kim/Finn" << std::endl;
    std::cout << "option name Hash type spin default 128 min 1 max 4096" <<std::endl;
    
    
    std::cout << "uciok" << std::endl;
}

std::string uci_getValue(std::vector<std::string> &vec, std::string key){
    int index = 0;
    for(std::string s:vec){
        if(s == key){
            return vec.at(index+1);
        }
        index ++;
    }
    return "";
}

void uci_processCommand(std::string str) {
    
    str = trim(str);
    
    std::vector<std::string> split{};
    splitString(str, split, ' ');

//    std::cout << str << std::endl;
    
//    std::cout << "...";
//
//    for(string s:split){
//        std::cout << s << std::endl;
//    }
//    std::cout << "...";
    
    if (split.at(0) == "uci"){
        uci_uci();
    }else if (split.at(0) == "setoption") {
        
        string name = uci_getValue(split, "name");
        string value = uci_getValue(split, "value");
        
        std::cout<< "-" << name << "-  -"<<value<<"-"<<std::endl;
        
        uci_set_option(name, value);
    }else if (split.at(0) == "go"){
        if(str.find("wtime") != string::npos){
            string wtime = uci_getValue(split, "wtime");
            string btime = uci_getValue(split, "btime");
            string wincr = uci_getValue(split, "winc");
            string bincr = uci_getValue(split, "binc");
            string mvtog = uci_getValue(split, "movestogo");
            
            uci_go_match(
                    stoi(wtime),
                    stoi(btime),
                    stoi(wincr),
                    stoi(bincr),
                    (mvtog.empty()) ? 1000:stoi(mvtog));
        
        }else if(str.find("depth") != string::npos){
            uci_go_depth(stoi(uci_getValue(split, "depth")));
        }else if(str.find("nodes") != string::npos){
            uci_go_nodes(stoi(uci_getValue(split, "nodes")));
        }else if(str.find("movetime") != string::npos){
            uci_go_time(stoi(uci_getValue(split, "movetime")));
        }else if(str.find("infinite") != string::npos){
            uci_go_infinite();
        }else if(str.find("mate") != string::npos){
            uci_go_mate(stoi(uci_getValue(split, "mate")));
        }
    }else if(split.at(0) == "stop"){
        uci_stop();
    }else if(split.at(0) == "isready"){
        uci_isReady();
    }else if(split.at(0) == "debug"){
        uci_debug(uci_getValue(split, "debug") == "on");
    }else if(split.at(0) == "position"){
    
        auto fenPos = str.find("fen");
        auto movePos = str.find("moves");
        
        string moves{};
        if(movePos != string::npos){
            moves = str.substr(movePos+5);
            moves = trim(moves);
        }
        
        if(fenPos != string::npos){
            string fen = str.substr(fenPos+3, movePos);
            fen = trim(fen);
            uci_position_fen(fen, moves);
        }else{
            uci_position_startpos(moves);
        }
    }else if(split.at(0) == "print"){
        std::cout << *board << std::endl;
    }
}

void uci_go_match(int wtime, int btime, int winc, int binc, int movesToGo) {
    Move m = bestMove(board);
    std::cout << "bestmove " << toString(m) << std::endl;
}

void uci_go_depth(int depth) {

}

void uci_go_nodes(int nodes) {

}

void uci_go_time(int movetime) {

}

void uci_go_infinite() {

}

void uci_go_mate(int depth) {

}

void uci_stop() {

}

void uci_set_option(std::string name, std::string value) {
    if(name == "hash") search_setHashSize(stoi(value));
}

void uci_isReady() {
    //TODO check if its running
    std::cout << "readyok" << std::endl;
}

void uci_debug(bool mode) {

}

void uci_position_fen(std::string fen, std::string moves) {
    if(board != nullptr) delete board;
    board = new Board{fen};
    
//    std::cout << "-" << fen << "-" << std::endl;
//    std::cout << "-" << moves << "-" << std::endl;
    
    
    if(moves.empty()) return;
    std::vector<string> mv{};
    splitString(moves, mv, ' ');
    
    for(string s:mv){
        
        string str1 = s.substr(0,2);
        string str2 = s.substr(2,4);
        Square s1 = squareIndex(str1);
        Square s2 = squareIndex(str2);
        
        Piece moving = board->getPiece(s1);
        Piece captured = board->getPiece(s2);
        
        assert(moving >= 0);
        
        Type type;
        
        if(s.size() > 4){
            
            Piece promo;
            switch(s.at(4)){
                case 'q': promo = QUEEN; break;
                case 'r': promo = ROOK; break;
                case 'b': promo = BISHOP; break;
                case 'n': promo = KNIGHT; break;
            }
            
            if(captured >= 0){
                type = 11 + promo;
            }else{
                type = 7 + promo;
            }
            
        }else{
            if((moving%6) == KING){
                if(abs(s2-s1) == 2){
                    if(s2 > s1){
                        type = KING_CASTLE;
                    }else{
                        type = QUEEN_CASTLE;
                    }
                }else{
                    if(captured >= 0){
                        type = CAPTURE;
                    }else{
                        type = QUIET;
                    }
                }
            }else if((moving % 6) == PAWN){
                if(abs(s2-s1) == 16){
                    type = DOUBLED_PAWN_PUSH;
                }else if(abs(s2-s1) != 8){
                    if(captured >= 0){
                        type = CAPTURE;
                    }else{
                        type = EN_PASSANT;
                    }
                }else{
                    type = QUIET;
                }
            }else{
                if(captured >= 0){
                    type = CAPTURE;
                }else{
                    type = QUIET;
                }
            }
        }
        Move m = genMove(s1,s2,type, moving, captured);
        
        assert(board->isLegal(m));
        board->move(m);
    
    }
    
}

void uci_position_startpos(std::string moves) {
    uci_position_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", moves);
}

void uci_quit() {
    
    delete board;
    board = nullptr;
    
    
    bb_cleanUp();
    search_cleanUp();
    
//    perft_cleanUp();
}
