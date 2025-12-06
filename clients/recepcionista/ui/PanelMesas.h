#pragma once

#include <QWidget>
#include <QVector>

class QPushButton;
class QGridLayout;

class PanelMesas : public QWidget {
    Q_OBJECT
public:
    explicit PanelMesas(QWidget *parent = nullptr);

    // Marca una mesa como no disponible (ya tiene pedido enviado)
    void marcarMesaNoDisponible(int numeroMesa);

signals:
    void mesaSeleccionada(int numeroMesa);

private:
    QVector<QPushButton*> botonesMesa;
    QVector<bool> estadosMesa; // true si seleccionada actualmente
    QGridLayout *layout;

    void crearBotones();
    void actualizarEstilos();
};
