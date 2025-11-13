#include "network/ClienteManagerApp.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);

  ClienteManagerApp app;
  app.iniciar();

  return a.exec();
}
