
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

#include "densenetwork.h"
#include "denseweights.h"
#include "uci.h"

int main(int argc, char* argv[]) {
    uci::mainloop(argc, argv);
    return 0;
    //    using namespace dense_network;
    //
    //    Network<16, 8, 1> network {LMR_NET_WEIGHTS};
    //    float             input[16] {8,0,0,9,-254 / 1024,1,-216 / 1024,0,0,0,1 / 1024,0,1,0,4,0};
    //
    //    std::cout << round(network.feed(input)[0]) << std::endl;
}
