#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QPen>
#include <QBrush>
#include <QMouseEvent>
#include "rules.h"
#include "move.h"

constexpr int TileSize = 60;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    scene(new QGraphicsScene(this))
{
    ui->setupUi(this);

    auto *view = new QGraphicsView(scene, this);
    setCentralWidget(view);

    scene->setSceneRect(0, 0, 8 * TileSize, 8 * TileSize);

    drawBoard();
    drawPieces();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawBoard()
{
    QColor lightSquare(240, 217, 181);
    QColor darkSquare(181, 136, 99);
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            QColor color = ((x + y) % 2 == 0)
            ? lightSquare
            : darkSquare;

            scene->addRect(
                x * TileSize,
                y * TileSize,
                TileSize,
                TileSize,
                QPen(Qt::NoPen),
                QBrush(color)
                );
        }
    }
}

void MainWindow::drawPieces()
{
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Piece* piece = board.pieceAtMutable(x, y); // <- используем mutable
            if (!piece) continue;

            auto* item = new ChessPiece(piece, x, y);
            scene->addItem(item);
            chessItems[x][y] = item; // сохраняем объект
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    auto views = scene->views();
    if (views.isEmpty())
        return;

    QPointF pos = views.first()->mapToScene(event->pos());
    int file = int(pos.x()) / TileSize;
    int rank = int(pos.y()) / TileSize;

    if (file < 0 || file > 7 || rank < 0 || rank > 7)
        return;

    // Находим кликнутую фигуру (ChessPiece)
    ChessPiece* clickedPiece = nullptr;
    for (QGraphicsItem* item : scene->items(pos)) {
        if (auto* cp = dynamic_cast<ChessPiece*>(item)) {
            clickedPiece = cp;
            break;
        }
    }

    // Нельзя выбрать чужую фигуру
    if (clickedPiece && clickedPiece->piece()->color != board.sideToMove)
        return;

    // Если ничего не выбрано, выбираем фигуру своей стороны
    if (!selectedPiece) {
        if (clickedPiece) {
            selectedPiece = clickedPiece;
            selectedPiece->setSelected(true);
            highlightMoves(selectedPiece);
        }
        return;
    }

    // Если кликнули на фигуру своей стороны, меняем выбор
    if (clickedPiece && clickedPiece->piece()->color == board.sideToMove) {
        selectedPiece->setSelected(false);
        selectedPiece = clickedPiece;
        selectedPiece->setSelected(true);
        highlightMoves(selectedPiece);
        return;
    }

    // Делаем ход на пустую клетку или на фигуру противника
    Piece* moving = selectedPiece->piece();
    Piece* capturedPiece = nullptr;
    ChessPiece* targetItem = chessItems[file][rank];

    if (targetItem)
        capturedPiece = board.pieceAtMutable(file, rank); // Берем Piece из доски

    Move move(selectedPiece->file(), selectedPiece->rank(),
              file, rank,
              moving, capturedPiece);

    // Проверяем валидность хода
    if (!isValidMove(board, move))
        return;

    clearHighlights();

    // Применяем ход к логике доски (удаление взятой фигуры и обновление доски)
    board.applyMove(move);

    // Удаляем визуально взятую фигуру, если была
    if (targetItem) {
        scene->removeItem(targetItem);
        delete targetItem;
        chessItems[file][rank] = nullptr;
    }

    // Обновляем матрицу и позицию перемещаемой фигуры
    chessItems[selectedPiece->file()][selectedPiece->rank()] = nullptr;
    selectedPiece->setBoardPos(file, rank);
    chessItems[file][rank] = selectedPiece;

    // Обновляем графическое отображение (например, пешка -> ферзь)
    selectedPiece->updateAppearance();

    // Снимаем выделение
    selectedPiece->setSelected(false);
    selectedPiece = nullptr;
}





void MainWindow::highlightMoves(ChessPiece* piece)
{
    if (piece->piece()->color != board.sideToMove)
        return;

    clearHighlights();

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            Move move(
                piece->file(), piece->rank(),
                x, y,
                piece->piece(),
                nullptr
                );

            move.pieceCaptured = detectCapturedPiece(board, move);

            if (isValidMove(board, move)) {
                auto* rect = scene->addRect(
                    x * TileSize,
                    y * TileSize,
                    TileSize,
                    TileSize,
                    QPen(Qt::NoPen),
                    QBrush(QColor(0, 255, 0, 80))
                    );

                moveHighlights.push_back(rect);
            }
        }
    }
}



void MainWindow::clearHighlights(){
    for (auto* item : moveHighlights) {
        scene->removeItem(item);
        delete item;
    }
    moveHighlights.clear();
}

