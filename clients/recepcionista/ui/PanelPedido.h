#pragma once

#include <QWidget>
#include <QJsonArray>
#include <QMap>
#include <QHeaderView>

// PATRÓN FACADE: Forward declaration en lugar de include
// Reduce acoplamiento y dependencias de compilación
class RecepcionistaFacade;

class QLabel;
class QLineEdit;
class QListWidget;
class QTableWidget;
class QPushButton;

class PanelPedido : public QWidget {
  Q_OBJECT
public:
  // PATRÓN FACADE + INYECCIÓN DE DEPENDENCIAS:
  // Recibe el Facade desde afuera en lugar de crearlo internamente
  explicit PanelPedido(RecepcionistaFacade *facade, QWidget *parent = nullptr);
  void setNumeroMesa(int numero);

private:
  int mesaActual = -1;
  QMap<int, QString> menuPlatos;
  QMap<int, double> preciosPlatos;

  QLabel *labelMesa;
  QLabel *labelPedido;
  QLabel *labelTotal;
  QLineEdit *inputNombre;
  QListWidget *listaPlatos;
  QTableWidget *tablaPedido;
  QPushButton *botonEnviar;

  // PATRÓN FACADE: Puntero a Facade compartido (no instancia propia)
  // Ventaja: Una sola conexión TCP para toda la aplicación
  RecepcionistaFacade *facade;

  void configurarUI();
  void agregarPlatoAlPedido(int idPlato, const QString &nombre);
  void eliminarPlato(int fila);
  void actualizarTotal();

private slots:
  void enviarPedido();
  void actualizarMenu(const QJsonArray &menu);
};
