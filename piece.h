#ifndef PIECE_H
#define PIECE_H

enum class PieceType { King, Queen, Rook, Bishop, Knight, Pawn };
enum class Color { White, Black };

struct Piece {
    PieceType type;
    Color color;

    bool hasMoved = false;

    Piece(PieceType t, Color c, bool moved = false) : type(t), color(c), hasMoved(moved) {}
};

#endif // PIECE_H
