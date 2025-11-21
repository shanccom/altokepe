#pragma once

#include <QWidget>
#include <QVector>


class QPushButton;
class QGridLayout;

class PanelMesas : public QWidget {
    Q_OBJECT
public:
    explicit PanelMesas(QWidget *parent = nullptr);

signals:
    void mesaSeleccionada(int numeroMesa);

private:
    QVector<QPushButton*> botonesMesa;
    QVector<bool> estadosMesa;
    QGridLayout *layout;

    void crearBotones();
    void actualizarEstilos();


};
