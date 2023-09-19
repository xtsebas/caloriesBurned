#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <string>
#include <omp.h>
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
    /**
     * Se realizar la suma de las calorias por posicion, dado que ambas son listas se usan cada una con su
     * posicion de 0 a 23. Esto se realiza de manera paralela
    */
    //Necesaria paralelización <----------------
    for (int i = 0; i < 24; ++i) {
        if (rand() % 100 < 60) {  //Factor de cambio para mejorar la diferencia de datos
            int caloriasQuemadas = rand() % 20 + 1; // Quema de calorías entre 1 y 20
            listaCalories[i] += caloriasQuemadas;
        }
    }
}

bool verificador(int index) {
    /**
     * Notifica al usuario que ID ya ha llegado a las 500 calorias o mas
    */
    if (listaCalories[index] >= 500 && !notificado[index]) {
        //Valor "Critical" para utilizar unicamente un hilo por notificacion <------------------------
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

    delete[] listaID;
    delete[] listaCalories;

    return 0;
}