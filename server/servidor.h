#ifndef SERVIDOR_STUB_H
#define SERVIDOR_STUB_H

#include <QTcpServer>
#include <QDebug>

class Servidor : public QTcpServer {
    Q_OBJECT
public:
    explicit Servidor(QObject* parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        qDebug() << "Nueva conexión entrante con descriptor:" << socketDescriptor;
    }
};

#endif
