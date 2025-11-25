#include "network/ClienteManagerApp.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);

  QFile styleFile(":/styles.qss"); // Busca en el directorio de trabajo
  if (styleFile.open(QFile::ReadOnly)) {
    a.setStyleSheet(QLatin1String(styleFile.readAll()));
    styleFile.close();
  } else {
    qWarning() << "No se pudo cargar la hoja de estilos 'styles.qss'."; 
  }

  ClienteManagerApp app;
  app.iniciar();

  return a.exec();
}
