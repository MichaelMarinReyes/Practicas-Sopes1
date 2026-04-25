import java.util.Scanner;
import java.util.concurrent.ThreadLocalRandom;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

public class SimuladorAeropuerto {

    private static final Object TRAMO_A = new Object();
    private static final Object TRAMO_B = new Object();

    private static final ReentrantLock LOCK_A = new ReentrantLock();
    private static final ReentrantLock LOCK_B = new ReentrantLock();

    private static Scanner sc = new Scanner(System.in);

    public static void main(String[] args) {
        int opcion = 0;
        do {
            try {
                System.out.println("\n--- SIMULADOR DE TRÁFICO AÉREO ---");
                System.out.println("1. Escenario con Interbloqueo (Deadlock)");
                System.out.println("2. Escenario Seguro (Prevención de Deadlock)");
                System.out.println("3. Salir");
                System.out.print("Seleccione una opción: ");

                if (sc.hasNextInt()) {
                    opcion = sc.nextInt();
                    if (opcion == 1 || opcion == 2) {
                        iniciarSimulacion(opcion);
                    }
                } else {
                    System.out.println("Error: Por favor, ingrese un número entero.");
                    sc.next();
                }
            } catch (Exception e) {
                System.out.println("Ocurrió un error inesperado: " + e.getMessage());
            }
        } while (opcion < 1 || opcion > 3);
    }

    private static void iniciarSimulacion(int tipoEscenario) {
        System.out.print("Ingrese número de aviones en TIERRA (Despegue): ");
        int tierra = validarEntrada();
        System.out.print("Ingrese número de aviones en AIRE (Aterrizaje): ");
        int aire = validarEntrada();

        List<Thread> flota = new ArrayList<>();

        for (int i = 1; i <= tierra; i++) {
            String nombre = "Avion-T-" + i;
            flota.add(new Thread(() -> {
                if (tipoEscenario == 1)
                    ejecutarDespegueDeadlock(nombre);
                else
                    ejecutarDespegueSeguro(nombre);
            }));
        }

        for (int i = 1; i <= aire; i++) {
            String nombre = "Avion-A-" + i;
            flota.add(new Thread(() -> {
                if (tipoEscenario == 1)
                    ejecutarAterrizajeDeadlock(nombre);
                else
                    ejecutarAterrizajeSeguro(nombre);
            }));
        }

        System.out.println("\n--- INICIANDO SIMULACIÓN ---");
        flota.forEach(Thread::start);

        if (tipoEscenario == 2) {
            flota.forEach(t -> {
                try {
                    t.join();
                } catch (InterruptedException e) {
                }
            });
            System.out.println("\n--- SIMULACIÓN FINALIZADA SIN BLOQUEOS ---");
        } else {
            System.out.println("\n[AVISO] Si el sistema se congela, se ha alcanzado un DEADLOCK.");
            System.out.println("[AVISO] Presione Ctrl+C para forzar la salida si es necesario.\n");
        }
    }

    // --- ESCENARIO 1: Lógica propensa a Deadlock -> Orden Inverso
    private static void ejecutarDespegueDeadlock(String id) {
        synchronized (TRAMO_A) {
            System.out.println("[" + id + "] RESERVA: Tramo A (Etapa 1)");
            pausaAleatoria(1000, 2000);
            System.out.println("[" + id + "] ESPERA: Solicitando Tramo B (Etapa 2)...");
            synchronized (TRAMO_B) {
                System.out.println("[" + id + "] DESPEGUE: Pista completa adquirida.");
            }
        }
        System.out.println("[" + id + "] LIBERACIÓN: Pista libre.");
    }

    private static void ejecutarAterrizajeDeadlock(String id) {
        /* Al solicitar B y luego A -> orden inverso al despegue, se causa la espera
         circular */
        synchronized (TRAMO_B) {
            System.out.println("[" + id + "] RESERVA: Tramo B (Aproximación)");
            pausaAleatoria(500, 1000);
            System.out.println("[" + id + "] ESPERA: Solicitando Tramo A (Final)...");
            synchronized (TRAMO_A) {
                System.out.println("[" + id + "] ATERRIZAJE: Pista completa adquirida.");
            }
        }
        System.out.println("[" + id + "] LIBERACIÓN: Pista libre.");
    }

    // --- ESCENARIO 2: Lógica Segura -> Jerarquía de Recursos y TryLock
    private static void ejecutarDespegueSeguro(String id) {
        boolean completado = false;
        while (!completado) {
            if (LOCK_A.tryLock()) {
                try {
                    System.out.println("[" + id + "] RESERVA: Tramo A (Modo Seguro)");
                    pausaAleatoria(500, 1000);
                    if (LOCK_B.tryLock()) {
                        try {
                            System.out.println("[" + id + "] DESPEGUE: Realizado con éxito.");
                            completado = true;
                        } finally {
                            LOCK_B.unlock();
                        }
                    }
                } finally {
                    LOCK_A.unlock();
                    if (!completado) {
                        System.out.println("[" + id + "] REINTENTO: No pudo obtener tramo B, liberando A.");
                        pausaAleatoria(200, 500);
                    }
                }
            }
        }
    }

    private static void ejecutarAterrizajeSeguro(String id) {
        // En modo seguro, acá ambos siguen el orden A -> B o usan mecanismos de escape
        boolean completado = false;
        while (!completado) {
            if (LOCK_A.tryLock()) {
                try {
                    System.out.println("[" + id + "] RESERVA: Tramo A (Aterrizaje Seguro)");
                    if (LOCK_B.tryLock()) {
                        try {
                            System.out.println("[" + id + "] ATERRIZAJE: Realizado con éxito.");
                            completado = true;
                        } finally {
                            LOCK_B.unlock();
                        }
                    }
                } finally {
                    LOCK_A.unlock();
                    if (!completado)
                        pausaAleatoria(200, 500);
                }
            }
        }
    }

    // --- UTILIDADES ---
    private static int validarEntrada() {
        while (!sc.hasNextInt()) {
            System.out.print("Entrada inválida. Ingrese un número: ");
            sc.next();
        }
        return sc.nextInt();
    }

    private static void pausaAleatoria(int min, int max) {
        try {
            Thread.sleep(ThreadLocalRandom.current().nextInt(min, max));
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}