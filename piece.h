#ifndef PIECE_H
#define PIECE_H

enum class PieceType { King, Queen, Rook, Bishop, Knight, Pawn };
enum class Color { White, Black };

struct Piece {
    PieceType type;
    Color color;

    bool hasMoved = false;
};

#endif // PIECE_H
