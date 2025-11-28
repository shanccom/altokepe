#pragma once

#include <QWidget>

// PATRÓN FACADE: Forward declarations
class RecepcionistaFacade;
class PanelMesas;
class PanelPedido;

class VentanaRecepcionista : public QWidget {
    Q_OBJECT
public:
    explicit VentanaRecepcionista(QWidget *parent = nullptr);

private:
    // PATRÓN FACADE: La ventana principal gestiona el Facade compartido
    // que se inyectará en todos los componentes que lo necesiten
    RecepcionistaFacade *facade;
    
    PanelMesas *panelMesas;
    PanelPedido *panelPedido;

    void configurarUI();

private slots:
    void manejarSeleccionMesa(int numeroMesa);
};
