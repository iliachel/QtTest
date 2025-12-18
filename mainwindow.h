#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

#include "board.h"
#include "chesspiece.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGraphicsScene;
class QMouseEvent;
class QGraphicsRectItem;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    Board board;
    ChessPiece* selectedPiece = nullptr;

    ChessPiece* chessItems[8][8] = {};

    void drawBoard();
    void drawPieces();
    void updateBoard();

    void highlightMoves(ChessPiece* piece);
    void clearHighlights();

    std::vector<QGraphicsRectItem*> moveHighlights;
};

#endif // MAINWINDOW_H
