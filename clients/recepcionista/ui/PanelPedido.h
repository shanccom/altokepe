#pragma once

#include <QWidget>
#include <QJsonArray>
#include <QMap>
#include <QHeaderView>

class RecepcionistaFacade;

class QLabel;
class QLineEdit;
class QListWidget;
class QTableWidget;
class QPushButton;

class PanelPedido : public QWidget {
  Q_OBJECT
public:
  explicit PanelPedido(RecepcionistaFacade *facade, QWidget *parent = nullptr);
  void setNumeroMesa(int numero);

signals:
  // Se emite cuando el pedido de una mesa se ha enviado correctamente
  void pedidoEnviadoMesa(int numeroMesa);

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

  RecepcionistaFacade *facade;

  void configurarUI();
  void agregarPlatoAlPedido(int idPlato, const QString &nombre);
  void eliminarPlato(int fila);
  void actualizarTotal();

private slots:
  void enviarPedido();
  void actualizarMenu(const QJsonArray &menu);
};
