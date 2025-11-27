#include <QApplication>
#include "ui/VentanaEstacionesUnificadas.h"
#include <QFile>
#include <QIODevice>
#include <QString>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Estaciones de Cocina");

    VentanaEstacionesUnificadas principal;
    principal.show();
    QFile styleFile(":/styles.qss");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString style = styleFile.readAll();
        app.setStyleSheet(style);
    }

    return app.exec();
}