#include <QtGui>
#include <stdlib.h>
#include "gamewindow.h"

using namespace tetris;

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    GameWindow gamewindow;
    gamewindow.show();
    return app.exec();
}
