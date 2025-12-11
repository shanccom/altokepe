#include "IdentificadorCliente.h"
#include <QDebug>

bool IdentificadorRecepcionista::validarDatos(const QJsonObject& data) {
  int id = data["id"].toInt(-1);
  if (id == -1) {
    qWarning() << "Recepcionista no proporcionó un 'id' válido.";
    return false;
  }
  return true;
}

bool IdentificadorRecepcionista::identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) {
  if (!validarDatos(data)) {
    return false;
  }
  
  tipoActor = TipoActor::RECEPCIONISTA;
  idActor = data["id"].toInt();
  qDebug() << "Identificado como RECEPCIONISTA con ID:" << idActor;
  return true;
}

bool IdentificadorManagerChef::validarDatos(const QJsonObject& data) {
    return true;
}

bool IdentificadorManagerChef::identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) {
    if (!validarDatos(data)) {
      return false;
    }
    
    tipoActor = TipoActor::MANAGER_CHEF;
    qDebug() << "Identificado como MANAGER_CHEF.";
    return true;
}

bool IdentificadorEstacionCocina::validarDatos(const QJsonObject& data) {
  QString nombre = data["nombre_estacion"].toString();
  if (nombre.isEmpty()) {
    qWarning() << "EstacionCocina no proporcionó 'nombre_estacion'.";
    return false;
  }
  return true;
}

bool IdentificadorEstacionCocina::identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) {
  if (!validarDatos(data)) {
    return false;
  }
  
  tipoActor = TipoActor::ESTACION_COCINA;
  nombreEstacion = data["nombre_estacion"].toString();
  qDebug() << "Identificado como ESTACION_COCINA con Nombre:" << nombreEstacion;
  return true;
}

bool IdentificadorRanking::validarDatos(const QJsonObject& data) {
    // Ranking no requiere datos adicionales
    return true;
}

bool IdentificadorRanking::identificar(const QJsonObject& data, TipoActor& tipoActor, int& idActor, QString& nombreEstacion) {
  if (!validarDatos(data)) {
    return false;
  }
  
  tipoActor = TipoActor::RANKING;
  qDebug() << "Identificado como RANKING.";
  return true;
}

IdentificadorCliente* IdentificadorClienteFactory::crearIdentificador(const QString& rol) {
  if (rol == "Recepcionista") {
    return new IdentificadorRecepcionista();
  } else if (rol == "ManagerChef") {
    return new IdentificadorManagerChef();
  } else if (rol == "EstacionCocina") {
    return new IdentificadorEstacionCocina();
  } else if (rol == "Ranking") {
    return new IdentificadorRanking();
  }
  
  qWarning() << "Rol desconocido:" << rol;
  return nullptr;
}