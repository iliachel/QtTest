#include "board.h"
#include <cmath>

Board::Board() {
    // White pieces
    squares[0][0] = std::make_unique<Piece>(PieceType::Rook, Color::White, false);
    squares[1][0] = std::make_unique<Piece>(PieceType::Knight, Color::White, false);
    squares[2][0] = std::make_unique<Piece>(PieceType::Bishop, Color::White, false);
    squares[3][0] = std::make_unique<Piece>(PieceType::Queen, Color::White, false);
    squares[4][0] = std::make_unique<Piece>(PieceType::King, Color::White, false);
    squares[5][0] = std::make_unique<Piece>(PieceType::Bishop, Color::White, false);
    squares[6][0] = std::make_unique<Piece>(PieceType::Knight, Color::White, false);
    squares[7][0] = std::make_unique<Piece>(PieceType::Rook, Color::White, false);
    for (int i = 0; i < 8; ++i)
        squares[i][1] = std::make_unique<Piece>(PieceType::Pawn, Color::White, false);

    // Black pieces
    squares[0][7] = std::make_unique<Piece>(PieceType::Rook, Color::Black, false);
    squares[1][7] = std::make_unique<Piece>(PieceType::Knight, Color::Black, false);
    squares[2][7] = std::make_unique<Piece>(PieceType::Bishop, Color::Black, false);
    squares[3][7] = std::make_unique<Piece>(PieceType::Queen, Color::Black, false);
    squares[4][7] = std::make_unique<Piece>(PieceType::King, Color::Black, false);
    squares[5][7] = std::make_unique<Piece>(PieceType::Bishop, Color::Black, false);
    squares[6][7] = std::make_unique<Piece>(PieceType::Knight, Color::Black, false);
    squares[7][7] = std::make_unique<Piece>(PieceType::Rook, Color::Black, false);
    for (int i = 0; i < 8; ++i)
        squares[i][6] = std::make_unique<Piece>(PieceType::Pawn, Color::Black, false);
}

Board::Board(const Board& other) {
    sideToMove = other.sideToMove;
    if (other.lastMove) {
        lastMove = std::make_unique<Move>(*other.lastMove);
    }
    for (int r = 0; r < 8; ++r) {
        for (int f = 0; f < 8; ++f) {
            if (other.squares[f][r]) {
                squares[f][r] = std::make_unique<Piece>(*other.squares[f][r]);
            }
        }
    }
}

const Piece* Board::pieceAt(int file, int rank) const {
    if (file < 0 || file > 7 || rank < 0 || rank > 7)
        return nullptr;
    return squares[file][rank].get();
}

Piece* Board::pieceAtMutable(int file, int rank) {
    if (file < 0 || file > 7 || rank < 0 || rank > 7)
        return nullptr;
    return squares[file][rank].get();
}

void Board::applyMove(const Move& move) {
    // Handle en passant capture
    if (move.pieceMoved.type == PieceType::Pawn && move.fromFile != move.toFile && !move.pieceCaptured) {
        int capturedPawnRank = (move.pieceMoved.color == Color::White) ? move.toRank - 1 : move.toRank + 1;
        squares[move.toFile][capturedPawnRank].reset();
    }

    // Move the piece
    squares[move.toFile][move.toRank] = std::move(squares[move.fromFile][move.fromRank]);
    squares[move.fromFile][move.fromRank].reset();

    // Handle castling
    if (move.pieceMoved.type == PieceType::King && std::abs(move.toFile - move.fromFile) == 2) {
        int rookFromFile = (move.toFile > move.fromFile) ? 7 : 0;
        int rookToFile = (move.toFile > move.fromFile) ? 5 : 3;
        squares[rookToFile][move.fromRank] = std::move(squares[rookFromFile][move.fromRank]);
        squares[rookFromFile][move.fromRank].reset();
        if (auto* rook = pieceAtMutable(rookToFile, move.fromRank)) {
            rook->hasMoved = true;
        }
    }

    // Handle pawn promotion
    bool promotion = false;
    if (move.pieceMoved.type == PieceType::Pawn) {
        if ((move.pieceMoved.color == Color::White && move.toRank == 7) ||
            (move.pieceMoved.color == Color::Black && move.toRank == 0)) {
            squares[move.toFile][move.toRank] = std::make_unique<Piece>(PieceType::Queen, move.pieceMoved.color, true);
            promotion = true;
        }
    }

    if (auto* piece = pieceAtMutable(move.toFile, move.toRank)) {
        piece->hasMoved = true;
    }

    lastMove = std::make_unique<Move>(move.fromFile, move.fromRank, move.toFile, move.toRank, move.pieceMoved, move.pieceCaptured, promotion);
    sideToMove = (sideToMove == Color::White) ? Color::Black : Color::White;
}

void Board::undoMove(const Move& move) {
    sideToMove = (sideToMove == Color::White) ? Color::Black : Color::White;

    // Move the piece back
    squares[move.fromFile][move.fromRank] = std::move(squares[move.toFile][move.toRank]);
    squares[move.toFile][move.toRank].reset();

    // Restore captured piece
    if (move.pieceCaptured) {
        squares[move.toFile][move.toRank] = std::make_unique<Piece>(*move.pieceCaptured);
    }

    // Handle en passant undo
    if (move.pieceMoved.type == PieceType::Pawn && move.fromFile != move.toFile && !move.pieceCaptured) {
        int capturedPawnRank = (move.pieceMoved.color == Color::White) ? move.toRank - 1 : move.toRank + 1;
        Color capturedPawnColor = (move.pieceMoved.color == Color::White) ? Color::Black : Color::White;
        squares[move.toFile][capturedPawnRank] = std::make_unique<Piece>(PieceType::Pawn, capturedPawnColor, true);
    }

    // Handle castling undo
    if (move.pieceMoved.type == PieceType::King && std::abs(move.toFile - move.fromFile) == 2) {
        int rookFromFile = (move.toFile > move.fromFile) ? 7 : 0;
        int rookToFile = (move.toFile > move.fromFile) ? 5 : 3;
        squares[rookFromFile][move.fromRank] = std::move(squares[rookToFile][move.fromRank]);
        squares[rookToFile][move.fromRank].reset();
        if (auto* rook = pieceAtMutable(rookFromFile, move.fromRank)) {
            rook->hasMoved = false;
        }
    }

    // Handle pawn promotion undo
    if (move.promotion) {
        squares[move.fromFile][move.fromRank] = std::make_unique<Piece>(PieceType::Pawn, move.pieceMoved.color, move.pieceMoved.hasMoved);
    }

    if (auto* piece = pieceAtMutable(move.fromFile, move.fromRank)) {
        piece->hasMoved = move.pieceMoved.hasMoved;
    }
}

