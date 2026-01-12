FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang-10 \
    git \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Create user
RUN useradd -rm -d /home/ubuntu -s /bin/bash -u 1000 ubuntu
USER ubuntu
WORKDIR /home/ubuntu

# Build SGFuzz
RUN git clone https://github.com/bajinsheng/SGFuzz.git && \
    cd SGFuzz && \
    sed -i 's/clang++/clang++-10/g' build.sh && \
    ./build.sh

# Copy source files
COPY --chown=ubuntu:ubuntu src/ /home/ubuntu/chess/src/
COPY --chown=ubuntu:ubuntu lib/ /home/ubuntu/chess/lib/
COPY --chown=ubuntu:ubuntu include/ /home/ubuntu/chess/include/
COPY --chown=ubuntu:ubuntu libcgc.h /home/ubuntu/chess/
COPY --chown=ubuntu:ubuntu fuzz_harness.c /home/ubuntu/chess/

# Build the fuzzer
WORKDIR /home/ubuntu/chess

# Compile with -Wno flags to suppress library redeclaration warnings
RUN clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Iinclude -Ilib \
    src/service.c -o service.o && \
    clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Ilib lib/stdlib.c -o stdlib.o && \
    clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Ilib lib/printf.c -o printf.o && \
    clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. -Ilib lib/mymath.c -o mymath.o && \
    clang-10 -c -g -O2 -fsanitize=fuzzer-no-link,address \
    -Wno-incompatible-library-redeclaration \
    -I. fuzz_harness.c -o harness.o && \
    clang++-10 -g -O2 -fsanitize=address \
    service.o stdlib.o printf.o mymath.o harness.o \
    /home/ubuntu/SGFuzz/libsfuzzer.a \
    -o chess_fuzzer -lpthread -lm

# Create seed corpus
RUN mkdir -p seeds && \
    echo -n "1,1 1,2" > seeds/pawn_move && \
    echo -n "1,0 2,2" > seeds/knight_move && \
    echo -n "6,0 5,2" > seeds/knight_white && \
    echo -n "9" > seeds/display && \
    echo -n "666" > seeds/quit

# Run command
# ./chess_fuzzer seeds/ -max_total_time=60 -print_final_stats=1
