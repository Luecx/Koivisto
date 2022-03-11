
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

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

int main(int argc, char *argv[]) {
    uci::init();
#ifndef __EMSCRIPTEN__
    std::string line;
    // read given commands from shell
    for(int i = 1; i < argc; i++){
        uci::processCommand(argv[i]);
        // OB requires us to give an exit command once the bench command is given
        if( strcmp(argv[i], "bench") == 0) {
            uci::processCommand("exit");
        }
    }

    while (std::getline(std::cin, line)) {
        uci::processCommand(line);
    }
#endif
    return 0;
}

#ifdef __EMSCRIPTEN__
#ifdef __cplusplus
extern "C" {
#endif
EMSCRIPTEN_KEEPALIVE void processCommand(const char *str){
    std::string command(str);
    uci::processCommand(command);
}
#ifdef __cplusplus
}
#endif
#endif
