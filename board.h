#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "move.h"

class Board {

public:
    Board();
    ~Board();

    const Piece* pieceAt(int file, int rank) const;

    void applyMove(const Move& move);

    Color sideToMove = Color::White;
    Move* lastMove = nullptr;

    Piece* pieceAtMutable(int file, int rank) {
        if (file < 0 || file > 7 || rank < 0 || rank > 7) return nullptr;
        return squares[file][rank];
    }

private:
    Piece* squares[8][8] = {};
};

#endif // BOARD_H
