#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define TOTAL_ESPACIOS 20
#define ESPACIOS_VIP 5
#define MAX_CAMIONES_SIMUL 3
#define TOTAL_CARROS 50
#define TOTAL_REPARTO 10
#define MAX_ESPERA_SEG 3

int espacios_ocupados = 0;
int espacios_vip_ocupados = 0;
int camiones_adentro = 0;
int carros_esperando = 0;
int camiones_esperando = 0;
int desistimientos = 0;

// Semáforos y Mutex
sem_t sem_capacidad;
sem_t sem_max_camiones;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Estructura para hilos
typedef struct {
    int id;
    int es_camion;
} Vehiculo;

void registrar_log(const char* evento, Vehiculo* v) {
    pthread_mutex_lock(&log_mutex);
    time_t now = time(NULL);
    char* t_str = ctime(&now);
    t_str[24] = '\0';

    int libres_total = TOTAL_ESPACIOS - espacios_ocupados;
    int libres_vip = ESPACIOS_VIP - espacios_vip_ocupados;

    FILE *f = fopen("log_estacionamiento.txt", "a");
    fprintf(f, "[%s] EVENTO: %s | TIPO: %s | ID: %d | Ocupados: %d | VIP Libres: %d | Esperando: %d | Desistieron: %d\n",
            t_str, evento, v->es_camion ? "Camion" : "Carro", v->id, espacios_ocupados, libres_vip, (carros_esperando + camiones_esperando), desistimientos);
    fclose(f);

    printf("[%s] %s: %s %d (Libres: %d, VIP Libres: %d)\n", 
           t_str, evento, v->es_camion ? "Camion" : "Carro", v->id, libres_total, libres_vip);
    
    pthread_mutex_unlock(&log_mutex);
}

void* proceso_vehiculo(void* arg) {
    Vehiculo* v = (Vehiculo*)arg;
    struct timespec ts;
    
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += MAX_ESPERA_SEG;

    pthread_mutex_lock(&log_mutex);
    if(v->es_camion) camiones_esperando++; else carros_esperando++;
    pthread_mutex_unlock(&log_mutex);

    if (v->es_camion) {
        if (sem_wait(&sem_max_camiones) == 0) {
            if (sem_timedwait(&sem_capacidad, &ts) == -1) goto desistir_camion;
            if (sem_timedwait(&sem_capacidad, &ts) == -1) {
                sem_post(&sem_capacidad);
                goto desistir_camion;
            }
        } else goto desistir;
    } else {
        if (sem_timedwait(&sem_capacidad, &ts) == -1) goto desistir;
    }

    pthread_mutex_lock(&log_mutex);
    if(v->es_camion) {
        camiones_esperando--;
        espacios_ocupados += 2;
        camiones_adentro++;
    } else {
        carros_esperando--;
        espacios_ocupados++;
        if (espacios_vip_ocupados < ESPACIOS_VIP) espacios_vip_ocupados++;
    }
    pthread_mutex_unlock(&log_mutex);

    registrar_log("ENTRADA", v);

    sleep(rand() % 4 + 1);

    pthread_mutex_lock(&log_mutex);
    if(v->es_camion) {
        espacios_ocupados -= 2;
        camiones_adentro--;
        sem_post(&sem_capacidad);
        sem_post(&sem_capacidad);
        sem_post(&sem_max_camiones);
    } else {
        espacios_ocupados--;
        if (espacios_vip_ocupados > 0) espacios_vip_ocupados--;
        sem_post(&sem_capacidad);
    }
    pthread_mutex_unlock(&log_mutex);

    registrar_log("SALIDA", v);
    free(v);
    return NULL;

desistir_camion:
    sem_post(&sem_max_camiones);
desistir:
    pthread_mutex_lock(&log_mutex);
    if(v->es_camion) camiones_esperando--; else carros_esperando--;
    desistimientos++;
    pthread_mutex_unlock(&log_mutex);
    registrar_log("DESISTE (TIMEOUT)", v);
    free(v);
    return NULL;
}


int main() {
    pthread_t hilos[TOTAL_CARROS + TOTAL_REPARTO];
    sem_init(&sem_capacidad, 0, TOTAL_ESPACIOS);
    sem_init(&sem_max_camiones, 0, MAX_CAMIONES_SIMUL);
    
    printf("--- Iniciando Simulación Estacionamiento ---\n");

    for (int i = 0; i < (TOTAL_CARROS + TOTAL_REPARTO); i++) {
        Vehiculo* v = malloc(sizeof(Vehiculo));
        v->id = i + 1;
        v->es_camion = (i < TOTAL_REPARTO) ? 1 : 0;
        pthread_create(&hilos[i], NULL, proceso_vehiculo, v);
        if (i % 2 == 0) usleep(100000);
    }

    for (int i = 0; i < (TOTAL_CARROS + TOTAL_REPARTO); i++) {
        pthread_join(hilos[i], NULL);
    }
    
    sem_destroy(&sem_capacidad);
    sem_destroy(&sem_max_camiones);
    printf("\nSimulación finalizada. Ver log_estacionamiento.txt\n");
    return 0;
}