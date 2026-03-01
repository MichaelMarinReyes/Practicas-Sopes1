#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int esNumeroValido(char *cadena);
int obtenerDigito(int numero, int posicion);
void ejercicio1();
void ejercicio2();
void ejercicio3();
void limpiarBuffer();

int main(void) {
    int opcion = 0;

    do {
        printf("\n========================================\n");
        printf("   GESTIÓN DE PROCESOS EN C\n");
        printf("========================================\n");
        printf("1. Ejercicio 1 - Conversor de Monedas\n");
        printf("2. Ejercicio 2 - Cierre de Caja\n");
        printf("3. Ejercicio 3 - Listado de Directorio\n");
        printf("4. Salir\n");
        printf("========================================\n");
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
                ejercicio3();
                break;
            case 4:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (opcion != 4);

    return 0;
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int esNumeroValido(char *cadena) {
    int puntoDecimal = 0;
    for (int i = 0; cadena[i] != '\0'; i++) {
        if (cadena[i] == '.') {
            if (puntoDecimal) return 0;
            puntoDecimal = 1;
        } else if (!isdigit(cadena[i])) {
            return 0;
        }
    }
    return 1;
}

int obtenerDigito(int numero, int posicion) {
    if (numero == 0) return 1;
    if (numero < 0) numero = -numero;

    char numStr[20];
    sprintf(numStr, "%d", numero);

    int len = strlen(numStr);
    if (posicion >= len) return 1;
    return numStr[posicion] - '0';
}

void ejercicio1() {
    char entrada[100];
    float cantidad_gtq;

    printf("\n--- EJERCICIO 1: CONVERSOR DE MONEDAS ---\n");
    printf("Ingrese la cantidad en Quetzales (GTQ): ");
    fgets(entrada, sizeof(entrada), stdin);
    entrada[strcspn(entrada, "\n")] = 0;

    if (!esNumeroValido(entrada)) {
        printf("Error: El valor ingresado no es un número válido\n");
        return;
    }

    cantidad_gtq = atof(entrada);

    if (cantidad_gtq <= 0) {
        printf("Error: La cantidad debe ser mayor a cero\n");
        return;
    }

    printf("\nProceso Padre (PID: %d) - Cantidad a convertir: %.2f GTQ\n", getpid(), cantidad_gtq);
    printf("Creando procesos hijos...\n\n");

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("Error al crear el primer hijo");
        return;
    }

    if (pid1 == 0) {
        float dolares = cantidad_gtq / 7.75;
        printf("=== HIJO 1 (PID: %d, Padre: %d) ===\n", getpid(), getppid());
        printf("Conversión a Dólares (USD):\n");
        printf("%.2f GTQ = %.2f USD (Tasa: 1 USD = 7.75 GTQ)\n\n", cantidad_gtq, dolares);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("Error al crear el segundo hijo");
        return;
    }

    if (pid2 == 0) {
        float pesos = cantidad_gtq / 0.39;
        printf("=== HIJO 2 (PID: %d, Padre: %d) ===\n", getpid(), getppid());
        printf("Conversión a Pesos Mexicanos (MXN):\n");
        printf("%.2f GTQ = %.2f MXN (Tasa: 1 MXN = 0.39 GTQ)\n\n", cantidad_gtq, pesos);
        exit(EXIT_SUCCESS);
    }

    pid_t pid3 = fork();
    if (pid3 < 0) {
        perror("Error al crear el tercer hijo");
        return;
    }

    if (pid3 == 0) {
        float euros = cantidad_gtq / 8.40;
        printf("=== HIJO 3 (PID: %d, Padre: %d) ===\n", getpid(), getppid());
        printf("Conversión a Euros (EUR):\n");
        printf("%.2f GTQ = %.2f EUR (Tasa: 1 EUR = 8.40 GTQ)\n\n", cantidad_gtq, euros);
        exit(EXIT_SUCCESS);
    }

    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    waitpid(pid3, &status, 0);

    printf("Todos los procesos hijos han terminado.\n");
    printf("Presione Enter para continuar...");
    getchar();
}

void ejercicio2() {
    char ingreso_str[100];
    char registro_str[100] = "201831260";
    float ingreso_total;
    int registro;

    printf("\n--- EJERCICIO 2: CIERRE DE CAJA ---\n");

    printf("Ingrese el ingreso total del día (Q): ");
    fgets(ingreso_str, sizeof(ingreso_str), stdin);
    ingreso_str[strcspn(ingreso_str, "\n")] = 0;

    //printf("Ingrese su registro académico (solo números): ");
    //fgets(registro_str, sizeof(registro_str), stdin);
    registro_str[strcspn(registro_str, "\n")] = 0;

    if (!esNumeroValido(ingreso_str)) {
        printf("Error: El ingreso debe ser un número válido\n");
        return;
    }

    for (int i = 0; registro_str[i] != '\0'; i++) {
        if (!isdigit(registro_str[i])) {
            printf("Error: El registro académico debe contener solo dígitos\n");
            return;
        }
    }

    ingreso_total = atof(ingreso_str);
    registro = atoi(registro_str);

    printf("\n=== PROCESO PADRE (PID: %d) ===\n", getpid());
    printf("Ingreso total del día: Q%.2f\n", ingreso_total);
    printf("Registro académico: %d\n\n", registro);

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("Error al crear el primer hijo");
        return;
    }

    if (pid1 == 0) {
        printf("=== HIJO 1 (PID: %d, Padre: %d) - GASTOS OPERATIVOS ===\n",
               getpid(), getppid());

        int digito1 = obtenerDigito(registro, 0);
        int digito2 = obtenerDigito(registro, 1);
        int digito3 = obtenerDigito(registro, 2);
        int digito4 = obtenerDigito(registro, 3);
        float sueldo = 85.00 * digito1;
        float electricidad = 40.00 * digito2;
        float agua = 15.00 * digito3;
        float renta = 60.00 * digito4;

        float total_gastos = sueldo + electricidad + agua + renta;

        printf("Desglose de gastos:\n");
        printf("  Sueldo empleado: Q85.00 * %d = Q%.2f\n", digito1, sueldo);
        printf("  Electricidad: Q40.00 * %d = Q%.2f\n", digito2, electricidad);
        printf("  Agua estimada: Q15.00 * %d = Q%.2f\n", digito3, agua);
        printf("  Renta del local: Q%.2f\n", renta);
        printf("  TOTAL GASTOS: Q%.2f\n\n", total_gastos);

        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("Error al crear el segundo hijo");
        return;
    }

    if (pid2 == 0) {
        int digito1 = obtenerDigito(registro, 0);
        int digito2 = obtenerDigito(registro, 1);
        int digito3 = obtenerDigito(registro, 2);
        float sueldo = 85.00 * digito1;
        float electricidad = 40.00 * digito2;
        float agua = 15.00 * digito3;
        float renta = 60.00;
        float total_gastos = sueldo + electricidad + agua + renta;

        float ganancia_neta = ingreso_total - total_gastos;

        printf("=== HIJO 2 (PID: %d, Padre: %d) - GANANCIA NETA ===\n", getpid(), getppid());
        printf("Ingreso total: Q%.2f\n", ingreso_total);
        printf("Gastos totales: Q%.2f\n", total_gastos);
        printf("Ganancia neta: Q%.2f\n", ganancia_neta);

        if (ganancia_neta > total_gastos) {
            printf("RESULTADO: Rentable (ganancia > gastos)\n");
            exit(EXIT_SUCCESS);
        } else {
            printf("RESULTADO: Con pérdida (ganancia < gastos)\n");
            exit(EXIT_FAILURE);
        }
    }

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    printf("\n=== PROCESO PADRE - RESUMEN DE CIERRE ===\n");

    if (WIFEXITED(status1)) {
        printf("Hijo 1 (Gastos) terminó con código: %d\n", WEXITSTATUS(status1));
    }

    if (WIFEXITED(status2)) {
        int codigo_hijo2 = WEXITSTATUS(status2);
        printf("Hijo 2 (Ganancia) terminó con código: %d\n", codigo_hijo2);

        if (codigo_hijo2 == EXIT_SUCCESS) {
            printf("CIERRE DEL DÍA: Exitoso (hubo ganancia)\n");
        } else {
            printf("CIERRE DEL DÍA: Se registraron pérdidas\n");
        }
    }

    printf("\nPresione Enter para continuar...");
    getchar();
}

