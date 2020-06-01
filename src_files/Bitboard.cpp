

#include "Bitboard.h"


using namespace bb;

U64 **bb::ROOK_ATTACKS = new U64*[64];
U64 **bb::BISHOP_ATTACKS = new U64*[64];

U64 **bb::all_hashes = {};

void bb::bb_init() {
    
    std::cout << "generating bitboards...";
    
    std::srand(seed);
    
    auto start = std::chrono::system_clock::now();
    
    
    generateZobristKeys();
    generateData();
    
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end-start;
    
    std::cout << "       done! [" << round(diff.count() * 1000) << " ms]" << std::endl;
}

void bb::bb_cleanUp() {
    for(int i = 0; i < 64; i++){
        delete ROOK_ATTACKS[i];
        ROOK_ATTACKS[i] = nullptr;
        delete BISHOP_ATTACKS[i];
        BISHOP_ATTACKS[i] = nullptr;
    }
    
    for(int i = 0; i <12; i++){
        delete all_hashes[i];
        all_hashes[i] = nullptr;
    }
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
        ROOK_ATTACKS[n] = new U64[ONE << (64-rookShifts[n])];
        BISHOP_ATTACKS[n] = new U64[ONE << (64-bishopShifts[n])];
        
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
 * A 1 or 0 will only be placed where there is a 1 in the mask.
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
        
        mask = lsbReset(mask);
        i++;
    }
    /**
     * assert that no other bits can be set other than the masked bits
     */
    assert((res & (~initMask)) == 0);
    
    return res;
}

void bb::generateZobristKeys() {
   
    all_hashes = new U64*[12];
    for (int i = 0; i < 6; i++) {
        all_hashes[i] = new U64[64];
        all_hashes[i+6] = new U64[64];
        for (int n = 0; n < 64; n++) {
            all_hashes[i][n] = randU64();
            all_hashes[i + 6][n] = randU64();
        }
    }
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






