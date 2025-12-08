
#include "VentanaEstacion.h"
#include <QHeaderView>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <QPointer>
#include <algorithm>

constexpr int Role_IdPedido = Qt::UserRole + 1;
constexpr int Role_IdInstancia = Qt::UserRole + 2;

VentanaEstacion::VentanaEstacion(const QString& nombreEstacion, QWidget* parent)
    : QWidget(parent), m_estacion(nombreEstacion) {

    setWindowTitle("Estación: " + nombreEstacion);
    resize(1000, 700);

    tabla = new QTableWidget(0, 4, this);
    tabla->setHorizontalHeaderLabels({"Plato", "Prioridad", "Estado", "Observaciones"});
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->verticalHeader()->setVisible(false);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);

    btnFinalizar = new QPushButton("Finalizar plato de mayor prioridad", this);
    connect(btnFinalizar, &QPushButton::clicked, this, &VentanaEstacion::onFinalizarMayorPrioridad);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(tabla);
    layout->addWidget(btnFinalizar);
    setLayout(layout);
}

void VentanaEstacion::cargarPlatosIniciales(const std::vector<InfoPlatoVisual>& platos) {
    tabla->setRowCount(0);
    platosOrdenados.clear();

    platosOrdenados = platos;
    std::sort(platosOrdenados.begin(), platosOrdenados.end(),
              [](const InfoPlatoVisual& a, const InfoPlatoVisual& b) {
                  return a.prioridad > b.prioridad;
              });

    for (const auto& plato : platosOrdenados) {
        agregarNuevoPlato(plato);
    }
}

void VentanaEstacion::agregarNuevoPlato(const InfoPlatoVisual& plato) {
    for(int i=0; i < tabla->rowCount(); ++i) {
        if (tabla->item(i, 0)->data(Role_IdInstancia).toLongLong() == plato.id_instancia) return;
    }

    int row = tabla->rowCount();
    tabla->insertRow(row);

    auto* itemNombre = new QTableWidgetItem(plato.nombrePlato);
    itemNombre->setData(Role_IdPedido, QVariant::fromValue(plato.id_pedido));
    itemNombre->setData(Role_IdInstancia, QVariant::fromValue(plato.id_instancia));

    tabla->setItem(row, 0, itemNombre);
    tabla->setItem(row, 1, new QTableWidgetItem(QString::number(plato.prioridad)));
    tabla->setItem(row, 2, new QTableWidgetItem(plato.estado));
    tabla->setItem(row, 3, new QTableWidgetItem("Ninguna"));

    if (plato.estado == "PREPARANDO") {
         tabla->item(row, 2)->setBackground(QColor("#c3e6cb"));
    }
}

void VentanaEstacion::onFinalizarMayorPrioridad() {
    if (tabla->rowCount() == 0) return;

    int filaObjetivo = -1;
    for (int i = 0; i < tabla->rowCount(); ++i) {
        QString estado = tabla->item(i, 2)->text();
        if (estado.contains("PREPARANDO")) {
            filaObjetivo = i;
            break;
        }
    }

    if (filaObjetivo != -1) {
        auto* item = tabla->item(filaObjetivo, 0);
        long long idPedido = item->data(Role_IdPedido).toLongLong();
        long long idInstancia = item->data(Role_IdInstancia).toLongLong();

        qDebug() << "Solicitando finalizar plato en estado PREPARANDO. ID:" << idInstancia;
        QTimer::singleShot(0, this, [this, filaObjetivo]() {
            if (filaObjetivo < tabla->rowCount()) {
                auto* item = tabla->item(filaObjetivo, 0);
                if (item) {
                    qDebug() << "[DEBUG] Eliminando fila segura:" << filaObjetivo;
                    tabla->removeRow(filaObjetivo);
                }
            }
        });
        emit marcarListoSolicitado(idPedido, idInstancia);
    } else {
        qWarning() << "No se encontró ningún plato en estado PREPARANDO para finalizar.";
    }
}

void VentanaEstacion::actualizarEstadoPlato(long long idInstancia, const QString& nuevoEstado) {
    for (int fila = 0; fila < tabla->rowCount(); ++fila) {
        auto* item = tabla->item(fila, 0);
        if (item && item->data(Role_IdInstancia).toLongLong() == idInstancia) {
            tabla->item(fila, 2)->setText(nuevoEstado);

            qDebug() << "Se actualizó el estado del plato con ID:" << idInstancia;

            // Gestión de Colores según estado
            if (nuevoEstado == "PREPARANDO") {
                tabla->item(fila, 2)->setBackground(QColor("#c3e6cb")); // Verde suave
            } else if (nuevoEstado == "DEVUELTO") {
                tabla->item(fila, 2)->setBackground(QColor("#f5c6cb")); // Rojo suave
            } else {
                tabla->item(fila, 2)->setBackground(Qt::white);
            }
            break;
        }
    }
}

void VentanaEstacion::eliminarPlato(long long idInstancia) {
    for (int fila = 0; fila < tabla->rowCount(); ++fila) {
        auto* item = tabla->item(fila, 0);
        if (item && item->data(Role_IdInstancia).toLongLong() == idInstancia) {
            tabla->removeRow(fila);
            break;
        }
    }
}

void VentanaEstacion::eliminarPlatosDePedido(long long idPedido) {
    // Iteramos hacia atrás para evitar problemas de índice al borrar
    for (int fila = tabla->rowCount() - 1; fila >= 0; --fila) {
        auto* item = tabla->item(fila, 0);
        if (item && item->data(Role_IdPedido).toLongLong() == idPedido) {
            tabla->removeRow(fila);
        }
    }
}
