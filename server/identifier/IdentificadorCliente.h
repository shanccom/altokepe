#ifndef IDENTIFICADORCLIENTE_H
#define IDENTIFICADORCLIENTE_H

#include <QJsonObject>
#include <QString>
#include "common/models/Estados.h"

class IdentificadorCliente {
  public:
  virtual ~IdentificadorCliente() = default;
  virtual bool identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) = 0;
  virtual bool validarDatos(const QJsonObject& data) = 0;
};

class IdentificadorRecepcionista : public IdentificadorCliente {
  public:
    bool identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) override;
    bool validarDatos(const QJsonObject& data) override;
};

class IdentificadorManagerChef : public IdentificadorCliente {
  public:
    bool identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) override;
    bool validarDatos(const QJsonObject& data) override;
};

class IdentificadorEstacionCocina : public IdentificadorCliente {
  public:
    bool identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) override;
    bool validarDatos(const QJsonObject& data) override;
};
  
class IdentificadorRanking : public IdentificadorCliente {
  public:
    bool identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) override;
    bool validarDatos(const QJsonObject& data) override;
};

class IdentificadorClienteFactory {
  public:
    static IdentificadorCliente* crearIdentificador(const QString& rol);
};

#endif