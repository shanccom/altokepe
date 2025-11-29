#ifndef LOGICANEGOCIOHANDLERS_H
#define LOGICANEGOCIOHANDLERS_H

#include <memory>
#include <QJsonObject>
#include <QString>

class LogicaNegocio;
class ManejadorCliente;
struct PedidoMesa;
struct PlatoInstancia;
struct PlatoDefinicion;

struct OperacionContext {
  QJsonObject mensaje;
  QJsonObject data;
  LogicaNegocio* logica = nullptr;
  ManejadorCliente* remitente = nullptr;
  PedidoMesa* pedido = nullptr;
  PlatoInstancia* instancia = nullptr;
  PlatoDefinicion* definicion = nullptr;
};

class OperacionHandler {
public:
  using Ptr = std::shared_ptr<OperacionHandler>;
  virtual ~OperacionHandler() = default;

  OperacionHandler* setNext(const Ptr& next);
  bool process(OperacionContext& ctx);

protected:
  virtual bool handle(OperacionContext& ctx) = 0;

private:
  Ptr m_next;
};

class DataExtractionHandler : public OperacionHandler {
public:
  explicit DataExtractionHandler(const QString& dataKey = QStringLiteral("data"));

protected:
  bool handle(OperacionContext& ctx) override;

private:
  QString m_key;
};

class PedidoLookupHandler : public OperacionHandler {
protected:
  bool handle(OperacionContext& ctx) override;
};

class InstanciaLookupHandler : public OperacionHandler {
protected:
  bool handle(OperacionContext& ctx) override;
};

class EstacionAutorizadaHandler : public OperacionHandler {
protected:
  bool handle(OperacionContext& ctx) override;
};

#endif
