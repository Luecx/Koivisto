/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "polyglot.h"
#include "position.h"
#include "board.h"
#include <fstream>

constexpr uint64_t keys_64[781]
{
#include "polyglotkeys.txt"
};

static uint64_t get_piece_key(Piece p, Square sq)
{
    constexpr int piece_conv[12]
    {
        1, 3, 5, 7, 9, 11, 
        0, 2, 4, 6, 8, 10
    };
    
    return keys_64[64 * piece_conv[p] + 8 * (int)rank_of(sq) + (int)file_of(sq)];
}

static uint64_t hash_pieces(Position const& position)
{
    uint64_t hash = 0;
    for(int i = 0;i < 64;i++)
    {
        Piece piece = position.pieces.squares[i];
        if (piece != Piece::Empty)
            hash ^= get_piece_key(piece, Square(i));
    }

    return hash;
}

static uint64_t hash_castle(Position const& position)
{
    constexpr int offset = 768;
    uint64_t hash = 0;
    
    uint64_t rights = position.castle_rights.data();

    if (test_bit(Square::G1, rights))
        hash ^= keys_64[offset + 0];

    if (test_bit(Square::C1, rights))
        hash ^= keys_64[offset + 1];
    
    if (test_bit(Square::G8 , rights))
        hash ^= keys_64[offset + 2];

    if (test_bit(Square::C8, rights))
        hash ^= keys_64[offset + 3];

    return hash;
}

static uint64_t hash_enpassant(Position const& position)
{
    uint64_t hash = 0;
    constexpr int offset = 772;

    if (position.ep_sq != bad_sq)
        hash ^= keys_64[offset + (int)file_of(position.ep_sq)];

    return hash;
}

static uint64_t hash_turn(Position const& position)
{
    if (position.side == White)
        return keys_64[780];

    return 0;
}

static uint64_t make_key(Position const& position)
{
    return hash_pieces(position) ^ hash_castle(position) ^ hash_turn(position) ^ hash_enpassant(position);
}

static Move decode_move(Position const& position, uint16_t move)
{
    if (!move)
        return NullMove;

    int to_file         = (move & 0x7);
    int to_row          = (move & 0x38)   >> 3;
    int from_file       = (move & 0x1C0)  >> 6;
    int from_row        = (move & 0xE00)  >> 9;
    int promoted        = (move & 0x7000) >> 12;

    Square from = Square(from_row * 8 + from_file);
    Square to   = Square(to_row * 8 + to_file);

    Piece moving = position.pieces.squares[from];

    if (type_of(moving) == PieceType::King)
    {
        if (from == E1 && to == H1)
            return CreateMove(E1, G1, MoveFlag::castle, 1);

        if (from == E1 && to == A1)
            return CreateMove(E1, C1, MoveFlag::castle, 1);

        if (from == E8 && to == H8)
            return CreateMove(E8, G8, MoveFlag::castle, 1);

        if (from == E8 && to == A8)
            return CreateMove(E8, C8, MoveFlag::castle, 1);
    }

    if (type_of(moving) == PieceType::Pawn && rank_of(from, position.side) == Rank::seven)
        return CreateMove(from, to, MoveFlag::promotion, promoted - 1);

    if (type_of(moving) == PieceType::Pawn && to == position.ep_sq)
        return CreateMove(from, to, MoveFlag::enpassant, 1);

    return CreateMove(from, to, MoveFlag::normal, 1);
}


namespace PolyGlot
{
    void Book::open(std::string_view path)
    {
        std::ifstream fil(path.data(), std::ios::binary | std::ios::ate);

        if (!fil)
        {
            std::cerr << "Couldn't open " << path << std::endl;
            return;
        }

        size_t size = fil.tellg();
        size_t count = size / sizeof(Entry);

        if (!count)
        {
            std::cerr << "No entries found in " << path << std::endl;
            return;
        }
        std::cout << count << " entries in " << path << std::endl;

        entries.clear();
        entries.reserve(count);

        fil.seekg(0);
        for(size_t i = 0;i < count;i++)
        {
            Entry entry;
            fil.read((char*)&entry, sizeof(Entry));
            entry.move   = __builtin_bswap16(entry.move);
            entry.weight = __builtin_bswap16(entry.weight);
            entry.learn  = __builtin_bswap32(entry.learn);
            entries.push_back(entry);
        }
    }

    Move Book::probe(Position const& position) const
    {
        uint64_t key = make_key(position);
        uint16_t best = 0;
        uint16_t move = 0;

        for(auto const& entry : entries)
        {
            if (key == __builtin_bswap64(entry.key) && entry.weight > best)
            {
                best = entry.weight;
                move = entry.move;
            }
        }
        return decode_move(position, move);
    }
}