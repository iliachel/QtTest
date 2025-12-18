#ifndef BOARD_H
#define BOARD_H

#include <memory>
#include "piece.h"
#include "move.h"

class Board {
public:
    Board();
    Board(const Board& other);

    const Piece* pieceAt(int file, int rank) const;
    Piece* pieceAtMutable(int file, int rank);

    void applyMove(const Move& move);
    void undoMove(const Move& move);

    Color sideToMove = Color::White;
    std::unique_ptr<Move> lastMove;

private:
    std::unique_ptr<Piece> squares[8][8];
};

#endif // BOARD_H
