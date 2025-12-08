#include "network/RankingClient.h"
#include "ui/RankingWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  RankingClient cliente;
  RankingWindow ventana;
  
  ventana.show();

  // Conectar la señal de datos del cliente con el slot de la ventana
  QObject::connect(&cliente, &RankingClient::datosActualizados, 
                   &ventana, &RankingWindow::actualizarDatos);

  // Conectar las señales de error del cliente con los slots de la ventana
  QObject::connect(&cliente, &RankingClient::errorConexion,
                   &ventana, &RankingWindow::mostrarErrorConexion);
  
  QObject::connect(&cliente, &RankingClient::errorDatos,
                   &ventana, &RankingWindow::mostrarErrorDatos);
  
  QObject::connect(&cliente, &RankingClient::errorServidor,
                   &ventana, &RankingWindow::mostrarErrorServidor);
  
  QObject::connect(&cliente, &RankingClient::desconectado,
                   &ventana, &RankingWindow::mostrarDesconexion);

  cliente.conectar("127.0.0.1", 5555);

  return app.exec();
}
