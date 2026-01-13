#include "chesspiece.h"
#include <QFont>

static QString pieceToUnicode(const Piece& p)
{
    if (p.color == Color::White) {
        switch (p.type) {
        case PieceType::King:   return "♔";
        case PieceType::Queen: return "♕";
        case PieceType::Rook:  return "♖";
        case PieceType::Bishop:return "♗";
        case PieceType::Knight:return "♘";
        case PieceType::Pawn:  return "♙";
        }
    } else {
        switch (p.type) {
        case PieceType::King:   return "♚";
        case PieceType::Queen: return "♛";
        case PieceType::Rook:  return "♜";
        case PieceType::Bishop:return "♝";
        case PieceType::Knight:return "♞";
        case PieceType::Pawn:  return "♟";
        }
    }
    return "?";
}
ChessPiece::ChessPiece(const Piece* piece, int file, int rank)
    : m_piece(piece), m_file(file), m_rank(rank)
{
    setPlainText(pieceToUnicode(*piece));
    setFont(QFont("Arial", 32));
    setDefaultTextColor(Qt::black);
    setBoardPos(file, rank);
    setAcceptedMouseButtons(Qt::NoButton);
}


void ChessPiece::setBoardPos(int file, int rank)
{
    m_file = file;
    m_rank = rank;
    setPos(file * 60 + 15, rank * 60 + 10);
}

void ChessPiece::setSelected(bool selected)
{
    if (selected) {
        setDefaultTextColor(Qt::red);
    } else {
        setDefaultTextColor(Qt::black);
    }
}

void ChessPiece::updateAppearance() {
    setPlainText(pieceToUnicode(*m_piece));
}


