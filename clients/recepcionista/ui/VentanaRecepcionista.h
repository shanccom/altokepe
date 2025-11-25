#pragma once

#include <QWidget>

class PanelMesas;
class PanelPedido;

class VentanaRecepcionista : public QWidget {
    Q_OBJECT
public:
    explicit VentanaRecepcionista(QWidget *parent = nullptr);

private:
    PanelMesas *panelMesas;
    PanelPedido *panelPedido;

    void configurarUI();

private slots:
    void manejarSeleccionMesa(int numeroMesa);
};
