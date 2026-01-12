/*
 * SGFuzz Harness for CROMU_00005 Chess Game
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <setjmp.h>

/* Global variables for CGC compatibility layer */
const unsigned char *__fuzz_data = NULL;
size_t __fuzz_size = 0;
size_t __fuzz_pos = 0;
jmp_buf __fuzz_jmp_buf;
int __fuzz_should_exit = 0;

/* External declarations from service.c */
extern char (*ptr_array)[8][8];
extern char current_team;

/* Renamed main function */
int chess_main(void);

/* Board array - needs to persist across calls for state reset */
static char fuzz_board[8][8];

/*
 * Reset game state to initial conditions
 */
static void reset_game_state(void) {
    current_team = 1;  /* WHITE_TEAM */
    ptr_array = &fuzz_board;

    /* Clear board */
    memset(fuzz_board, '.', sizeof(fuzz_board));

    /* White pieces - row 0 */
    fuzz_board[0][0] = 'd';  /* ROOK_WHITE */
    fuzz_board[1][0] = 'b';  /* KNIGHT_WHITE */
    fuzz_board[2][0] = 'c';  /* BISHOP_WHITE */
    fuzz_board[3][0] = 'f';  /* QUEEN_WHITE */
    fuzz_board[4][0] = 'e';  /* KING_WHITE */
    fuzz_board[5][0] = 'c';  /* BISHOP_WHITE */
    fuzz_board[6][0] = 'b';  /* KNIGHT_WHITE */
    fuzz_board[7][0] = 'd';  /* ROOK_WHITE */

    /* White pawns - row 1 */
    for (int i = 0; i < 8; i++) {
        fuzz_board[i][1] = 'a';  /* PAWN_WHITE */
    }

    /* Black pawns - row 6 */
    for (int i = 0; i < 8; i++) {
        fuzz_board[i][6] = 'g';  /* PAWN_BLACK */
    }

    /* Black pieces - row 7 */
    fuzz_board[0][7] = 'j';  /* ROOK_BLACK */
    fuzz_board[1][7] = 'h';  /* KNIGHT_BLACK */
    fuzz_board[2][7] = 'i';  /* BISHOP_BLACK */
    fuzz_board[3][7] = 'l';  /* QUEEN_BLACK */
    fuzz_board[4][7] = 'k';  /* KING_BLACK */
    fuzz_board[5][7] = 'i';  /* BISHOP_BLACK */
    fuzz_board[6][7] = 'h';  /* KNIGHT_BLACK */
    fuzz_board[7][7] = 'j';  /* ROOK_BLACK */
}

/*
 * LibFuzzer entry point
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size == 0) {
        return 0;
    }

    /* Set up fuzzer input buffer */
    __fuzz_data = data;
    __fuzz_size = size;
    __fuzz_pos = 0;
    __fuzz_should_exit = 0;

    /* Reset game state */
    reset_game_state();

    /* Run the chess game with setjmp for _terminate handling */
    if (setjmp(__fuzz_jmp_buf) == 0) {
        chess_main();
    }

    return 0;
}
