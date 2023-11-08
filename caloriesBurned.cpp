#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <pthread.h>
#include <random>
#include <vector>
#include <mutex>

using namespace std;

// Arreglos globales para almacenar los IDs de los estudiantes, las calorías quemadas y el estado de notificación.
int *listaID; // Lista de ID de estudiantes
int *listaCalories; // Lista de calorías por estudiante correspondiente de manera paralela
bool *notificado; // Lista para rastrear si ya hemos notificado sobre un estudiante
int numThreads = 24; // Número de hilos para crear, representando el número de estudiantes.

// Mutex para sincronizar el acceso a recursos compartidos.
std::mutex dataMutex;

// Estructura para pasar datos a los hilos.
struct ThreadData {
    int id;
};

int generarID() {
    /**
     * Función para generar de manera aleatoria cada uno de los IDs para los diferentes estudiantes.
     */
    std::string id;
    for (int i = 0; i < 5; ++i) {
        int numeroAleatorio = rand() % 10;
        id += std::to_string(numeroAleatorio);
    }

    // Intentar convertir el ID generado de string a entero.
    try {
        int idEntero = std::stoi(id);
        return idEntero;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: No se pudo convertir la cadena a entero." << std::endl;
        return 0;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: El valor está fuera del rango de representación de un entero." << std::endl;
        return 0;
    }
}

void sumaPorUsuario(int id) {
    // Genera una semilla única basada en el tiempo actual y el ID del hilo.
    unsigned seed = static_cast<unsigned>(time(nullptr)) + id;
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> dist(1, 25);

    // Simula la quema de calorías a lo largo del tiempo.
    for (int tiempo = 0; tiempo < 50; ++tiempo) {
        if (dist(generator) < 60) {
            std::uniform_int_distribution<int> distCal(1, 25);
            int caloriasQuemadas = distCal(generator);

            // Usa mutex para proteger el acceso al recurso compartido listaCalories.
            std::lock_guard<std::mutex> lock(dataMutex);
            listaCalories[id] += caloriasQuemadas;
        }
    }
}

bool verificador(int index) {
    // Protege los recursos compartidos con un mutex.
    std::lock_guard<std::mutex> lock(dataMutex);
    // Verifica si el estudiante ha quemado 500 o más calorías y no ha sido notificado.
    if (listaCalories[index] >= 500 && !notificado[index]) {
        cout << "El estudiante con ID: " << listaID[index] << " ha quemado 500 calorías o más." << endl;
        notificado[index] = true;
        return true;
    }
    return false;
}

void *threadFunction(void *data) {
    ThreadData *threadData = (ThreadData *)data;
    int id = threadData->id;

    // Realiza la simulación de quema de calorías y verifica la notificación.
    sumaPorUsuario(id);
    verificador(id);

    pthread_exit(NULL);
}

int main() {
    // Inicializa el generador de números aleatorios.
    srand(static_cast<unsigned>(time(nullptr)));

    // Inicializa los arreglos globales.
    notificado = new bool[24]();
    listaID = new int[24];
    listaCalories = new int[24]();

    // Crea los hilos.
    pthread_t threads[numThreads];
    std::vector<ThreadData> threadData(numThreads);

    // Genera los IDs para todos los estudiantes.
    for (int i = 0; i < 24; ++i) {
        int idConvertido = generarID();
        // Verifica que el ID generado sea válido.
        if (idConvertido != 0) {
            listaID[i] = idConvertido;
        }
    }

    // Inicia la simulación de quema de calorías para cada estudiante en un hilo separado.
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].id = i;
        pthread_create(&threads[i], NULL, threadFunction, &threadData[i]);
    }

    // Espera a que todos los hilos terminen.
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Calcula e imprime el total de calorías quemadas por la clase.
    int totalCalorias = 0;
    for (int i = 0; i < 24; ++i) {
        totalCalorias += listaCalories[i];
    }
    cout << "Total de calorías quemadas por la clase: " << totalCalorias << endl;

    // Imprime las calorías quemadas por cada estudiante.
    for (int i = 0; i < 24; ++i) {
        cout << "Estudiante con ID: " << listaID[i] << " quemó " << listaCalories[i] << " calorías." << endl;
    }

    // Libera la memoria dinámicamente asignada.
    delete[] listaID;
    delete[] listaCalories;
    delete[] notificado;

    return 0;
}
