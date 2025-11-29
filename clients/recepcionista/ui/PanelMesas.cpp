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

        // Texto con salto de línea: emoji arriba, texto abajo
        QString texto = QString("🍽️\nMesa %1").arg(numeroMesa);

        QPushButton *boton = new QPushButton(texto, this);
        boton->setObjectName("mesa");
        boton->setCheckable(true);
        boton->setMinimumSize(250, 200);
        boton->setMaximumSize(250, 200);

        // Fuente para emoji + texto, más grande pero uniforme
        QFont fuente;
        fuente.setPointSize(28); // tamaño de letra aumentado
        fuente.setBold(true);
        boton->setFont(fuente);

        // Centrar contenido
        boton->setStyleSheet("text-align: center;");

        botonesMesa.append(boton);
        estadosMesa.append(false);

        int fila = i / columnas;
        int columna = i % columnas;
        layout->addWidget(boton, fila, columna);

        connect(boton, &QPushButton::clicked, this, [this, i, numeroMesa]() {
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

        if (estadosMesa[i]) {
            boton->setStyleSheet("background-color: #ff5c5c; color: white; font-weight: bold; text-align: center; font-size: 35px;");
        } else {
            boton->setStyleSheet("font-weight: bold; text-align: center; font-size: 35px;");
        }
    }
}

