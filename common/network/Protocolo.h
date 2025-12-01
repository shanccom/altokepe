#ifndef PROTOCOLO_H
#define PROTOCOLO_H

namespace Protocolo {
  // Claves Generales
  constexpr const char* COMANDO = "comando";
  constexpr const char* EVENTO = "evento";
  constexpr const char* DATA = "data";

  // Comandos Cliente a Servidor
  constexpr const char* IDENTIFICARSE = "IDENTIFICARSE";
  constexpr const char* NUEVO_PEDIDO = "NUEVO_PEDIDO";
  constexpr const char* PEDIDO_REGISTRADO = "PEDIDO_REGISTRADO";
  constexpr const char* PREPARAR_PEDIDO = "PREPARAR_PEDIDO";
  constexpr const char* CANCELAR_PEDIDO = "CANCELAR_PEDIDO";
  constexpr const char* MARCAR_PLATO_TERMINADO = "MARCAR_PLATO_TERMINADO";
  constexpr const char* CONFIRMAR_ENTREGA = "CONFIRMAR_ENTREGA";
  constexpr const char* DEVOLVER_PLATO = "DEVOLVER_PLATO";
  constexpr const char* SOLICITAR_ESTADO = "SOLICITAR_ESTADO";
  constexpr const char* SIMULAR_VENTA = "SIMULAR_VENTA";

  // Eventos específicos
  constexpr const char* NUEVO_PLATO_EN_COLA = "NUEVO_PLATO_EN_COLA";
  constexpr const char* PLATO_ESTADO_CAMBIADO = "PLATO_ESTADO_CAMBIADO";
  constexpr const char* ACTUALIZACION_RANKING = "ACTUALIZACION_RANKING";
  constexpr const char* PLATO_EN_PREPARACION = "PLATO_EN_PREPARACION";
  constexpr const char* PLATO_TERMINADO = "PLATO_TERMINADO";
  constexpr const char* PLATO_DEVUELTO = "PLATO_DEVUELTO";
  constexpr const char* PEDIDO_CANCELADO = "PEDIDO_CANCELADO";
  constexpr const char* PEDIDO_ENTREGADO = "PEDIDO_ENTREGADO";
  constexpr const char* ACTUALIZACION_ESTADO_GENERAL = "ACTUALIZACION_ESTADO_GENERAL";
  constexpr const char* ACTUALIZACION_ESTADO_ESTACION = "ACTUALIZACION_ESTADO_ESTACION";
  constexpr const char* ACTUALIZACION_MENU = "ACTUALIZACION_MENU";
  
  // Respuestas Servidor a Cliente
  constexpr const char* OK = "OK";
  constexpr const char* ERROR = "ERROR";
  constexpr const char* MENSAJE_ERROR = "mensaje_error";
}

#endif
