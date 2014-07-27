#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QBasicTimer>
#include <QLabel>
#include <QFrame>
#include <QPointer>

#include "block.h"

namespace tetris {

class GameBoard : public QFrame {
  Q_OBJECT

public:
  GameBoard(QWidget *parent = 0);

  void setNextBlockLabel(QLabel *label);
  QSize sizeHint() const;
  QSize minimumSizeHint() const;

public slots:
  void start();
  void pause();

signals:
  void linesRemovedChanged(int numLines);
  void playerScoreChanged(int score);
  void currentLevelChanged(int level);

protected:
  void paintEvent(QPaintEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void timerEvent(QTimerEvent *event);

private:
  enum {
    BoardWidth = 10,
    BoardHeigth = 22
  };
  BlockType &shapeAt(int x, int y) { return board[(y * BoardWidth) + x]; }
  bool isStarted;
  bool isPaused;
  bool isWaitingAfterLine;
  bool isGameOver;

  int linesRemoved;
  int blocksDropped;
  int playerScore;
  int currentLevel;
  int currentX;
  int currentY;
  int threshold;
  int fallDownRate;

  QPointer<QLabel> nextBlockLabel;
  QBasicTimer timer;
  Block currentBlock;
  Block nextBlock;
  BlockType board[BoardWidth * BoardHeigth];


  void setPlayerScore(const int score);
  void setCurrentLevel(const int level);
  void setLinesRemoved(const int numLines);
  void newBlock();
  void showNextBlock();
  void clearBoard();
  void drawSquare(QPainter &painter, int x, int y, BlockType type);
  void pieceDropped();
  void dropDown();
  void oneLineDown();
  void removeFullLines();

  bool tryMove(const Block &newBlock, int newX, int newY);

  int sqWidth();
  int sqHeight();
  int getTimeout();
};

}

#endif // GAMEBOARD_H
