

#include "Bitboard.h"

using namespace bb;

U64 ROOK_ATTACKS[64][4096] = {};
U64 BISHOP_ATTACKS[64][1024] = {};
U64 **all_hashes = {};
U64 **white_hashes = {};
U64 **black_hashes= {};

void bb::init() {
    
    std::cout << "generating bitboards...";
    
    std::srand(seed);
    
    auto start = std::chrono::system_clock::now();
    
    
    generateZobristKeys();
    generateData();
    
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end-start;
    
    std::cout << "       done! [" << round(diff.count() * 1000) << " ms]" << std::endl;
}

U64 bb::randU64(){
    U64 res{0};
    
    res ^= U64(rand()) << 0;
    res ^= U64(rand()) << 10;
    res ^= U64(rand()) << 20;
    res ^= U64(rand()) << 30;
    res ^= U64(rand()) << 40;
    res ^= U64(rand()) << 50;
    res ^= U64(rand()) << 60;
    
    return res;
}

U64 bb::generateSlidingAttacks(Square sq, Direction direction, U64 occ){
    U64 res {0};
    
    static const U64 topBottom = RANK_1 | RANK_8;
    static const U64 leftRight = FILE_A | FILE_H;
    
    if((1ULL << sq) & RANK_1 && direction < -2){
        return res;
    }if((1ULL << sq) & RANK_8 && direction > 2){
        return res;
    }if((1ULL << sq) & FILE_A && (direction == WEST || direction == SOUTH_WEST || direction == NORTH_WEST)){
        return res;
    }if((1ULL << sq) & FILE_H && (direction == EAST || direction == SOUTH_EAST || direction == NORTH_EAST)){
        return res;
    }
    
    while(true) {
        sq += direction;
        
        
        U64 currentSq = (U64)1 << sq;
        
        
        res |= currentSq;
        
        if (_and(occ, currentSq)) {
            return res;
        }
        if (abs(direction) == 8) {
            if (currentSq & topBottom) {
                return res;
            }
        } else if(abs(direction) == 1){
            if (currentSq & leftRight) {
                return res;
            }
        } else{
            if(currentSq & CIRCLE_A){
                return res;
            }
        }
        
    }
}


void bb::generateData() {
    for (int n = 0; n < 64; n++) {
        for(int i = 0; i < pow(2, 64 - rookShifts[n]); i++){
            U64 rel_occ = populateMask(rookMasks[n], i);
            int index = (int) ((rel_occ * rookMagics[n]) >> rookShifts[n]);
            
            ROOK_ATTACKS[n][index] = generateRookAttack(n, rel_occ);
        }
        
        for(int i = 0; i < pow(2, 64 - bishopShifts[n]); i++){
            U64 rel_occ = populateMask(bishopMasks[n], i);
            int index = (int) ((rel_occ * bishopMagics[n]) >> bishopShifts[n]);
            BISHOP_ATTACKS[n][index] = generateBishopAttack(n, rel_occ);
        }
        
    }
}

/**
 * populates the given bitboard with 1 and zeros.
 * The population is unique for the given index.
 * A 1 or 0 will only be placed where there is a 1 in the input.
 * @param mask
 * @param index
 * @return
 */
U64 bb::populateMask(U64 mask, U64 index) {
    
    U64 initMask{mask};
    
    U64 res = 0;
    Square i = 0;
    
    while (mask) {
        Square bit = bitscanForward(mask);
        
        if (getBit(index, i)){
            setBit(res, bit);
        }
        
        lsbReset(mask);
        i++;
    }
    /**
     * assert that no other bits can be set other than the masked bits
     */
    assert((res & (~initMask)) == 0);
    
    return res;
}

void bb::generateZobristKeys() {
    white_hashes = new U64*[6];
    black_hashes = new U64*[6];
    all_hashes = new U64*[12];
    for (int i = 0; i < 6; i++) {
        white_hashes[i] = new U64[64];
        black_hashes[i] = new U64[64];
        for (int n = 0; n < 64; n++) {
            white_hashes[i][n] = randU64();
            black_hashes[i][n] = randU64();
        }
        all_hashes[i] = white_hashes[i];
        all_hashes[i + 6] = black_hashes[i];
    }
}

U64 bb::getHash(Piece piece, Square sq) {
    return all_hashes[piece][sq];
}

