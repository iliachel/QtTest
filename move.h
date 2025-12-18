#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

// Структура для представления одного хода
struct Move {
    int fromFile;
    int fromRank;
    int toFile;
    int toRank;

    Piece* pieceMoved = nullptr;
    Piece* pieceCaptured = nullptr;

    Move(int fFile, int fRank, int tFile, int tRank, Piece* moved, Piece* captured = nullptr)
        : fromFile(fFile), fromRank(fRank), toFile(tFile), toRank(tRank), pieceMoved(moved), pieceCaptured(captured)
    {}
};

#endif // MOVE_H
