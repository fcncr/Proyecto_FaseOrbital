#include <iostream>

#include "ArgumentosCLI.h"
#include "Aplicacion.h"

//valida los argumentos y ejecuta el modo solicitado o muestra el uso cuando no son validos
int main(int argc, char* argv[]) {
    ConfiguracionPrograma config; //recibe el modo y las opciones que establezca el lector de argumentos

    if (!parsearArgumentos(argc, argv, config)) { //comprueba los argumentos y completa la configuracion
        cerr << "Argumentos invalidos." << '\n'; //informa que no puede iniciar con esas opciones
        cerr << '\n'; //separa el error de las instrucciones de uso

        imprimirUso(cerr); //muestra las formas de ejecutar el programa

        return 1; //termina con error sin ejecutar ningun modo
    }

    return ejecutarAplicacion(config); //devuelve al sistema el resultado del modo seleccionado
}