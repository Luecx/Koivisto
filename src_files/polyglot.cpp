
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

#include "board.h"
#include "polyglot.h"
#include <fstream>

constexpr uint64_t keys_64[781]
{
#include "polyglotkeys.txt"
};

static int convert_piece(Piece p)
{
    switch(p)
    {
        case WHITE_PAWN: return 1;
        case WHITE_KNIGHT: return 3;
        case WHITE_BISHOP: return 5;
        case WHITE_ROOK: return 7;
        case WHITE_QUEEN: return 9;
        case WHITE_KING : return 11;

        case BLACK_PAWN: return 0;
        case BLACK_KNIGHT: return 2;
        case BLACK_BISHOP: return 4;
        case BLACK_ROOK: return 6;
        case BLACK_QUEEN: return 8;
        case BLACK_KING : return 10;
    }
    throw std::runtime_error("");
}

static uint64_t get_piece_key(Piece p, Square sq)
{
    return keys_64[64 * convert_piece(p) + sq];
}

static uint64_t hash_pieces(Board& board)
{
    uint64_t hash = 0;
    for(int i = 0;i < 64;i++)
    {
        Piece piece = board.getPiece(i);
        if (piece != -1)
            hash ^= get_piece_key(piece, Square(i));
    }

    return hash;
}

static uint64_t hash_castle(Board& board)
{
    constexpr int offset = 768;
    uint64_t hash = 0;
    
    if (board.getCastlingRights(WHITE_KINGSIDE_CASTLING))
        hash ^= keys_64[offset + 0];

    if (board.getCastlingRights(WHITE_QUEENSIDE_CASTLING))
        hash ^= keys_64[offset + 1];
    
    if (board.getCastlingRights(BLACK_KINGSIDE_CASTLING))
        hash ^= keys_64[offset + 2];

    if (board.getCastlingRights(BLACK_QUEENSIDE_CASTLING))
        hash ^= keys_64[offset + 3];

    return hash;
}

static uint64_t hash_enpassant(Board& board)
{
    uint64_t hash = 0;
    constexpr int offset = 772;

    if (board.getEnPassantSquare() != -1)
        hash ^= keys_64[offset + (board.getEnPassantSquare() & 7)];

    return hash;
}

static uint64_t hash_turn(Board& board)
{
    if (board.getActivePlayer() == WHITE)
        return keys_64[780];

    return 0;
}

static uint64_t make_key(Board& position)
{
    return hash_pieces(position) ^ hash_castle(position) ^ hash_turn(position) ^ hash_enpassant(position);
}

static Move decode_move(Board& board, uint16_t move)
{
    if (!move)
        return 0;

    int to_file         = (move & 0x7);
    int to_row          = (move & 0x38)   >> 3;
    int from_file       = (move & 0x1C0)  >> 6;
    int from_row        = (move & 0xE00)  >> 9;
    int promoted        = (move & 0x7000) >> 12;

    Square from = Square(from_row * 8 + from_file);
    Square to   = Square(to_row * 8 + to_file);
    Piece moving = board.getPiece(from);

    if (getPieceType(moving) == PieceTypes::KING)
    {
        if (from == E1 && to == H1)
            return genMove(E1, G1, KING_CASTLE, WHITE_KING);

        if (from == E1 && to == A1)
            return genMove(E1, C1, QUEEN_CASTLE, WHITE_KING);

        if (from == E8 && to == H8)
            return genMove(E8, G8, KING_CASTLE, BLACK_KING);

        if (from == E8 && to == A8)
            return genMove(E8, C8, QUEEN_CASTLE, BLACK_KING);
    }
    bool is_capture = board.getPiece(to) != -1;

    if (getPieceType(moving) == PAWN && (to_row == RANK_8 || to_row == RANK_7))
    {
        if (is_capture)
            return genMove(from, to, promoted + 11, moving, board.getPiece(to));
        else 
            return genMove(from, to, promoted + 7, moving);
    }

    if (getPieceType(moving) == PAWN && to == board.getEnPassantSquare())
    {
        return genMove(from, to, EN_PASSANT, moving);
    }

    if (is_capture)
        return genMove(from, to, CAPTURE, moving, board.getPiece(to));

    return genMove(from, to, QUIET, moving);
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

    Move Book::probe(Board& board) const
    {
        uint64_t key = make_key(board);
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
        return decode_move(board, move);
    }
}