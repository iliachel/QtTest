#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include <optional>

// Represents a single move in the game
struct Move {
    int fromFile;
    int fromRank;
    int toFile;
    int toRank;

    Piece pieceMoved;
    std::optional<Piece> pieceCaptured;
    bool promotion = false;

    Move(int fFile, int fRank, int tFile, int tRank, const Piece& moved, std::optional<Piece> captured = std::nullopt, bool isPromotion = false)
        : fromFile(fFile), fromRank(fRank), toFile(tFile), toRank(tRank), pieceMoved(moved), pieceCaptured(captured), promotion(isPromotion)
    {}
};

#endif // MOVE_H
