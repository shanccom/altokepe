
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
    int row = tabla->rowCount();
    tabla->insertRow(row);

    auto* itemNombre = new QTableWidgetItem(plato.nombrePlato);
    itemNombre->setData(Role_IdPedido, QVariant::fromValue(plato.id_pedido));
    itemNombre->setData(Role_IdInstancia, QVariant::fromValue(plato.id_instancia));

    tabla->setItem(row, 0, itemNombre);
    tabla->setItem(row, 1, new QTableWidgetItem(QString::number(plato.prioridad)));
    tabla->setItem(row, 2, new QTableWidgetItem(plato.estado));
    tabla->setItem(row, 3, new QTableWidgetItem("Ninguna"));
}

void VentanaEstacion::onFinalizarMayorPrioridad() {
    if (tabla->rowCount() == 0) return;

    int maxPriorityRow = 0;
    double maxPriority = tabla->item(0, 1)->text().toDouble();

    for (int i = 1; i < tabla->rowCount(); ++i) {
        double priority = tabla->item(i, 1)->text().toDouble();
        if (priority > maxPriority) {
            maxPriority = priority;
            maxPriorityRow = i;
        }
    }

    auto* item = tabla->item(maxPriorityRow, 0);
    long long idPedido = item->data(Role_IdPedido).toLongLong();
    long long idInstancia = item->data(Role_IdInstancia).toLongLong();

    emit marcarListoSolicitado(idPedido, idInstancia);
}

void VentanaEstacion::actualizarEstadoPlato(long long idPedido, long long idInstancia, const QString& nuevoEstado) {
    int filaAEliminar = -1;

    for (int fila = 0; fila < tabla->rowCount(); ++fila) {
        auto* item = tabla->item(fila, 0);
        if (!item) continue;

        if (item->data(Role_IdPedido).toLongLong() == idPedido &&
            item->data(Role_IdInstancia).toLongLong() == idInstancia) {

            tabla->item(fila, 2)->setText(nuevoEstado);

            if (nuevoEstado.contains("FINALIZADO", Qt::CaseInsensitive)) {
                filaAEliminar = fila;
            }
            break;
        }
    }

    if (filaAEliminar >= 0) {
        QTimer::singleShot(0, this, [this, filaAEliminar]() {
            if (filaAEliminar < tabla->rowCount()) {
                auto* item = tabla->item(filaAEliminar, 0);
                if (item) {
                    qDebug() << "[DEBUG] Eliminando fila segura:" << filaAEliminar;
                    tabla->removeRow(filaAEliminar);
                }
            }
        });
    }
}