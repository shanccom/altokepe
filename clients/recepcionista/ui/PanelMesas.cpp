#include "PanelMesas.h"
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>

PanelMesas::PanelMesas(QWidget *parent)
    : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    layout = new QGridLayout();
    layout->setHorizontalSpacing(30);
    layout->setVerticalSpacing(30);

    mainLayout->addLayout(layout);
    crearBotones();
}

void PanelMesas::crearBotones() {
    const int totalMesas = 9;
    const int columnas = 3;

    for (int i = 0; i < totalMesas; ++i) {
        int numeroMesa = i + 1;

        QString texto = QString("🍽️\nMesa %1").arg(numeroMesa);

        QPushButton *boton = new QPushButton(texto, this);
        boton->setObjectName("mesa");
        boton->setCheckable(true);
        boton->setMinimumSize(250, 200);
        boton->setMaximumSize(250, 200);

        QFont fuente;
        fuente.setPointSize(28);
        fuente.setBold(true);
        boton->setFont(fuente);

        boton->setStyleSheet("text-align: center;");

        botonesMesa.append(boton);
        estadosMesa.append(false);

        int fila = i / columnas;
        int columna = i % columnas;
        layout->addWidget(boton, fila, columna);

        connect(boton, &QPushButton::clicked, this, [this, i, numeroMesa]() {
            // Ignorar clicks si la mesa ya está deshabilitada
            if (!botonesMesa[i]->isEnabled()) return;
            estadosMesa[i] = !estadosMesa[i];
            actualizarEstilos();
            if (estadosMesa[i]) {
                emit mesaSeleccionada(numeroMesa);
            }
        });
    }

    actualizarEstilos();
}

void PanelMesas::actualizarEstilos() {
    for (int i = 0; i < botonesMesa.size(); ++i) {
        QPushButton *boton = botonesMesa[i];

        if (!boton->isEnabled()) {
            boton->setStyleSheet("background-color: #b0b0b0; color: #666; font-weight: bold; text-align: center; font-size: 35px;");
        } else if (estadosMesa[i]) {
            boton->setStyleSheet("background-color: #ff5c5c; color: white; font-weight: bold; text-align: center; font-size: 35px;");
        } else {
            boton->setStyleSheet("font-weight: bold; text-align: center; font-size: 35px;");
        }
    }
}

void PanelMesas::marcarMesaNoDisponible(int numeroMesa) {
    int index = numeroMesa - 1;
    if (index < 0 || index >= botonesMesa.size()) return;
    estadosMesa[index] = false; // quitar selección
    botonesMesa[index]->setEnabled(false);
    actualizarEstilos();
}
