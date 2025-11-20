#ifndef PROTOCOLO_H
#define PROTOCOLO_H

namespace Protocolo {
  // --- Claves Generales ---
  constexpr const char* COMANDO = "comando";
  constexpr const char* EVENTO = "evento";
  constexpr const char* DATA = "data";

    // --- Comandos Cliente -> Servidor ---
  constexpr const char* IDENTIFICARSE = "IDENTIFICARSE";
  constexpr const char* NUEVO_PEDIDO = "NUEVO_PEDIDO";
  constexpr const char* PREPARAR_PEDIDO = "PREPARAR_PEDIDO";
  constexpr const char* CANCELAR_PEDIDO = "CANCELAR_PEDIDO";
  constexpr const char* MARCAR_PLATO_TERMINADO = "MARCAR_PLATO_TERMINADO";
  constexpr const char* CONFIRMAR_ENTREGA = "CONFIRMAR_ENTREGA";
  constexpr const char* DEVOLVER_PLATO = "DEVOLVER_PLATO";

  // --- Eventos específicos de estación ---
  constexpr const char* NUEVO_PLATO_EN_COLA = "NUEVO_PLATO_EN_COLA";
  constexpr const char* PLATO_ESTADO_CAMBIADO = "PLATO_ESTADO_CAMBIADO";

  // --- Comando específico para estaciones ---
  constexpr const char* PREPARAR_PLATO = "PREPARAR_PLATO";


  // --- Eventos Servidor -> Cliente ---
  constexpr const char* ACTUALIZACION_ESTADO_GENERAL = "ACTUALIZACION_ESTADO_GENERAL";
  constexpr const char* ACTUALIZACION_ESTADO_ESTACION = "ACTUALIZACION_ESTADO_ESTACION";
  
  // --- Respuestas Servidor -> Cliente ---
  constexpr const char* OK = "OK";
  constexpr const char* ERROR = "ERROR";
  constexpr const char* MENSAJE_ERROR = "mensaje_error";
  
  }
  
#endif