#include "VentanaRecepcionista.h"
#include "PanelMesas.h"
#include "PanelPedido.h"
#include "../facade/RecepcionistaFacade.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

VentanaRecepcionista::VentanaRecepcionista(QWidget *parent)
    : QWidget(parent) {
    facade = new RecepcionistaFacade(this);
    facade->conectarAlServidor("127.0.0.1", 5555);
    configurarUI();

    connect(panelMesas, &PanelMesas::mesaSeleccionada,
            this, &VentanaRecepcionista::manejarSeleccionMesa);

    // Cuando se envía un pedido, deshabilitar la mesa correspondiente
    connect(panelPedido, &PanelPedido::pedidoEnviadoMesa, panelMesas, &PanelMesas::marcarMesaNoDisponible);
}

void VentanaRecepcionista::configurarUI() {
    panelMesas = new PanelMesas(this);
    panelPedido = new PanelPedido(facade, this);

    auto *titulo = new QLabel("Sistema Altoke Pe", this);
    titulo->setObjectName("tituloPrincipal");
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 48px; font-weight: bold;");

    auto *subtituloMesas = new QLabel("Seleccione la Mesa", this);
    subtituloMesas->setStyleSheet("font-size: 28px; font-weight: bold;");
    auto *layoutIzquierdo = new QVBoxLayout;
    layoutIzquierdo->setAlignment(Qt::AlignTop);
    layoutIzquierdo->setSpacing(10);
    layoutIzquierdo->setContentsMargins(10, 10, 10, 10);
    layoutIzquierdo->addWidget(subtituloMesas);
    layoutIzquierdo->addWidget(panelMesas);

    auto *subtituloPedido = new QLabel("Pedido", this);
    subtituloPedido->setStyleSheet("font-size: 28px; font-weight: bold;");
    auto *layoutDerecho = new QVBoxLayout;
    layoutDerecho->setAlignment(Qt::AlignTop);
    layoutDerecho->setSpacing(10);
    layoutDerecho->setContentsMargins(10, 10, 10, 10);
    layoutDerecho->addWidget(subtituloPedido);
    layoutDerecho->addWidget(panelPedido);

    auto *layoutCentral = new QHBoxLayout;
    layoutCentral->addLayout(layoutIzquierdo, 3);
    layoutCentral->addLayout(layoutDerecho, 2);

    auto *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addWidget(titulo);
    layoutPrincipal->addLayout(layoutCentral);

    setLayout(layoutPrincipal);
}

void VentanaRecepcionista::manejarSeleccionMesa(int numeroMesa) {
    panelPedido->setNumeroMesa(numeroMesa);
}
