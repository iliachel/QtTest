#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

// Represents a single move in the game
struct Move {
    int fromFile;
    int fromRank;
    int toFile;
    int toRank;

    const Piece* pieceMoved = nullptr;
    const Piece* pieceCaptured = nullptr;
    bool promotion = false;

    Move(int fFile, int fRank, int tFile, int tRank, const Piece* moved, const Piece* captured = nullptr, bool isPromotion = false)
        : fromFile(fFile), fromRank(fRank), toFile(tFile), toRank(tRank), pieceMoved(moved), pieceCaptured(captured), promotion(isPromotion)
    {}
};

#endif // MOVE_H
