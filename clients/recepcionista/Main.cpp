#include <QApplication>
#include "ui/VentanaRecepcionista.h"
#include <QFile>
#include <QTextStream>
#include "data/GestorPedidos.h"  // <-- AÑADIDO

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Cargar archivo QSS
    QFile file(":/styles.qss");  // <-- lo busca desde el resources.qrc
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet);
    }

    // Cargar historial antes de abrir la ventana
    gestorPedidos.cargarDesdeHistorial();  // <-- AÑADIDO

    VentanaRecepcionista ventana;
    ventana.showMaximized();

    return app.exec();
}
￼Enter