void ejercicio3() {
    char directorio[256];

    printf("\n--- EJERCICIO 3: LISTADO DE DIRECTORIO ---\n");
    printf("Ingrese la ruta del directorio a listar: ");
    fgets(directorio, sizeof(directorio), stdin);
    directorio[strcspn(directorio, "\n")] = 0;

    printf("\n=== PROCESO PADRE (PID: %d) ===\n", getpid());
    printf("Directorio a listar: %s\n\n", directorio);
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error al crear el proceso hijo");
        return;
    }

    if (pid == 0) {
        printf("=== HIJO (PID: %d, Padre: %d) - EJECUTANDO ls -l ===\n",
               getpid(), getppid());

        char *args[] = {"ls", "-l", directorio, NULL};

        execvp("ls", args);
        perror("Error al ejecutar ls");
        printf("El directorio '%s' posiblemente no existe\n", directorio);
        exit(EXIT_FAILURE);
    }

    int status;
    waitpid(pid, &status, 0);
    printf("\n=== PROCESO PADRE - RESULTADO ===\n");

    if (WIFEXITED(status)) {
        int codigo_salida = WEXITSTATUS(status);
        if (codigo_salida == EXIT_SUCCESS) {
            printf("El proceso hijo se ejecutó correctamente\n");
        } else {
            printf("El proceso hijo terminó con error (código: %d)\n", codigo_salida);
        }
    } else {
        printf("El proceso hijo no terminó normalmente\n");
    }

    printf("\nPresione Enter para continuar...");
    getchar();
}