#include "network/RankingClient.h"
#include "ui/RankingWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // 1. Instanciamos la Lógica (Cliente)
  RankingClient cliente;

  // 2. Instanciamos la Vista (Ventana)
  RankingWindow ventana;
  ventana.show();

  // 3. CONECTAMOS (Patrón Observer)
  // Cuando 'cliente' diga "rankingActualizado", 'ventana' ejecutará
  // "actualizarLista"
  QObject::connect(&cliente, &RankingClient::rankingActualizado, &ventana,
                   &RankingWindow::actualizarLista);

  // 4. Iniciamos la simulación (ya que no hay servidor hoy)
  cliente.iniciarSimulacion();

  return app.exec();
}
