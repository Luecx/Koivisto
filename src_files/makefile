CC      = g++
SRC     = *.cpp syzygy/*.c nn/*.cpp nn/data/*.cpp
LIBS    = -lpthread -lm
EXE     = Koivisto

WFLAGS = -std=c++14
CFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -march=native
RFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -static
TFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -march=native -fopenmp -DTUNE
PFLAGS = -O0 $(WFLAGS) -DNDEBUG -p -pg
DFLAGS = -O0 $(WFLAGS)

POPCNTFLAGS = -DUSE_POPCNT -msse3 -mpopcnt
PEXTFLAGS   = $(POPCNTFLAGS) -DUSE_PEXT -mbmi2

ARMV8FLAGS  = -O3 $(WFLAGS) -DNDEBUG -flto -march=armv8-a -m64
ARMV7FLAGS  = -O3 $(WFLAGS) -DNDEBUG -flto -march=armv7-a -m32
ARMV7FLAGS += -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -Wl,--fix-cortex-a8

#popcnt:
#	$(CC) $(CFLAGS) $(SRC) $(LIBS) $(POPCNTFLAGS) -o $(EXE)
#
#nopopcnt:
#	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(EXE)
#
#pext:
#	$(CC) $(CFLAGS) $(SRC) $(LIBS) $(PEXTFLAGS) -o $(EXE)

release:
	$(CC) $(CFLAGS) $(SRC) $(LIBS) $(POPCNTFLAGS) -o $(EXE)

#tune:
#	$(CC) $(TFLAGS) $(SRC) $(LIBS) $(POPCNT) -o $(EXE)
#
#profile:
#	$(CC) $(PFLAGS) $(SRC) $(LIBS) $(POPCNT) -o $(EXE)
#
#debug:
#	$(CC) $(DFLAGS) $(SRC) $(LIBS) $(POPCNT) -o $(EXE)
#
#armv8:
#	$(CC) $(ARMV8FLAGS) $(SRC) -lm -o $(EXE)
#
#armv7:
#	$(CC) $(ARMV7FLAGS) $(SRC) -lm -o $(EXE)