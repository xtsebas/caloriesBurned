#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <pthread.h>
#include <random>
#include <vector>
#include <mutex>

using namespace std;

int *listaID; // Lista de ID de estudiantes
int *listaCalories; // Lista de calorías por estudiante correspondiente de manera paralela
bool *notificado; // Lista para rastrear si ya hemos notificado sobre un estudiante
int numThreads = 4; // Número de hilos

// Mutex para garantizar acceso seguro a los datos compartidos
std::mutex dataMutex;

// Estructura para pasar datos a cada hilo
struct ThreadData {
    int id;
};

int generarID() {
    /**
     * Función para generar de manera aleatoria cada uno de los IDs para los diferentes usuarios.
     */
    std::string id;
    for (int i = 0; i < 5; ++i) {
        int numeroAleatorio = rand() % 10;
        id += std::to_string(numeroAleatorio);
    }

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

    for (int tiempo = 0; tiempo < 50; ++tiempo) {
        if (dist(generator) < 60) {
            std::uniform_int_distribution<int> distCal(1, 25);
            int caloriasQuemadas = distCal(generator); // Mover la definición aquí

            std::lock_guard<std::mutex> lock(dataMutex); // Usar lock_guard
            listaCalories[id] += caloriasQuemadas;
        }
    }
}

bool verificador(int index) {
    std::lock_guard<std::mutex> lock(dataMutex); // Usar lock_guard
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

    sumaPorUsuario(id);
    verificador(id);

    pthread_exit(NULL);
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    notificado = new bool[24]();
    listaID = new int[24];
    listaCalories = new int[24]();

    // Crear hilos
    pthread_t threads[numThreads];
    std::vector<ThreadData> threadData(numThreads);

    for (int i = 0; i < 24; ++i) {
        int idConvertido = generarID();
        if (idConvertido != -1) {
            listaID[i] = idConvertido;
        }
    }

    // Simulación de quema de calorías
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].id = i;
        pthread_create(&threads[i], NULL, threadFunction, &threadData[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    int totalCalorias = 0; // Variable para almacenar la suma total de calorías quemadas por todos los estudiantes
    for (int i = 0; i < 24; ++i) {
        totalCalorias += listaCalories[i];
    }

    // Imprimir el total de calorías quemadas por la clase
    cout << "Total de calorías quemadas por la clase: " << totalCalorias << endl;

    // Imprimir el total de calorías quemadas por la clase por estudiante
    for (int i = 0; i < 24; ++i) {
        cout << "Estudiante con ID: " << listaID[i] << " quemó " << listaCalories[i] << " calorías." << endl;
    }

    delete[] listaID;
    delete[] listaCalories;
    delete[] notificado;

    return 0;
}
