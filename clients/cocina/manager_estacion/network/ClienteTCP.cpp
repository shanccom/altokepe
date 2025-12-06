#include "ClienteTCP.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

ClienteTCP::ClienteTCP(QObject* parent) : QObject(parent), socket(new QTcpSocket(this)) {
    connect(socket, &QTcpSocket::connected, this, &ClienteTCP::onConectado);
    connect(socket, &QTcpSocket::disconnected, this, &ClienteTCP::onDesconectado);
    connect(socket, &QTcpSocket::readyRead, this, &ClienteTCP::leerDatos);
}

void ClienteTCP::conectar(const QString& host, quint16 puerto) {
    qDebug() << "[ClienteTCP] Conectando a" << host << ":" << puerto;
    socket->connectToHost(host, puerto);
}

void ClienteTCP::enviarMensaje(const QJsonObject& mensaje) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = QJsonDocument(mensaje).toJson(QJsonDocument::Compact) + "\n";
        socket->write(data);
    } else {
        qWarning() << "[ClienteTCP] No se puede enviar mensaje. Socket no conectado.";
    }
}

void ClienteTCP::onConectado() {
    qDebug() << "[ClienteTCP] Conectado al servidor.";
    emit conectado();
}

void ClienteTCP::onDesconectado() {
    qDebug() << "[ClienteTCP] Desconectado del servidor.";
    emit desconectado();
}

void ClienteTCP::leerDatos() {
    buffer.append(socket->readAll());

    while (buffer.contains('\n')) {
        int pos = buffer.indexOf('\n');
        QByteArray linea = buffer.left(pos);
        buffer.remove(0, pos + 1);

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(linea, &error);

        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            emit nuevoMensajeRecibido(doc.object());
        } else {
            qWarning() << "[ClienteTCP] JSON inválido recibido:" << QString(linea);
        }
    }
}