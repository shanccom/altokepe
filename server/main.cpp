#include <QCoreApplication>
#include <QDebug>
#include "servidor.h"

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    Servidor servidor;

    quint16 port = 5555;
    if (!servidor.listen(QHostAddress::Any, port)) {
        qCritical() << "No se pudo iniciar el servidor en el puerto" << port;
        return 1;
    }

    qInfo() << "Servidor escuchando en el puerto" << port;
    return a.exec();
}
