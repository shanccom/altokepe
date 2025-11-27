#!/usr/bin/env python3
import socket
import json
import time

# Conectar al servidor
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('localhost', 5555))

# Identificarse
identificacion = {"comando": "IDENTIFICARSE", "rol": "Ranking"}
sock.sendall((json.dumps(identificacion) + "\n").encode())

time.sleep(1)

# Simular ventas
for i in range(3):
    venta = {"comando": "SIMULAR_VENTA", "id_plato": 1}
    sock.sendall((json.dumps(venta) + "\n").encode())
    print(f"Venta {i+1} del plato ID 1 enviada")
    time.sleep(1)

for i in range(2):
    venta = {"comando": "SIMULAR_VENTA", "id_plato": 2}
    sock.sendall((json.dumps(venta) + "\n").encode())
    print(f"Venta {i+1} del plato ID 2 enviada")
    time.sleep(1)

print("Simulación completada. Presiona Ctrl+C para salir.")
time.sleep(10)
sock.close()
