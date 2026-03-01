#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

void limpiarBuffer();
void ejercicio1();
void *funcionHilo(void *args);
void ejercicio2();
void *cajero(void * args);

//Ejercicio 1
int stock[3];
int capacidad[3];
int totalProducido[3];
int totalConsumido[3];
int vecesSinStock[3];
int vecesLleno[3];
pthread_mutex_t mutex;
//Ejercicio 2
double saldo = 100.0;
pthread_mutex_t mutexBanco;


char *nombres[3] = {
    "Trigo",
    "Carne",
    "Frutas y Verduras"};

int main() {
    int opcion = 0;

    do {
        printf("\n===================================================\n");
        printf("   HILOS Y PROCESOS\n");
        printf("===================================================\n");
        printf("1. Ejercicio 1 - Fábrica de producción de alimentos\n");
        printf("2. Ejercicio 2 - Banco regional\n");
        printf("3. Salir\n");
        printf("===================================================\n");
        printf("Seleccione una opción: ");

        scanf("%d", &opcion);
        limpiarBuffer();

        switch (opcion) {
        case 1:
            ejercicio1();
            break;
        case 2:
            ejercicio2();
            break;
        case 3:
            printf("Saliendo del programa...\n");
            break;
        default:
            printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (opcion < 3 || opcion > 3);
    return 0;
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ejercicio1() {
    pthread_t hilos[9];
    int ids[9];
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);

    printf("\n--- EJERCICIO 1: Fábrica de producción de alimentos ---\n");
    printf("Capacidad máxima Trigo: ");
    scanf("%d", &capacidad[0]);
    printf("Capacidad máxima Carne: ");
    scanf("%d", &capacidad[1]);
    printf("Capacidad máxima Frutas y Verduras: ");
    scanf("%d", &capacidad[2]);
    limpiarBuffer();

    // Ciclo para iniciar datos
    for (int i = 0; i < 3; i++) {
        stock[i] = 0;
        totalProducido[i] = 0;
        vecesSinStock[i] = 0;
        vecesLleno[i] = 0;
    }

    // Ciclo para crear los hilos, en este caso 9 hilos
    for (int i = 0; i < 9; i++) {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, funcionHilo, &ids[i]);
    }

    // Ciclo para esperar a que los hilos trabajen
    for (int i = 0; i < 9; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Generación de reportes
    printf("\n=========== REPORTE FINAL ===========\n");

    for (int i = 0; i < 3; i++) {
        printf("\nMaterial: %s\n", nombres[i]);
        printf("Total producido: %d\n", totalProducido[i]);
        printf("Total consumido: %d\n", totalConsumido[i]);
        printf("Veces sin stock: %d\n", vecesSinStock[i]);
        printf("Veces bodega llena: %d\n", vecesLleno[i]);
        printf("Stock final: %d\n", stock[i]);
    }

    pthread_mutex_destroy(&mutex);
}

void *funcionHilo(void *args) {
    int id = *(int *)args;

    for (int i = 0; i < 20; i++) {
        int material;
        int cantidad = rand() % 5 + 1;
        pthread_mutex_lock(&mutex);

        // Productores
        if (id < 3) {
            material = id;

            if (stock[material] + cantidad <= capacidad[material]) {
                stock[material] += cantidad;
                totalProducido[material] += cantidad;
                printf("Productor %d produjo %d unidades de %s\nStock actual de %s: %d / %d\n\n", id, cantidad, nombres[material], nombres[material], stock[material], capacidad[material]);
            }
            else {
                int espacio = capacidad[material] - stock[material];

                if (espacio > 0) {
                    stock[material] += espacio;
                    totalProducido[material] += espacio;
                    printf("Productor %d solo pudo almacenar %d unidades de %s (bodega casi llena)\n", id, espacio, nombres[material]);
                } else {
                    vecesLleno[material]++;
                    printf("Productor %d no pudo producir %s (bodega llena)\n", id, nombres[material]);
                }

                printf("Stock actual de %s: %d / %d\n\n", nombres[material], stock[material], capacidad[material]);
            }
        } else { // Consumidores
            material = (id - 3) / 2;

            if (stock[material] > 0) {
                int consumido;

                if (stock[material] >= cantidad) {
                    consumido = cantidad;
                } else {
                    consumido = stock[material];
                }

                stock[material] -= consumido;
                totalConsumido[material] += consumido;
                printf("Consumidor %d consumió %d unidades de %s\nStock restante de %s: %d\n\n", id, consumido, nombres[material], nombres[material], stock[material]);
            }
            else {
                vecesSinStock[material]++;
                printf("Consumidor %d intentó consumir %s pero no hay stock disponible\n\n", id, nombres[material]);
            }
        }

        //Para evitar stock
        if (stock[material] < 0) {
            stock[material] = 0;
        }

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    pthread_exit(NULL);
}

void ejercicio2() {
    pthread_t hilos[4];
     int ids[4];
     srand(time(NULL));
     pthread_mutex_init(&mutexBanco, NULL);
     printf("\n--- EJERCICIO 2: Banco Regional ---\n");
     printf("Registro académico: 201831260\n");
     printf("Saldo inicial: Q. %.2f\n\n", saldo);

     for (int i = 0; i < 4; i++) {
        ids[i] = i + 1;
        pthread_create(&hilos[i], NULL, cajero, &ids[i]);
     }

     for (int i = 0; i < 4; i++) {
        pthread_join(hilos[i], NULL);
     }

    printf("\n=========== RESULTADO FINAL ===========\n");
    printf("Saldo final de la cuenta: Q. %.2f\n", saldo);

    pthread_mutex_destroy(&mutexBanco);
}

void *cajero(void *args) {
    int id = *(int *)args;
    double monto;

    for (int i = 0; i < 20; i++) {
        sleep(rand() % 2 + 1); //tiempo variable
        pthread_mutex_lock(&mutexBanco);
        double saldoTemporal = saldo;

        if (id == 1) {
            monto = 3;
            saldoTemporal += monto;
            saldo = saldoTemporal;
            printf("Cajero 1 (dígito de registro académico = 3) depositó Q. %.2f\nSaldo actual: Q. %.2f\n\n", monto, saldo);
        } else if (id == 2) {
            monto = 1;
            saldoTemporal += monto;
            saldo = saldoTemporal;
            printf("Cajero 2 (dígito de registro académico = 1) depositó Q. %.2f\nSaldo actual: Q. %.2f\n\n", monto, saldo);
        } else if (id == 3) {
            monto = 2;

            if (saldoTemporal >= monto) {
                saldoTemporal -= monto;
                saldo = saldoTemporal;
                printf("Cajero 3 (dígito de registro académico = 2) retiró Q. %.2f\nSaldo actual: Q. %.2f\n\n", monto, saldo);
            } else {
                printf("Cajero 3 (dígito de registro académico = 2) intentó retirar Q. %.2f pero el saldo es insuficiente (Q. %.2f)\n\n", monto, saldo);
            }
        } else if (id = 4) {
            monto = 6;

            if (saldoTemporal >= monto) {
                saldoTemporal -=monto;
                saldo = saldoTemporal;
                printf("Cajero 4 (dígito de registro académico = 6) retiró Q. %.2f\nSaldo actual: Q. %.2f\n\n", monto, saldo);
            } else {
                printf("Cajero 4 (dígito de registro académico = 6) intentó retirar Q. %.2f pero el saldo es insuficiente (Q. %.2f)\n\n", monto, saldo);
            }
        }
        pthread_mutex_unlock(&mutexBanco);
    }
    pthread_exit(NULL);
}