void ManejadorCliente::procesarBuffer() {
  while (m_buffer.contains('\n')) {
    int newlinePos = m_buffer.indexOf('\n');
    QByteArray jsonData = m_buffer.left(newlinePos);
    m_buffer.remove(0, newlinePos + 1);
    if (jsonData.isEmpty()) continue;
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
      qWarning() << "Mensaje JSON inválido recibido en socket" << m_socketDescriptor;
      continue;
    }

    QJsonObject mensaje = doc.object();
    if (m_tipoActor == TipoActor::DESCONOCIDO) {
      if (mensaje[Protocolo::COMANDO].toString() == Protocolo::IDENTIFICARSE) {
        identificarCliente(mensaje);
      } else {
        qWarning() << "Cliente no identificado intentó enviar un comando. Se ignora.";
      }
    } else {
      LogicaNegocio::instance()->procesarMensaje(mensaje, this);
    }
  }
}

void ManejadorCliente::identificarCliente(const QJsonObject& data) {
  QString rolString = data["rol"].toString();

  if (rolString == "Recepcionista") {
    m_tipoActor = TipoActor::RECEPCIONISTA;
  } else if (rolString == "ManagerChef") {
    m_tipoActor = TipoActor::MANAGER_CHEF;
  } else if (rolString == "EstacionCocina") {
    m_tipoActor = TipoActor::ESTACION_COCINA;
  } else if (rolString == "Ranking") {
    m_tipoActor = TipoActor::RANKING;
  } else {
    m_tipoActor = TipoActor::DESCONOCIDO;
    qWarning() << "Rol desconocido recibido:" << rolString << ". Desconectando.";
    m_socket->disconnectFromHost();
    return;
  }

  switch (m_tipoActor) {
    case TipoActor::RECEPCIONISTA:
      m_idActor = data["id"].toInt(-1);
      if (m_idActor == -1) {
        qWarning() << "Recepcionista no proporcionó un 'id' válido. Desconectando.";
        m_socket->disconnectFromHost();
        return;
      }
      break;

    case TipoActor::ESTACION_COCINA:
      m_nombreEstacion = data["nombre_estacion"].toString();
      if (m_nombreEstacion.isEmpty()) {
        qWarning() << "EstacionCocina no proporcionó 'nombre_estacion'. Desconectando.";
        m_socket->disconnectFromHost();
        return;
      }
      break;

    default:
      break;
  }

  // Enviar estado inicial
  if (m_tipoActor != TipoActor::DESCONOCIDO)
    LogicaNegocio::instance()->enviarEstadoInicial(this);
}
