//
// Created by finne on 5/31/2020.
//

#include "uci.h"

void uci_uci() {
    std::cout << "id name Koivisto" << std::endl;
    std::cout << "id author /Finn" << std::endl;
    std::cout << "option name Hash type spin default 128 min 1 max 4096" <<std::endl;
    
//    bb_init();
//    search_init(128);
//    perft_init(false);
    
    std::cout << "uciok" << std::endl;
}

void uci_processCommand(std::string str) {
    
    str = trim(str);
    
    if (str.rfind("setoption", 0) == 0) {
        
        int nameIndex = str.find("name");
        int valueIndex = str.find("value");
        
        string name = str.substr(nameIndex + 4, valueIndex);
        string value = str.substr(valueIndex+5);
        
        name = trim(name);
        value = trim(value);
        
        std::cout<< "-" << name << "-  -"<<value<<"-"<<std::endl;
        
        uci_set_option(name, value);
    }
}

void uci_go_match(int wtime, int btime, int winc, int binc, int movesToGo) {

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

}

void uci_position_startpos(std::string moves) {

}

void uci_quit() {

}
