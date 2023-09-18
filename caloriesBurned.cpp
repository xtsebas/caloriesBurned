#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <string>
#include <omp.h>
using namespace std;

int *listaID; //Lista de ID de estudiantes

int generarID() {
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


int main() {

    srand(static_cast<unsigned>(time(nullptr)));

    listaID = new int[24];

    for (int i = 0; i < 24; ++i) {
        int idConvertido = generarID();
        if (idConvertido != -1) {
            listaID[i] = idConvertido;
        }
    }

    std::cout << "ID de estudiantes:" << std::endl;
    for (int i = 0; i < 24; ++i) {
            cout << (i + 1) << ". " << listaID[i] << endl;
    }

    delete[] listaID;

    return 0;
}
