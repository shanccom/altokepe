#ifndef TARJETAPLATO_H
#define TARJETAPLATO_H

#include <QWidget>

class QLabel;

class TarjetaPlato : public QWidget {
  Q_OBJECT
public:
  explicit TarjetaPlato(const QString& nombrePlato, const QString& estadoInicial, QWidget* parent = nullptr);
  void actualizarEstado(const QString& nuevoEstado);

private:
  QLabel* m_nombreLabel;
  QLabel* m_estadoLabel;
};

#endif
