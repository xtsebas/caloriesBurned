#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <string>
#include <omp.h>
#include <random>
using namespace std;

int *listaID; //Lista de ID de estudiantes
int *listaCalories; //Lista de calorias por estudiante correspondiente de manera paralela
bool *notificado; // Lista para rastrear si ya hemos notificado sobre un estudiante


int generarID() {
    /**
     * Funcion para generar de manera aletoria cada uno de los IDs para los diferentes usuarios.
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

void sumaPorUsuario() {
    // Directiva OpenMP para ejecutar el bucle for en paralelo con multiples hilos.
    #pragma omp parallel for
    for (int i = 0; i < 24; ++i) {

        // Genera una semilla unica basada en el tiempo actual y el numero de hilo.
        unsigned seed = static_cast<unsigned>(time(nullptr)) + omp_get_thread_num();
        
        // Inicializa el generador de numeros aleatorios Mersenne Twister con la semilla generada.
        std::mt19937 generator(seed);
        
        // Define una distribuciun uniforme que generaru numeros entre 1 y 25.
        std::uniform_int_distribution<int> dist(1, 25);

        if (dist(generator) < 60) {
            
            // Define una nueva distribución uniforme para generar calorías quemadas entre 1 y 25.
            std::uniform_int_distribution<int> distCal(1, 25);
            
            // Genera un número aleatorio que representa las calorías quemadas en esta iteración.
            int caloriasQuemadas = distCal(generator);
            
            // Suma las calorías quemadas al total de calorías del estudiante.
            listaCalories[i] += caloriasQuemadas;
            
        }
    }
}



bool verificador(int index) {
    /**
     * Notifica al usuario que ID ya ha llegado a las 500 calorias o mas
    */
    if (listaCalories[index] >= 500 && !notificado[index]) {

        //Valor "Critical" para utilizar unicamente un hilo por notificacion
        #pragma omp critical
        {
            cout << "El estudiante con ID: " << listaID[index] << " ha quemado 500 calorias o mas." << endl;
        }
        notificado[index] = true; 
        return true;
    }
    return false;
}


int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    notificado = new bool[24](); 
    listaID = new int[24];
    listaCalories = new int[24](); 

    for (int i = 0; i < 24; ++i) {
        int idConvertido = generarID();
        if (idConvertido != -1) {
            listaID[i] = idConvertido;
        }
    }

    // Simulación de quema de calorías. Implementacion
    for (int tiempo = 0; tiempo < 50; ++tiempo) { 
        sumaPorUsuario();
        for (int i = 0; i < 24; ++i) {
            verificador(i);
        }
    }

    int totalCalorias = 0; //Variable para almacenar la suma total de calorías quemadas por todos los estudiantes 
    #pragma omp parallel for reduction(+:totalCalorias) //Crear una copia privada de "totalCalorias" para cada hilo.
    for(int i = 0; i < 24; ++i){
        totalCalorias += listaCalories[i];
    }

    //Imprimir el total de calorias quemadas por la clase
    cout << "Total de calorias quemadas por la clase: " << totalCalorias << endl;

    //Imprimir el total de calorias quemadas por la clase por estudiante
    for(int i = 0; i < 24; ++i){
        cout << "Estudiante con ID: " << listaID[i] << " quemo " << listaCalories[i] << " calorias." << endl;
    }

    delete[] listaID;
    delete[] listaCalories;
    delete[] notificado;

    return 0;
}