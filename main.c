#include "chib.c"

#include <stdio.h>
#include <stdlib.h>

const char START_FEN[] =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int main(void) {
    Board board = new_board();
    load_fen(&board, START_FEN);

    printf("Board:\n%s\n", stringify_board(board));
    printf("White pawn bitboard value: %llu\n", board.bitboards[0]);

    return 0;
}
