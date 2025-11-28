#ifndef SERVERCOMMANDS_H
#define SERVERCOMMANDS_H

#include "ICommand.h"
#include <QJsonObject>

class ManejadorCliente;

class BaseServerCommand : public ICommand {
public:
  BaseServerCommand(ManejadorCliente* remitente, const QJsonObject& payload);
  virtual ~BaseServerCommand() = default;

protected:
  ManejadorCliente* m_remitente;
  QJsonObject m_payload;
};

// Comandos de Flujo de Pedidos
class CmdNuevoPedido : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

class CmdPrepararPedido : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

class CmdCancelarPedido : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

class CmdMarcarPlatoTerminado : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

class CmdConfirmarEntrega : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

class CmdDevolverPlato : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

// Comandos Administrativos y de Estado
class CmdSolicitarEstado : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

class CmdSimularVenta : public BaseServerCommand {
public:
  using BaseServerCommand::BaseServerCommand;
  void execute() override;
};

#endif
