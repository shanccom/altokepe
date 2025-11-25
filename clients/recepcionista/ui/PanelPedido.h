#pragma once

#include <QWidget>
#include <QJsonArray>
#include <QMap>
#include "../network/ClienteRecepcionista.h"
#include <QHeaderView>

class QLabel;
class QLineEdit;
class QListWidget;
class QTableWidget;
class QPushButton;

class PanelPedido : public QWidget {
  Q_OBJECT
public:
  explicit PanelPedido(QWidget *parent = nullptr);
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

  ClienteRecepcionista cliente;

  void configurarUI();
  void agregarPlatoAlPedido(int idPlato, const QString &nombre);
  void eliminarPlato(int fila);
  void actualizarTotal();

private slots:
  void enviarPedido();
  void actualizarMenu(const QJsonArray &menu);
};