U64 bb::lookUpRookAttack(Square index, U64 occupied) {
    return ROOK_ATTACKS[index][(int) ((occupied & rookMasks[index]) * rookMagics[index] >> (rookShifts[index]))];
    
    //return generateRookAttack(index, occupied);
}

U64 bb::lookUpBishopAttack(Square index, U64 occupied) {
    
    return BISHOP_ATTACKS[index][(int) ((occupied & bishopMasks[index]) *
                                        bishopMagics[index] >> (bishopShifts[index]))];
    //return generateBishopAttack(index, occupied);
}

U64 bb::generateRookAttack(Square sq, U64 occupied) {
    return generateSlidingAttacks(sq, NORTH, occupied) |
           generateSlidingAttacks(sq, EAST, occupied) |
           generateSlidingAttacks(sq, WEST, occupied) |
           generateSlidingAttacks(sq, SOUTH, occupied);
}


U64 bb::generateBishopAttack(Square sq, U64 occupied) {
    return generateSlidingAttacks(sq, NORTH_WEST, occupied) |
           generateSlidingAttacks(sq, NORTH_EAST, occupied) |
           generateSlidingAttacks(sq, SOUTH_WEST, occupied) |
           generateSlidingAttacks(sq, SOUTH_EAST, occupied);
}



