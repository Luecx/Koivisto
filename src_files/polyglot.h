
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

#pragma once
#include "board.h"

#include <string_view>
#include <vector>

namespace polyglot {
class Book {
    public:
    Book() = default;

    void                       open(std::string_view path);

    [[nodiscard]] move::Move   probe(const Board&) const;

    [[nodiscard]] size_t       size() const { return entries.size(); }

    bool                       enabled = false;

    private:
    struct Entry {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    };
    std::vector<Entry> entries;
};

inline Book book;
}    // namespace polyglot
