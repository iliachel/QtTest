#ifndef RULES_H
#define RULES_H

#include "board.h"
#include "move.h"
#include <cmath>

// Проверка, что на пути между from и to нет фигур (для ладьи, слона, ферзя)
inline bool pathIsClear(const Board& board, const Move& move) {
    int df = move.toFile - move.fromFile;
    int dr = move.toRank - move.fromRank;

    int stepX = (df == 0) ? 0 : (df / std::abs(df));
    int stepY = (dr == 0) ? 0 : (dr / std::abs(dr));

    int x = move.fromFile + stepX;
    int y = move.fromRank + stepY;

    while (x != move.toFile || y != move.toRank) {
        if (board.pieceAt(x, y))
            return false;
        x += stepX;
        y += stepY;
    }
    return true;
}

// Проверка, можно ли двигать фигуру из (fromFile, fromRank) в (toFile, toRank)
inline bool isValidMove(const Board& board, const Move& move) {
    if (move.pieceCaptured &&
        move.pieceCaptured->color == move.pieceMoved->color)
        return false;

    if (!move.pieceMoved)
        return false;

    int df = move.toFile - move.fromFile;
    int dr = move.toRank - move.fromRank;

    switch (move.pieceMoved->type) {
    case PieceType::Pawn:
        if (move.pieceMoved->type == PieceType::Pawn) {
            int dir = (move.pieceMoved->color == Color::White) ? 1 : -1;

            // ход вперед
            if (df == 0) {
                if (dr == dir && !board.pieceAt(move.toFile, move.toRank)) return true;
                if (dr == 2*dir && ((move.pieceMoved->color == Color::White && move.fromRank == 1) ||
                                      (move.pieceMoved->color == Color::Black && move.fromRank == 6)) &&
                    !board.pieceAt(move.toFile, move.toRank)) return true;
            }
            // взятие
            if (std::abs(df) == 1 && dr == dir && board.pieceAt(move.toFile, move.toRank)) return true;

            // en passant
            if (std::abs(df) == 1 && dr == dir && !board.pieceAt(move.toFile, move.toRank) && board.lastMove) {
                Move* lm = board.lastMove;
                if (lm->pieceMoved->type == PieceType::Pawn &&
                    std::abs(lm->toRank - lm->fromRank) == 2 &&
                    lm->toFile == move.toFile &&
                    lm->toRank == move.fromRank) {
                    return true;
                }
            }
            return false;
        }

        break;

    case PieceType::Rook:
        if ((df == 0 || dr == 0) && pathIsClear(board, move)) return true;
        break;

    case PieceType::Bishop:
        if (std::abs(df) == std::abs(dr) && pathIsClear(board, move)) return true;
        break;

    case PieceType::Queen:
        if ((std::abs(df) == std::abs(dr) || df == 0 || dr == 0) && pathIsClear(board, move)) return true;
        break;

    case PieceType::Knight:
        if ((std::abs(df) == 2 && std::abs(dr) == 1) || (std::abs(df) == 1 && std::abs(dr) == 2)) return true;
        break;

    case PieceType::King:
        if (move.pieceMoved->type == PieceType::King) {
            int df = move.toFile - move.fromFile;
            int dr = move.toRank - move.fromRank;

            // обычный король
            if (std::abs(df) <= 1 && std::abs(dr) <= 1) return true;

            // рокировка
            if (!move.pieceMoved->hasMoved && dr == 0 && std::abs(df) == 2) {
                int rookFile = (df > 0) ? 7 : 0;
                Piece* rook = const_cast<Piece*>(board.pieceAt(rookFile, move.fromRank));
                if (!rook || rook->type != PieceType::Rook || rook->hasMoved)
                    return false;

                int step = (df > 0) ? 1 : -1;
                for (int x = move.fromFile + step; x != rookFile; x += step)
                    if (board.pieceAt(x, move.fromRank)) return false;

                return true;
            }
        }

        if (std::abs(df) <= 1 && std::abs(dr) <= 1) return true;
        break;
    }

    return false;
}

inline Piece* detectCapturedPiece(const Board& board, const Move& move)
{
    // обычное взятие
    if (board.pieceAt(move.toFile, move.toRank))
        return const_cast<Piece*>(board.pieceAt(move.toFile, move.toRank));

    // en passant
    if (move.pieceMoved->type == PieceType::Pawn && board.lastMove) {
        int df = move.toFile - move.fromFile;
        int dr = move.toRank - move.fromRank;
        int dir = (move.pieceMoved->color == Color::White) ? 1 : -1;

        if (std::abs(df) == 1 && dr == dir) {
            Move* lm = board.lastMove;

            if (lm->pieceMoved->type == PieceType::Pawn &&
                std::abs(lm->toRank - lm->fromRank) == 2 &&
                lm->toFile == move.toFile &&
                lm->toRank == move.fromRank) {

                return const_cast<Piece*>(board.pieceAt(lm->toFile, lm->toRank));
            }
        }
    }

    return nullptr;
}


#endif // RULES_H
