// VentanaEstacionesUnificadas.h
#pragma once
#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QMap>
#include "../network/ClienteEstacionApp.h"

class VentanaEstacionesUnificadas : public QWidget {
    Q_OBJECT
public:
    explicit VentanaEstacionesUnificadas(QWidget* parent = nullptr);
    ~VentanaEstacionesUnificadas();

private:
    QTabWidget* tabs;
    QPushButton* btnSeparar;
    QMap<QString, ClienteEstacionApp*> estaciones;

    void crearEstaciones();
    void separarEstaciones();
};