
#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>

class VentanaEstacion : public QWidget {
    Q_OBJECT
public:
    struct InfoPlatoVisual {
        QString nombrePlato;
        double prioridad;
        QString estado;
        long long id_pedido;
        long long id_instancia;
    };

    explicit VentanaEstacion(const QString& nombreEstacion, QWidget* parent = nullptr);

    void cargarPlatosIniciales(const std::vector<InfoPlatoVisual>& platos);
    void agregarNuevoPlato(const InfoPlatoVisual& plato);
    void actualizarEstadoPlato(long long idInstancia, const QString& nuevoEstado);
    void eliminarPlato(long long idInstancia);
    void eliminarPlatosDePedido(long long idPedido);

signals:
    void marcarListoSolicitado(long long idPedido, long long idInstancia);

private slots:
    void onFinalizarMayorPrioridad();

private:
    QTableWidget* tabla;
    QPushButton* btnFinalizar;
    QString m_estacion;
    std::vector<InfoPlatoVisual> platosOrdenados;
};
