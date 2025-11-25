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

  cliente.iniciarSimulacion();

  return app.exec();
}
