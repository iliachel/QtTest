#include "board.h"
#include <cmath>
Board::Board() {
    // Белые
    squares[0][0] = new Piece{PieceType::Rook, Color::White, false};
    squares[1][0] = new Piece{PieceType::Knight, Color::White, false};
    squares[2][0] = new Piece{PieceType::Bishop, Color::White, false};
    squares[3][0] = new Piece{PieceType::Queen, Color::White, false};
    squares[4][0] = new Piece{PieceType::King, Color::White, false};
    squares[5][0] = new Piece{PieceType::Bishop, Color::White, false};
    squares[6][0] = new Piece{PieceType::Knight, Color::White, false};
    squares[7][0] = new Piece{PieceType::Rook, Color::White, false};
    for(int i = 0; i < 8; ++i)
        squares[i][1] = new Piece{PieceType::Pawn, Color::White, false};

    // Черные
    squares[0][7] = new Piece{PieceType::Rook, Color::Black, false};
    squares[1][7] = new Piece{PieceType::Knight, Color::Black, false};
    squares[2][7] = new Piece{PieceType::Bishop, Color::Black, false};
    squares[3][7] = new Piece{PieceType::Queen, Color::Black, false};
    squares[4][7] = new Piece{PieceType::King, Color::Black, false};
    squares[5][7] = new Piece{PieceType::Bishop, Color::Black, false};
    squares[6][7] = new Piece{PieceType::Knight, Color::Black, false};
    squares[7][7] = new Piece{PieceType::Rook, Color::Black, false};
    for(int i = 0; i < 8; ++i)
        squares[i][6] = new Piece{PieceType::Pawn, Color::Black, false};
}


const Piece* Board::pieceAt(int file, int rank) const
{
    if (file < 0 || file > 7 || rank < 0 || rank > 7)
        return nullptr;
    return squares[file][rank];
}

Board::~Board() {
    delete lastMove;
    for (int x = 0; x < 8; ++x){
        for (int y = 0; y < 8; ++y)
        {
            delete squares[x][y];
        }
    }

}

void Board::applyMove(const Move& move)
{
    // en passant
    if (move.pieceMoved->type == PieceType::Pawn &&
        move.fromFile != move.toFile && !move.pieceCaptured) {
        int pawnRank = (move.pieceMoved->color == Color::White)
        ? move.toRank - 1 : move.toRank + 1;
        delete squares[move.toFile][pawnRank];
        squares[move.toFile][pawnRank] = nullptr;
    }

    // удаление обычной фигуры
    if (move.pieceCaptured) {
        delete move.pieceCaptured;
    }

    // перемещение фигуры
    squares[move.toFile][move.toRank] = move.pieceMoved;
    squares[move.fromFile][move.fromRank] = nullptr;

    // рокировка
    if (move.pieceMoved->type == PieceType::King &&
        abs(move.toFile - move.fromFile) == 2) {
        int rookFrom = (move.toFile > move.fromFile) ? 7 : 0;
        int rookTo   = (move.toFile > move.fromFile) ? 5 : 3;
        Piece* rook = squares[rookFrom][move.fromRank];
        squares[rookTo][move.fromRank] = rook;
        squares[rookFrom][move.fromRank] = nullptr;
        if (rook) rook->hasMoved = true;
    }

    // превращение пешки
    if (move.pieceMoved->type == PieceType::Pawn) {
        if ((move.pieceMoved->color == Color::White && move.toRank == 7) ||
            (move.pieceMoved->color == Color::Black && move.toRank == 0)) {
            move.pieceMoved->type = PieceType::Queen;
        }
    }

    move.pieceMoved->hasMoved = true;

    delete lastMove;
    lastMove = new Move(move);

    sideToMove = (sideToMove == Color::White) ? Color::Black : Color::White;
}

