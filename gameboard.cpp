#include "gameboard.h"
#include <QtGui>
#include <cmath>

namespace tetris {


GameBoard::GameBoard(QWidget *parent) : QFrame(parent) {

  setFrameStyle(QFrame::Panel | QFrame::Sunken);
  setFocusPolicy(Qt::StrongFocus);

 
  isStarted = false;
  isPaused = false;

  
  clearBoard();

  nextBlock.setRandomBlock();
}


void GameBoard::clearBoard() {
  for (int i(0); i < BoardWidth * BoardHeigth; i++) {
    board[i] = NoBlock;
  }
}

void GameBoard::setNextBlockLabel(QLabel *label) {
  nextBlockLabel = label;
}


QSize GameBoard::sizeHint() const {
  return QSize(BoardWidth * 15 + frameWidth() * 2, BoardHeigth * 15 + frameWidth() * 2);
}


QSize GameBoard::minimumSizeHint() const {
  return QSize(BoardWidth * 5 + frameWidth() * 2, BoardHeigth * 5 + frameWidth() * 2);
}

void GameBoard::start() {
  if (isPaused)
    return;

  isStarted = true;
  isWaitingAfterLine = false;
  isGameOver = false;
  blocksDropped = 0;
  setLinesRemoved(0);  
  setPlayerScore(0);   
  setCurrentLevel(0);  
  fallDownRate = 1000;
  threshold = 15; 
  clearBoard();  
  newBlock();
  timer.start(getTimeout(), this);
 
}

void GameBoard::pause() {
  if (!isStarted)
      return;

  isPaused = !isPaused;
  if (isPaused) {
    timer.stop();
  } else {
    timer.start(getTimeout(), this);
  }

  update();
}

int GameBoard::getTimeout() {
  return fallDownRate / pow(1.4142, currentLevel);
}

void GameBoard::newBlock() {
  currentBlock = nextBlock;
  nextBlock.setRandomBlock();
  showNextBlock();

  currentX = BoardWidth / 2 + 1;
  currentY = BoardHeigth - 1 + currentBlock.minY();

  if (!tryMove(currentBlock, currentX, currentY)) {
    currentBlock.setBlockType(NoBlock);
    timer.stop();
    isStarted = false;
    isGameOver = true;
  }
}

void GameBoard::showNextBlock() {
  if (!nextBlockLabel)
      return;

  int dx = nextBlock.maxX() - nextBlock.minX() + 1;
  int dy = nextBlock.maxY() - nextBlock.minY() + 1;

  QPixmap pixmap(dx * sqWidth(), dy * sqHeight());
  QPainter painter(&pixmap);
  painter.fillRect(pixmap.rect(), nextBlockLabel->palette().background());

  for (int i(0); i < 4; ++i) {
    int x = nextBlock.x(i) - nextBlock.minX();
    int y = nextBlock.y(i) - nextBlock.minY();
    drawSquare(painter, x * sqWidth(), y * sqHeight(), nextBlock.shape());
	}
  nextBlockLabel->setPixmap(pixmap);
}

void GameBoard::paintEvent(QPaintEvent *event) {
  QFrame::paintEvent(event);

  QPainter painter(this);
 
  QRect rect = contentsRect();
 
 	if (isGameOver) {
    painter.drawText(rect, Qt::AlignCenter,"Game Over");
    return;
  }
  
  if (isPaused) {
    painter.drawText(rect, Qt::AlignCenter,"Paused");
    return;
  }

  int boardTop = rect.bottom() - BoardHeigth * sqHeight();

  for (int i = 0; i < BoardHeigth; ++i) {
    for (int j = 0; j < BoardWidth; j++) {
      BlockType block = shapeAt(j, BoardHeigth - i - 1);
      if (block != NoBlock) {
      
        drawSquare(painter, rect.left() + j * sqWidth(), boardTop + i * sqHeight(), block);
      }
    }
  }

  if (currentBlock.shape() != NoBlock) {
    for (int i = 0; i < 4; ++i) {
      int x = currentX + currentBlock.x(i);
      int y = currentY - currentBlock.y(i);
      drawSquare(painter, 
      					 rect.left() + x * sqWidth(), 
                 boardTop + (BoardHeigth - y - 1) * sqHeight(), 
                 currentBlock.shape());
    }
  }
}

void GameBoard::keyPressEvent(QKeyEvent *event) {
  if (!isStarted || isPaused || currentBlock.shape() == NoBlock) {
    QFrame::keyPressEvent(event);
    return;
  }

  switch(event->key()) {
  case Qt::Key_Left:
    tryMove(currentBlock, currentX - 1, currentY);
  	break;
  case Qt::Key_Right:
    tryMove(currentBlock, currentX + 1, currentY);
     break;
  case Qt::Key_Down:
    tryMove(currentBlock.rotatedRight(), currentX, currentY);
    break;
  case Qt::Key_Up:
    tryMove(currentBlock.rotatedLeft(), currentX, currentY);
    break;
  case Qt::Key_Space:
    dropDown();
    break;
  case Qt::Key_D:
    oneLineDown();
    break;
  default:
    QFrame::keyPressEvent(event);
    break;
  }
}

void GameBoard::timerEvent(QTimerEvent *event) {

  if (event->timerId() == timer.timerId()) {
    if (isWaitingAfterLine) {
        isWaitingAfterLine = false;
        newBlock();
        timer.start(getTimeout(), this);
    } else {
        oneLineDown();
    }
  } else {
    QFrame::timerEvent(event);
  }
}

void GameBoard::oneLineDown() {
  if (!tryMove(currentBlock, currentX, currentY - 1))
    pieceDropped();
}

void GameBoard::dropDown() {
  int newY = currentY;

  while(newY > 0) {
    if (!tryMove(currentBlock, currentX, currentY - 1))
      break;
    --newY;
  }
  pieceDropped();
}

void GameBoard::pieceDropped() {
  for (int i(0); i < 4; ++i) {
    int x = currentX + currentBlock.x(i);
    int y = currentY - currentBlock.y(i);
    shapeAt(x, y) = currentBlock.shape();
  }
  removeFullLines();
  if (!isWaitingAfterLine)
    newBlock();
}

void GameBoard::removeFullLines() {
  int numFullLines = 0;

  for (int i(BoardHeigth - 1); i >= 0; --i) {
    bool lineIsFull = true;
    for (int j(0); j < BoardWidth; ++j) {
      if (shapeAt(j, i) == NoBlock) {
        lineIsFull = false;
        break;
      }
    }
    if (lineIsFull) {
      ++numFullLines;

      for (int k(i); k < BoardHeigth - 1; ++k) {
        for (int j(0); j < BoardWidth; ++j) {
          shapeAt(j, k) = shapeAt(j, k + 1);
        }
      }
      for (int j(0); j < BoardWidth; ++j) {  }
    }
  }
  if (numFullLines > 0) {
    setLinesRemoved(linesRemoved + numFullLines);
    setPlayerScore(playerScore + pow(2, numFullLines) - 1);
    timer.start(500, this);
    isWaitingAfterLine = true;
    currentBlock.setBlockType(NoBlock);
    update();
  }
}

void GameBoard::drawSquare(QPainter &painter, int x, int y, BlockType type) {
 
  static const QRgb colorSet[8] = {
  	0x000000,  0xCC6666,  0x66CC66,  0x6666CC,
    0xCCCC66,  0xCC66CC,  0x66CCCC,  0xDAAA00
  };

  QColor color = colorSet[int(type)];
 
  painter.fillRect(x + 1, y + 1, sqWidth() - 2, sqHeight() - 2, color);
  painter.setPen(color.light());
 
  painter.drawLine(x, y + sqHeight() - 1, x, y);
  painter.drawLine(x, y, x + sqWidth() - 1, y);
  painter.setPen(color.dark());
  painter.drawLine(x + 1, y + sqHeight() - 1, x + sqWidth() - 1, y + sqHeight() - 1);
  painter.drawLine(x + sqWidth() - 1, y + sqHeight() - 1, x + sqWidth() - 1, y + 1);
}

bool GameBoard::tryMove(const Block &newBlock, int newX, int newY) {
  for (int i(0); i < 4; ++i) {
    int x = newX + newBlock.x(i);
    int y = newY - newBlock.y(i);

    if (x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeigth) {
      return false;
    }
  
    if (shapeAt(x, y) != NoBlock) {
      return false;
    }
  }

  currentBlock = newBlock;
  currentX = newX;
  currentY = newY;
  update();
  
  return true;
}

int GameBoard::sqWidth() {
  return contentsRect().width() / BoardWidth;
}

int GameBoard::sqHeight() {
  return contentsRect().height() / BoardHeigth;
}

void GameBoard::setPlayerScore(const int score) {
  playerScore = score;
  if (playerScore > 0 && playerScore / threshold > currentLevel) {
    setCurrentLevel(++currentLevel);
    timer.start(getTimeout(), this);
  }
  emit playerScoreChanged(score);
}

void GameBoard::setCurrentLevel(const int level) {
  currentLevel = level;
  emit currentLevelChanged(level);
}

void GameBoard::setLinesRemoved(const int numLines) {
  linesRemoved = numLines;
  emit linesRemovedChanged(numLines);
}

}
