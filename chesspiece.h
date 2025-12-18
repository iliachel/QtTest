#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <QGraphicsTextItem>
#include "piece.h"

class ChessPiece : public QGraphicsTextItem {
public:
    ChessPiece(Piece* piece, int file, int rank);

    Piece* piece() const { return m_piece; }

    int file() const { return m_file; }
    int rank() const { return m_rank; }

    void setBoardPos(int file, int rank);
    void setSelected(bool selected);
    void updateAppearance();


private:
    Piece* m_piece;
    int m_file;
    int m_rank;
};


#endif // CHESSPIECE_H
