#ifndef _GAMEWINDOW_H
#define _GAMEWINDOW_H

#include <QtGui>

namespace tetris {

class GameBoard;

class GameWindow : public QWidget {
  Q_OBJECT

public:
  GameWindow();

private:
  QLabel *nextBlockLabel;
  QLabel *playerScoresLabel;
  QLabel *linesLabel;
  QLabel *currentLevelLabel;
  GameBoard *gameBoard;
  QPushButton *startBtn;
  QPushButton *pauseBtn;
  QPushButton *exitBtn;

  QLabel *createLabel(const QString &text);
};

}
#endif // _GAMEWINDOW_H
