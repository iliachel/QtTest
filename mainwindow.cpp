#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsView>
#include <QPen>
#include <QBrush>
#include <QMouseEvent>
#include "rules.h"
#include "move.h"
#include <vector>

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
            QColor color = ((x + y) % 2 == 0) ? lightSquare : darkSquare;
            scene->addRect(x * TileSize, y * TileSize, TileSize, TileSize, QPen(Qt::NoPen), QBrush(color));
        }
    }
}

void MainWindow::drawPieces()
{
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            const Piece* piece = board.pieceAt(x, y);
            if (!piece) continue;

            auto* item = new ChessPiece(piece, x, y);
            scene->addItem(item);
            chessItems[x][y] = item;
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

    ChessPiece* clickedPiece = chessItems[file][rank];

    if (selectedPiece) {
        // A piece is already selected, try to move it
        std::optional<Piece> capturedPiece;
        if (board.pieceAt(file, rank)) {
            capturedPiece = *board.pieceAt(file, rank);
        }
        Move move(selectedPiece->file(), selectedPiece->rank(), file, rank, *selectedPiece->piece(), capturedPiece);

        if (isValidMove(board, move)) {
            board.applyMove(move);
            selectedPiece->setSelected(false);
            clearHighlights();
            selectedPiece = nullptr;
            updateBoard();
        } else {
            // Invalid move, deselect or select another piece
            selectedPiece->setSelected(false);
            if (clickedPiece && clickedPiece->piece()->color == board.sideToMove) {
                selectedPiece = clickedPiece;
                selectedPiece->setSelected(true);
                highlightMoves(selectedPiece);
            } else {
                selectedPiece = nullptr;
                clearHighlights();
            }
        }
    } else {
        // No piece is selected, select the clicked piece if it's of the correct color
        if (clickedPiece && clickedPiece->piece()->color == board.sideToMove) {
            selectedPiece = clickedPiece;
            selectedPiece->setSelected(true);
            highlightMoves(selectedPiece);
        }
    }
}

void MainWindow::updateBoard()
{
    // Clear existing pieces from the scene
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (chessItems[x][y]) {
                scene->removeItem(chessItems[x][y]);
                delete chessItems[x][y];
                chessItems[x][y] = nullptr;
            }
        }
    }

    // Redraw pieces from the board state
    drawPieces();
}

void MainWindow::highlightMoves(ChessPiece* piece)
{
    if (!piece || piece->piece()->color != board.sideToMove)
        return;

    clearHighlights();

    auto validMoves = generatePseudoLegalMoves(board, piece->piece()->color);

    for (const auto& move : validMoves) {
        if (move.fromFile == piece->file() && move.fromRank == piece->rank()) {
            auto* rect = scene->addRect(move.toFile * TileSize, move.toRank * TileSize, TileSize, TileSize,
                                        QPen(Qt::NoPen), QBrush(QColor(0, 255, 0, 80)));
            moveHighlights.push_back(rect);
        }
    }
}

void MainWindow::clearHighlights()
{
    for (auto* item : moveHighlights) {
        scene->removeItem(item);
        delete item;
    }
    moveHighlights.clear();
}

