#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

int turno = 0; 
bool sucursal1_activa = true;
void solicitar_bodega(int id_sucursal);

int main() {
    printf("Ejercicio 1\n");
    printf("===============================================\n\n");

    printf("--- ESCENARIO 1: Operación Normal ---\n");
    solicitar_bodega(1);
    solicitar_bodega(2);

    printf("--- ESCENARIO 2: Sucursal 1 entra en mantenimiento ---\n");
    sucursal1_activa = false; 
    printf("NOTA: La Sucursal 1 no operará hoy, por lo tanto no cambiará el turno.\n");

    solicitar_bodega(2); 

    return 0;
}

void solicitar_bodega(int id_sucursal) {
    int otro = (id_sucursal == 1) ? 2 : 1;
    int valor_turno = (id_sucursal == 1) ? 0 : 1;
    int siguiente_turno = (id_sucursal == 1) ? 1 : 0;

    printf("--- Intento de acceso: Sucursal %d ---\n", id_sucursal);
    
    while (turno != valor_turno) {
        printf("[BLOQUEADA] Sucursal %d esperando su turno. Turno actual de: Sucursal %d\n", id_sucursal, otro);
        sleep(2);
    }

    printf("[ACCESO] Sucursal %d procesando pedido en bodega...\n", id_sucursal);
    sleep(1);
    printf("[LISTO] Sucursal %d finalizó su solicitud.\n", id_sucursal);

    turno = siguiente_turno;
    printf("[SISTEMA] Prioridad transferida a Sucursal %d\n\n", otro);
}