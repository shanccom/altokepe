#include "network/RankingClient.h"
#include "ui/RankingWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  RankingClient cliente;
  RankingWindow ventana;
  
  ventana.show();

  // Conectar la señal del cliente con el slot de la ventana
  QObject::connect(&cliente, &RankingClient::datosActualizados, &ventana,
                   &RankingWindow::actualizarDatos);

  cliente.conectar("127.0.0.1", 5555);

  return app.exec();
}
