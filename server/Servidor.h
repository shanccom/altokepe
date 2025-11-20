#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <QTcpServer>

class Servidor : public QTcpServer {
  Q_OBJECT
public:
  explicit Servidor(QObject* parent = nullptr);

protected:
  void incomingConnection(qintptr socketDescriptor) override;
};

#endif