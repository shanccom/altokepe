#include <QApplication>
#include "ui/VentanaRecepcionista.h"
#include <QFile>
#include <QTextStream>
#include "data/GestorPedidos.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QFile file(":/styles.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet);
    }

    gestorPedidos.cargarDesdeHistorial();

    VentanaRecepcionista ventana;
    ventana.showMaximized();

    return app.exec();
}
