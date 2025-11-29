#include <QApplication>
#include "ui/VentanaRecepcionista.h"
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Cargar archivo QSS
  QFile file(":/styles.qss");
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    app.setStyleSheet(styleSheet);
  }

  VentanaRecepcionista ventana;
  ventana.showMaximized();

  return app.exec();
}
