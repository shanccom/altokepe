#include "network/RankingClient.h"
#include "ui/RankingWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  RankingClient cliente;

  RankingWindow ventana;
  ventana.show();

  QObject::connect(&cliente, &RankingClient::rankingActualizado, &ventana,
                   &RankingWindow::actualizarLista);

  // Conectar al servidor local (Puerto 5555)
  cliente.conectar("127.0.0.1", 5555);

  return app.exec();
}
