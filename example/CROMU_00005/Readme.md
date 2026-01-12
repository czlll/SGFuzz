## SGFuzz Example: CROMU_00005 Chess Game

This directory contains the SGFuzz-adapted version of the CROMU_00005 CGC (Cyber Grand Challenge) binary. The original challenge is a chess game implementation with an out-of-bounds write vulnerability.

For differences from the original and key modifications, see [DIFFERENCES.md](DIFFERENCES.md).

### Build and Run with Docker

#### 1. Build the Docker image
```shell
cd example/CROMU_00005
docker build -t cromu_00005_sgfuzz .
```

#### 2. Run the fuzzer
```shell
# Quick test (30 seconds)
docker run --rm cromu_00005_sgfuzz ./chess_fuzzer seeds/ -max_total_time=30 -print_final_stats=1

# Extended fuzzing (10 minutes)
docker run --rm cromu_00005_sgfuzz ./chess_fuzzer seeds/ -max_total_time=600 -print_final_stats=1
```

#### 3. Interactive mode (optional)
```shell
# Enter container for manual exploration
docker run -it --rm cromu_00005_sgfuzz /bin/bash

# Inside container
./chess_fuzzer seeds/ -max_total_time=60 -print_final_stats=1
```

### Build Manually (without Docker)

#### 1. Build and install SGFuzz
```shell
git clone https://github.com/bajinsheng/SGFuzz
cd SGFuzz
./build.sh
cp libsfuzzer.a /usr/lib/libsFuzzer.a
```

#### 2. Compile the target
```shell
cd example/CROMU_00005

# Compile source files
clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Iinclude -Ilib \
    src/service.c -o service.o

clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Ilib lib/stdlib.c -o stdlib.o

clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Ilib lib/printf.c -o printf.o

clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Ilib lib/mymath.c -o mymath.o

clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. fuzz_harness.c -o harness.o

# Link with SGFuzz
clang++-10 -g -O2 -fsanitize=address \
    service.o stdlib.o printf.o mymath.o harness.o \
    /path/to/SGFuzz/libsfuzzer.a \
    -o chess_fuzzer -lpthread -lm
```

#### 3. Create seed corpus
```shell
mkdir -p seeds
echo -n "1,1 1,2" > seeds/pawn_move
echo -n "1,0 2,2" > seeds/knight_move
echo -n "6,0 5,2" > seeds/knight_white
echo -n "9" > seeds/display
echo -n "666" > seeds/quit
```

#### 4. Run the fuzzer
```shell
./chess_fuzzer seeds/ -max_total_time=60 -print_final_stats=1
```

### About the Vulnerability

The original CROMU_00005 contains an out-of-bounds write vulnerability (CWE-787):
- In `service.c:performMove()`, the white knight move validator executes before bounds checking
- A white knight can move to position (7,8), which is outside the 8x8 board array
- This overwrites a function pointer on the stack
- Triggering display (option '9') then jumps to the corrupted pointer

SGFuzz state instrumentation helps discover this vulnerability by tracking game state transitions and guiding the fuzzer toward interesting protocol states.