void bb::printBitmap(U64 bb) {
    for (int i = 7; i >= 0; i--) {
        for (int n = 0; n < 8; n++) {
            if((bb >> (i * 8 + n)) & (U64)1){
                std::cout << "1";
            }else{
                std::cout << "0";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

Rank bb::rankIndex(Square &square_index) {
    return square_index >> 3;
}

File bb::fileIndex(Square &square_index) {
    return square_index & 7;
}

Square bb::squareIndex(Rank &rank, File &file) {
    return 8 * rank + file;
}

Square bb::squareIndex(std::string &str) {
    
    Rank r = str.at(1)- '1';
    File f = toupper(str.at(0))-'A';
    
    return squareIndex(r,f);
}

Diagonal bb::diagonalIndex(Square &square_index) {
    return 7 + rankIndex(square_index) - fileIndex(square_index);
}

AntiDiagonal bb::antiDiagonalIndex(Square &square_index) {
    return rankIndex(square_index) + fileIndex(square_index);
}

Diagonal bb::diagonalIndex(Rank &rank, File &file) {
    return 7 + rank - file;
}

AntiDiagonal bb::antiDiagonalIndex(Rank &rank, File &file) {
    return rank + file;
}

/**
 * toggles the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
void bb::toggleBit(U64 &number, Square &index) {
    number ^= (1ULL << index);
}

/**
 * set the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
void bb::setBit(U64 &number, Square &index) {
    number |= (1ULL << index);
}

/**
 * unset the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
void bb::unsetBit(U64 &number, Square &index) {
    number &= ~(1ULL << index);
}

/**
 * get the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
bool bb::getBit(U64 &number, Square &index) {
    return ((number >> index) & 1ULL) == 1;
}


/**
 * isolates the lsb:
 *
 *       x          &        -x         =     LS1B_of_x
 * . . . . . . . .     1 1 1 1 1 1 1 1     . . . . . . . .
 * . . 1 . 1 . . .     1 1 . 1 . 1 1 1     . . . . . . . .
 * . 1 . . . 1 . .     1 . 1 1 1 . 1 1     . . . . . . . .
 * . . . . . . . .     1 1 1 1 1 1 1 1     . . . . . . . .
 * . 1 . . . 1 . .  &  1 . 1 1 1 . 1 1  =  . . . . . . . .
 * . . 1 . 1 . . .     . . 1 1 . 1 1 1     . . 1 . . . . .
 * . . . . . . . .     . . . . . . . .     . . . . . . . .
 * . . . . . . . .     . . . . . . . .     . . . . . . . .
 *
 *
 * @param number
 * @return
 */
U64 &bb::lsbIsolation(U64 &number) {
    number &= -number;
    return number;
}

/**
 * resets the lsb:
 *
 *       x          &      (x-1)        =  x_with_reset_LS1B
 * . . . . . . . .     . . . . . . . .     . . . . . . . .
 * . . 1 . 1 . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . 1 . . . 1 . .     . 1 . . . 1 . .     . 1 . . . 1 . .
 * . . . . . . . .     . . . . . . . .     . . . . . . . .
 * . 1 . . . 1 . .  &  . 1 . . . 1 . .  =  . 1 . . . 1 . .
 * . . 1 . 1 . . .     1 1 . . 1 . . .     . . . . 1 . . .
 * . . . . . . . .     1 1 1 1 1 1 1 1     . . . . . . . .
 * . . . . . . . .     1 1 1 1 1 1 1 1     . . . . . . . .
 * @param number
 * @return
 */
U64 &bb::lsbReset(U64 &number) {
    number &= (number - 1);
    return number;
}

U64 bb::_xor(U64 &a, U64 &b) {
    return a ^ b;
}

U64 bb::_or(U64 &a, U64 &b) {
    return a | b;
}

U64 bb::_and(U64 &a, U64 &b) {
    return a & b;
}

U64 bb::_not(U64 &a) {
    return ~a;
}

U64 bb::shiftWest(U64 b) {
    b = (b >> 1) & NOT_FILE_H;
    return b;
}

U64 bb::shiftEast(U64 b) {
    b = (b << 1) & NOT_FILE_A;
    return b;
}

U64 bb::shiftSouth(U64 b) {
    b = b >> 8;
    return b;
}

U64 bb::shiftNorth(U64 b) {
    b = b << 8;
    return b;
}

U64 bb::shiftNorthEast(U64 b) {
    b = (b << 9) & NOT_FILE_A;
    return b;
}

U64 bb::shiftSouthEast(U64 b) {
    b = (b >> 7) & NOT_FILE_A;
    return b;
}

U64 bb::shiftSouthWest(U64 b) {
    b = (b >> 9) & NOT_FILE_H;
    return b;
}

U64 bb::shiftNorthWest(U64 b) {
    b = (b << 7) & NOT_FILE_H;
    return b;
}


/**
 * returns the index of the LSB
 * @param bb
 * @return
 */
Square bb::bitscanForward(U64 &bb) {
    
    assert(bb != 0);
    
    return __builtin_ctzll(bb);
}

/**
 * returns the index of the MSB
 * @param bb
 * @return
 */
Square bb::bitscanReverse(U64 &bb) {
    
    assert(bb != 0);
    
    return __builtin_clzll(bb) ^ 63;
}

/**
 * returns the amount of set bits in the given bitboard.
 * @param bb
 * @return
 */
int bb::bitCount(U64 &bb) {
    return __builtin_popcountll(bb);
//        int counter = 0;
//        while(bb != 0){
//            bb = lsbReset(bb);
//            counter ++;
//        }
//        return counter;
}

/**
 * The Chebyshev distance is the maximum of the absolute rank- and file-distance of both squares.
 * @param f1
 * @param r1
 * @param f2
 * @param r2
 * @return
 */
int bb::chebyshevDistance(File &f1, Rank &r1, File &f2, Rank &r2) {
    return max(abs(r2 - r1), abs(f2 - f1));
}

/**
 * The Chebyshev distance is the maximum of the absolute rank- and file-distance of both squares.
 * @param sq1
 * @param sq2
 * @return
 */
int bb::chebyshevDistance(Square &sq1, Square &sq2) {
    
    File fI1 = fileIndex(sq1);
    Rank rI1 = rankIndex(sq1);
    File fI2 = fileIndex(sq2);
    Rank rI2 = rankIndex(sq2);
    
    return chebyshevDistance(fI1, rI1, fI2, rI2);
}

/**
 * the orthogonal Manhattan-Distance is the sum of both absolute rank- and file-distance distances
 * @param f1
 * @param r1
 * @param f2
 * @param r2
 * @return
 */
int bb::manhattanDistance(File &f1, Rank &r1, File &f2, Rank &r2) {
    return max(abs(r2 - r1), abs(f2 - f1));
}

/**
 * the orthogonal Manhattan-Distance is the sum of both absolute rank- and file-distance distances
 * @param sq1
 * @param sq2
 * @return
 */
int bb::manhattanDistance(Square &sq1, Square &sq2) {
    File fI1 = fileIndex(sq1);
    Rank rI1 = rankIndex(sq1);
    File fI2 = fileIndex(sq2);
    Rank rI2 = rankIndex(sq2);
    
    return manhattanDistance(fI1, rI1, fI2, rI2);
}


template<class T>
T bb::abs(const T &a) {
    if (a < 0) return -a;
    return a;
}

template<class T>
T bb::max(const T &a, const T &b) {
    return (a < b) ? b : a;     // or: return comp(a,b)?b:a; for version (2)
}

