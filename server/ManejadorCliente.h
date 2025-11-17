#ifndef MANEJADORCLIENTE_H
#define MANEJADORCLIENTE_H

#include <string>
#include <vector>

class ManejadorCliente {

public:
    explicit ManejadorCliente(int socketDescriptor);
    virtual ~ManejadorCliente();

    void procesar();
    void enviarMensaje(const std::string& mensaje);

private:
    void leerDatos();
    void procesarBuffer();
    void desconectado();

private:
    int m_socketDescriptor;
    std::string m_buffer;
};

#endif
