#ifndef RULES_H
#define RULES_H

#include "board.h"
#include "move.h"
#include <cmath>
#include <vector>

// Check if the path between 'from' and 'to' is clear (for rooks, bishops, queens)
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

// Check if a move is valid
inline bool isValidPawnMove(const Board& board, const Move& move) {
    int df = move.toFile - move.fromFile;
    int dr = move.toRank - move.fromRank;
    int dir = (move.pieceMoved->color == Color::White) ? 1 : -1;

    // Forward move
    if (df == 0) {
        // Single square forward
        if (dr == dir && !board.pieceAt(move.toFile, move.toRank))
            return true;
        // Double square forward
        if (dr == 2 * dir && !move.pieceMoved->hasMoved &&
            !board.pieceAt(move.fromFile, move.fromRank + dir) &&
            !board.pieceAt(move.toFile, move.toRank))
            return true;
    }
    // Capture
    if (std::abs(df) == 1 && dr == dir && board.pieceAt(move.toFile, move.toRank))
        return true;
    // En passant
    if (std::abs(df) == 1 && dr == dir && !board.pieceAt(move.toFile, move.toRank) && board.lastMove) {
        const Move* lm = board.lastMove.get();
        if (lm->pieceMoved->type == PieceType::Pawn &&
            std::abs(lm->toRank - lm->fromRank) == 2 &&
            lm->toFile == move.toFile &&
            lm->toRank == move.fromRank) {
            return true;
        }
    }
    return false;
}

inline bool isValidMove(const Board& board, const Move& move) {
    if (move.pieceCaptured && move.pieceCaptured->color == move.pieceMoved->color)
        return false;

    if (!move.pieceMoved)
        return false;

    int df = move.toFile - move.fromFile;
    int dr = move.toRank - move.fromRank;

    switch (move.pieceMoved->type) {
        case PieceType::Pawn:
            return isValidPawnMove(board, move);
        case PieceType::Rook:
            return (df == 0 || dr == 0) && pathIsClear(board, move);
        case PieceType::Bishop:
            return (std::abs(df) == std::abs(dr)) && pathIsClear(board, move);
        case PieceType::Queen:
            return (std::abs(df) == std::abs(dr) || df == 0 || dr == 0) && pathIsClear(board, move);
        case PieceType::Knight:
            return (std::abs(df) == 2 && std::abs(dr) == 1) || (std::abs(df) == 1 && std::abs(dr) == 2);
        case PieceType::King:
            return isValidKingMove(board, move);
    }
    return false;
}

inline bool isValidKingMove(const Board& board, const Move& move) {
    int df = move.toFile - move.fromFile;
    int dr = move.toRank - move.fromRank;

    // Normal move
    if (std::abs(df) <= 1 && std::abs(dr) <= 1)
        return true;

    // Castling
    if (!move.pieceMoved->hasMoved && dr == 0 && std::abs(df) == 2) {
        int rookFile = (df > 0) ? 7 : 0;
        const Piece* rook = board.pieceAt(rookFile, move.fromRank);
        if (!rook || rook->type != PieceType::Rook || rook->hasMoved)
            return false;

        int step = (df > 0) ? 1 : -1;
        for (int x = move.fromFile + step; x != rookFile; x += step)
            if (board.pieceAt(x, move.fromRank))
                return false;
        return true;
    }
    return false;
}

// Check if the king of a given color is in check
inline bool isKingInCheck(const Board& board, Color kingColor) {
    int kingFile = -1, kingRank = -1;
    // Find the king's position
    for (int r = 0; r < 8; ++r) {
        for (int f = 0; f < 8; ++f) {
            const Piece* piece = board.pieceAt(f, r);
            if (piece && piece->type == PieceType::King && piece->color == kingColor) {
                kingFile = f;
                kingRank = r;
                break;
            }
        }
        if (kingFile != -1) break;
    }

    if (kingFile == -1) return false; // Should not happen in a valid game

    // Check if any opponent's piece can attack the king
    Color opponentColor = (kingColor == Color::White) ? Color::Black : Color::White;
    auto opponentMoves = generatePseudoLegalMoves(board, opponentColor);
    for (const auto& move : opponentMoves) {
        if (move.toFile == kingFile && move.toRank == kingRank) {
            return true;
        }
    }

    return false;
}

// Generate all legal moves for a given color
inline std::vector<Move> generateLegalMoves(Board& board, Color color) {
    std::vector<Move> legalMoves;
    auto pseudoLegalMoves = generatePseudoLegalMoves(board, color);

    for (const auto& move : pseudoLegalMoves) {
        Board tempBoard = board;
        tempBoard.applyMove(move);
        if (!isKingInCheck(tempBoard, color)) {
            legalMoves.push_back(move);
        }
    }
    return legalMoves;
}

// Check if the current player is in checkmate
inline bool isCheckmate(Board& board, Color color) {
    if (!isKingInCheck(board, color)) {
        return false;
    }
    return generateLegalMoves(board, color).empty();
}

// Check if the current player is in stalemate
inline bool isStalemate(Board& board, Color color) {
    if (isKingInCheck(board, color)) {
        return false;
    }
    return generateLegalMoves(board, color).empty();
}

// Generate all pseudo-legal moves for a given color
inline std::vector<Move> generatePseudoLegalMoves(const Board& board, Color color) {
    std::vector<Move> moves;
    for (int r = 0; r < 8; ++r) {
        for (int f = 0; f < 8; ++f) {
            const Piece* piece = board.pieceAt(f, r);
            if (piece && piece->color == color) {
                for (int tr = 0; tr < 8; ++tr) {
                    for (int tf = 0; tf < 8; ++tf) {
                        const Piece* captured = board.pieceAt(tf, tr);
                        Move move(f, r, tf, tr, piece, captured);
                        if (isValidMove(board, move)) {
                            moves.push_back(move);
                        }
                    }
                }
            }
        }
    }
    return moves;
}
#endif // RULES_H
