// VentanaEstacionesUnificadas.cpp
#include "VentanaEstacionesUnificadas.h"
#include <QVBoxLayout>

VentanaEstacionesUnificadas::VentanaEstacionesUnificadas(QWidget* parent)
    : QWidget(parent) {

    setWindowTitle("Vista Unificada de Estaciones");
    resize(1200, 800);

    tabs = new QTabWidget(this);
    btnSeparar = new QPushButton("Separar estaciones", this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(tabs);
    layout->addWidget(btnSeparar);

    setLayout(layout);
    crearEstaciones();

    connect(btnSeparar, &QPushButton::clicked, this, &VentanaEstacionesUnificadas::separarEstaciones);
}

VentanaEstacionesUnificadas::~VentanaEstacionesUnificadas() {
    for (auto estacion : estaciones) {
        delete estacion;
    }
}

void VentanaEstacionesUnificadas::crearEstaciones() {
    QStringList nombres = {"Snack", "Otro", "Principal", "Bebida"};

    for (const QString& nombre : nombres) {
        auto* cliente = new ClienteEstacionApp(nombre, this);
        cliente->iniciar();
        estaciones[nombre] = cliente;

        QWidget* contenedor = cliente->getVentana(); // asumimos getter en ClienteEstacionApp
        contenedor->setParent(tabs); // evitar que se cree como ventana flotante
        tabs->addTab(contenedor, nombre);
    }
}

void VentanaEstacionesUnificadas::separarEstaciones() {
    for (const QString& nombre : estaciones.keys()) {
        auto* cliente = estaciones[nombre];
        QWidget* ventana = cliente->getVentana();

        ventana->setParent(nullptr); // lo saca del QTabWidget
        ventana->show();             // ahora es una ventana independiente

        int idx = tabs->indexOf(ventana);
        if (idx >= 0) tabs->removeTab(idx);
    }

    btnSeparar->setEnabled(false); // prevenir repetir
}