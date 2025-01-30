#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1, 
    A2, B2, C2, D2, E2, F2, G2, H2, 
    A3, B3, C3, D3, E3, F3, G3, H3, 
    A4, B4, C4, D4, E4, F4, G4, H4, 
    A5, B5, C5, D5, E5, F5, G5, H5, 
    A6, B6, C6, D6, E6, F6, G6, H6, 
    A7, B7, C7, D7, E7, F7, G7, H7, 
    A8, B8, C8, D8, E8, F8, G8, H8, 
};
// clang-format on

typedef uint8_t Square;

#define rank_of(square) ((square) >> 3)
#define file_of(square) ((square) & 7)
#define bitboard_of(square) ((uint64_t)1 << (square))

enum Piece {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
};

typedef uint8_t Piece;

enum Color { Black, White };

typedef uint8_t Color;

#define inverse_of(color) (color ^ 1)

const uint8_t FLAGS_BLACK_KINGSIDE = 1;
const uint8_t FLAGS_BLACK_QUEENSIDE = 2;
// const uint8_t FLAGS_WHITE_KINGSIDE = 4;
// const uint8_t FLAGS_WHITE_QUEENSIDE = 8;
const uint8_t FLAGS_ACTIVE_COLOR = 16;
const uint8_t FLAGS_EN_PASSANT = 32;

typedef struct __attribute__((packed)) Board {
    uint64_t bitboards[12];
    Square en_passant_target;
    uint8_t halfmoves;  // 75-move-rule
    uint16_t fullmoves; // Longest possible game is 8848.5 fullmoves
    uint8_t flags;      // --ECQKqk (2 most significant bits are unused)
} Board;

int can_castle_kingside(Board board, Color color) {
    return board.flags & (FLAGS_BLACK_KINGSIDE << (color * 2));
}

int can_castle_queenside(Board board, Color color) {
    return board.flags & (FLAGS_BLACK_QUEENSIDE << (color * 2));
}

Color active_color(Board board) {
    return (board.flags & FLAGS_ACTIVE_COLOR) >> 4;
}

int can_en_passant(Board board) {
    return board.flags & FLAGS_EN_PASSANT;
}

Board new_board(void) {
    Board board = {
        .bitboards = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    return board;
}

void load_fen(Board *board, const char *fen) {
    int length = strlen(fen);
    char current;
    int i;

    // Pieces
    for (i = 0; i < 12; i++) {
        board->bitboards[i] = 0;
    }
    int rank = 7;
    int file = 0;
    for (i = 0; i < length; i++) {
        current = fen[i];

        if (current == '/') {
            continue;
        };

        if (current == ' ') {
            break;
        }

        uint64_t *bitboard = 0;
        int jump_count = 1;
        switch (current) {
        case 'P':
            bitboard = &board->bitboards[0];
            break;
        case 'N':
            bitboard = &board->bitboards[1];
            break;
        case 'B':
            bitboard = &board->bitboards[2];
            break;
        case 'R':
            bitboard = &board->bitboards[3];
            break;
        case 'Q':
            bitboard = &board->bitboards[4];
            break;
        case 'K':
            bitboard = &board->bitboards[5];
            break;
        case 'p':
            bitboard = &board->bitboards[6];
            break;
        case 'n':
            bitboard = &board->bitboards[7];
            break;
        case 'b':
            bitboard = &board->bitboards[8];
            break;
        case 'r':
            bitboard = &board->bitboards[9];
            break;
        case 'q':
            bitboard = &board->bitboards[10];
            break;
        case 'k':
            bitboard = &board->bitboards[11];
            break;

        default:
            jump_count = current - '0';
            if (jump_count < 1 || jump_count > 8) {
                return;
            }
            break;
        }

        if (bitboard != 0) {
            *bitboard |= (uint64_t)1 << (8 * rank + file);
        }

        file += jump_count;

        if (file > 7) {
            rank--;
            file = 0;
        }
    }
}

char *stringify_board(Board board) {
    const char PIECE_CHARS[12] = {'P', 'N', 'B', 'R', 'Q', 'K',
                                  'p', 'n', 'b', 'r', 'q', 'k'};

    char *output = malloc(sizeof(char) * 128);

    int i, j, k;

    for (i = 1; i < 128; i += 2) {
        output[i] = ' ';
    }

    for (i = 15; i < 128; i += 16) {
        output[i] = '\n';
    }

    for (i = 7; i >= 0; i--) {
        for (j = 0; j < 8; j++) {
            int coord = i * 8 + j;
            int output_coord = ((7 - i) * 8 + j) * 2;

            uint64_t mask = bitboard_of(coord);
            int found_piece = 0;

            for (k = 0; k < 12; k++) {
                if (board.bitboards[k] & mask) {
                    output[output_coord] = PIECE_CHARS[k];
                    found_piece = 1;
                    break;
                }
            }

            if (found_piece == 0) {
                output[output_coord] = '-';
            }
        }
    }

    output[127] = '\0';
    return output;
}
