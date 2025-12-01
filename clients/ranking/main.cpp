#include "network/RankingClient.h"
#include "ui/RankingWindow.h"
#include "repository/JsonMenuRepository.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  RankingClient cliente;

  // Inyección de Dependencias: Usamos el repositorio JSON
  JsonMenuRepository repositorio(":/menu.json");
  RankingWindow ventana(&repositorio);
  
  ventana.show();

  QObject::connect(&cliente, &RankingClient::rankingActualizado, &ventana,
                   &RankingWindow::actualizarRanking);

  cliente.conectar("127.0.0.1", 5555);

  return app.exec();
}
