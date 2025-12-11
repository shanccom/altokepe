#ifndef EXCEPCIONESCOMMON_H
#define EXCEPCIONESCOMMON_H

#include <exception>
#include <string>
#include <QString>

//Excepcion Base: Excepción base para todas las excepciones del módulo common
class ExcepcionCommon : public std::exception {
protected:
  std::string mensaje;
  
public:
  explicit ExcepcionCommon(const std::string& msg) : mensaje(msg) {}
  explicit ExcepcionCommon(const QString& msg) : mensaje(msg.toStdString()) {}
  
  const char* what() const noexcept override {
    return mensaje.c_str();
  }
  
  std::string obtenerMensaje() const {
    return mensaje;
  }
};

//Excepcion SerializacionException: Se lanza cuando ocurre un error durante la serialización o deserialización de objetos a/desde JSON
class ExcepcionSerializacion : public ExcepcionCommon {
public:
  explicit ExcepcionSerializacion(const std::string& msg) 
    : ExcepcionCommon("Error de serialización: " + msg) {}
  explicit ExcepcionSerializacion(const QString& msg) 
    : ExcepcionCommon("Error de serialización: " + msg.toStdString()) {}
};

//Excepcion CampoFaltanteException: Se lanza cuando falta un campo requerido en el objeto JSON durante la deserialización
class ExcepcionCampoFaltante : public ExcepcionSerializacion {
private:
  std::string nombreCampo;
  
public:
  explicit ExcepcionCampoFaltante(const std::string& campo) 
    : ExcepcionSerializacion("Campo requerido faltante: " + campo),
      nombreCampo(campo) {}
  
  explicit ExcepcionCampoFaltante(const QString& campo) 
    : ExcepcionSerializacion("Campo requerido faltante: " + campo.toStdString()),
      nombreCampo(campo.toStdString()) {}
  
  std::string obtenerNombreCampo() const {
    return nombreCampo;
  }
};

//Excepcion TipoIncorrectoException: Se lanza cuando un campo JSON tiene un tipo de dato incorrecto o incompatible con el esperado
class ExcepcionTipoIncorrecto : public ExcepcionSerializacion {
private:
  std::string nombreCampo;
  std::string tipoEsperado;
  std::string tipoRecibido;
  
public:
  ExcepcionTipoIncorrecto(const std::string& campo, const std::string& esperado, const std::string& recibido)
    : ExcepcionSerializacion("Tipo incorrecto en campo '" + campo + "'. Esperado: " + esperado + ", Recibido: " + recibido),
      nombreCampo(campo), tipoEsperado(esperado), tipoRecibido(recibido) {}
  
  ExcepcionTipoIncorrecto(const QString& campo, const QString& esperado, const QString& recibido)
    : ExcepcionSerializacion("Tipo incorrecto en campo '" + campo.toStdString() + "'. Esperado: " + esperado.toStdString() + ", Recibido: " + recibido.toStdString()),
      nombreCampo(campo.toStdString()), tipoEsperado(esperado.toStdString()), tipoRecibido(recibido.toStdString()) {}
  
  std::string obtenerNombreCampo() const { return nombreCampo; }
  std::string obtenerTipoEsperado() const { return tipoEsperado; }
  std::string obtenerTipoRecibido() const { return tipoRecibido; }
};

//Excepcion ModeloInvalidoException: Se lanza cuando los datos de un modelo no cumplen con las reglas de validación del negocio
class ExcepcionModeloInvalido : public ExcepcionCommon {
private:
  std::string nombreModelo;
  
public:
  ExcepcionModeloInvalido(const std::string& modelo, const std::string& razon)
    : ExcepcionCommon("Modelo inválido '" + modelo + "': " + razon),
      nombreModelo(modelo) {}
  
  ExcepcionModeloInvalido(const QString& modelo, const QString& razon)
    : ExcepcionCommon("Modelo inválido '" + modelo.toStdString() + "': " + razon.toStdString()),
      nombreModelo(modelo.toStdString()) {}
  
  std::string obtenerNombreModelo() const {
    return nombreModelo;
  }
};

//Excepcion ProtocoloException: Se lanza cuando hay un error en la comunicación del protocolo de red entre cliente y servidor
class ExcepcionProtocolo : public ExcepcionCommon {
private:
  std::string comando;
  
public:
  explicit ExcepcionProtocolo(const std::string& msg)
    : ExcepcionCommon("Error de protocolo: " + msg), comando("") {}
  
  ExcepcionProtocolo(const std::string& cmd, const std::string& msg)
    : ExcepcionCommon("Error de protocolo en comando '" + cmd + "': " + msg),
      comando(cmd) {}
  
  ExcepcionProtocolo(const QString& cmd, const QString& msg)
    : ExcepcionCommon("Error de protocolo en comando '" + cmd.toStdString() + "': " + msg.toStdString()),
      comando(cmd.toStdString()) {}
  
  std::string obtenerComando() const {
    return comando;
  }
};

//Excepcion AdaptadorException: Se lanza cuando ocurre un error en la capa de adaptador durante la conversión entre diferentes formatos de serialización
class ExcepcionAdaptador : public ExcepcionCommon {
private:
  std::string operacion;
  
public:
  ExcepcionAdaptador(const std::string& op, const std::string& msg)
    : ExcepcionCommon("Error en adaptador durante '" + op + "': " + msg),
      operacion(op) {}
  
  ExcepcionAdaptador(const QString& op, const QString& msg)
    : ExcepcionCommon("Error en adaptador durante '" + op.toStdString() + "': " + msg.toStdString()),
      operacion(op.toStdString()) {}
  
  std::string obtenerOperacion() const {
    return operacion;
  }
};

//Excepcion EstadoInvalidoException: Se lanza cuando se intenta realizar una transición de estado inválida en un plato o pedido
class ExcepcionEstadoInvalido : public ExcepcionCommon {
private:
  std::string estadoActual;
  std::string estadoDestino;
  
public:
  ExcepcionEstadoInvalido(const std::string& actual, const std::string& destino)
    : ExcepcionCommon("Transición de estado inválida: de '" + actual + "' a '" + destino + "'"),
      estadoActual(actual), estadoDestino(destino) {}
  
  ExcepcionEstadoInvalido(const QString& actual, const QString& destino)
    : ExcepcionCommon("Transición de estado inválida: de '" + actual.toStdString() + "' a '" + destino.toStdString() + "'"),
      estadoActual(actual.toStdString()), estadoDestino(destino.toStdString()) {}
  
  std::string obtenerEstadoActual() const { return estadoActual; }
  std::string obtenerEstadoDestino() const { return estadoDestino; }
};

#endif
