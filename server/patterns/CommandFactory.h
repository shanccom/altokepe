#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include <memory>
#include <QJsonObject>
#include <QString>
#include "ICommand.h"

class ManejadorCliente;

class CommandFactory {
public:
  // Crea un comando basado en el campo "comando" del JSON.
  // Retorna nullptr si el comando no es reconocido.
  static std::unique_ptr<ICommand> create(const QJsonObject& mensaje, ManejadorCliente* remitente);
};

#endif
