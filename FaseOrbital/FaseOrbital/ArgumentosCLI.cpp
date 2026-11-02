#include "ArgumentosCLI.h"
#include <iostream> //permite mostrar el mensaje del alias repetido
#include <sstream>
#include <limits>

//quita espacios tabulaciones y saltos de linea de los extremos sin cambiar el interior del texto
static string recortarTexto(const string& texto) {
    size_t inicio = 0; //empieza a buscar el primer caracter que pertenece al alias

    while (inicio < texto.size() && (texto[inicio] == ' ' || texto[inicio] == '\t' || texto[inicio] == '\r' || texto[inicio] == '\n')) { //omite los espacios permitidos al inicio
        inicio++; //avanza hasta encontrar texto o llegar al final
    }

    size_t fin = texto.size(); //situa el limite justo despues del ultimo caracter

    while (fin > inicio && (texto[fin - 1] == ' ' || texto[fin - 1] == '\t' || texto[fin - 1] == '\r' || texto[fin - 1] == '\n')) { //omite los espacios finales sin pasar el inicio
        fin--; //retrocede hasta encontrar el ultimo caracter del alias
    }

    return texto.substr(inicio, fin - inicio); //devuelve el texto recortado o una cadena vacia si solo habia espacios
}

//reconoce los textos que empiezan con dos guiones para no tomarlos como valores de otra opcion
static bool pareceOpcion(const string& texto) {
    return texto.size() >= 2 && texto[0] == '-' && texto[1] == '-'; //comprueba el prefijo sin acceder fuera del texto
}

//comprueba que una opcion tenga un argumento siguiente no vacio que no empiece con dos guiones
static bool hayValorDespues(int indice, int argc, char* argv[]) {
    if (indice + 1 >= argc) { //detecta una opcion colocada al final del comando
        return false; //indica que falta su valor
    }

    string siguiente = argv[indice + 1]; //consulta el argumento que sigue a la opcion

    if (siguiente.empty()) { //comprueba si se recibio un argumento sin contenido
        return false; //rechaza el valor vacio
    }

    if (pareceOpcion(siguiente)) { //detecta que el siguiente argumento parece otra opcion
        return false; //evita consumir esa opcion como si fuera el valor pendiente
    }

    return true; //confirma que hay un argumento que se puede revisar como valor
}

//convierte el texto de la semilla y comprueba que el valor leido quepa en un entero sin signo
static bool convertirSemilla(const string& texto, unsigned int& valor) {
    if (texto.empty()) { //comprueba si no hay un numero para leer
        return false; //rechaza una semilla sin contenido
    }

    if (texto[0] == '-') { //detecta un signo negativo al inicio del argumento
        return false; //rechaza esta forma de escribir una semilla negativa
    }

    istringstream entrada(texto); //prepara la lectura numerica desde la cadena

    unsigned long long temporal; //recibe el numero en un tipo mayor antes de comprobar el limite
    char extra; //permite detectar texto sobrante despues del numero

    if (!(entrada >> temporal)) { //intenta leer la semilla como un entero
        return false; //rechaza el texto si no se puede convertir
    }

    if (entrada >> extra) { //busca caracteres adicionales que no sean espacios
        return false; //rechaza letras separadores decimales y otros datos despues del entero
    }

    if (temporal > numeric_limits<unsigned int>::max()) { //comprueba el mayor valor que puede guardar la configuracion
        return false; //rechaza una semilla fuera de rango
    }

    valor = static_cast<unsigned int>(temporal); //guarda la semilla despues de validar su rango

    return true; //confirma que la conversion termino
}

//convierte el limite de oleadas a un entero mayor que cero y dentro del rango permitido por int
static bool convertirOleadas(const string& texto, int& valor) {
    if (texto.empty()) { //comprueba si el argumento no tiene contenido
        return false; //rechaza un limite sin numero
    }

    istringstream entrada(texto); //prepara la lectura del limite de oleadas

    long long temporal; //permite revisar el rango antes de guardar el valor en un int
    char extra; //recibira cualquier caracter sobrante despues del numero

    if (!(entrada >> temporal)) { //intenta leer un entero desde el texto
        return false; //rechaza los datos que no se pueden convertir
    }

    if (entrada >> extra) { //comprueba si queda algo distinto de espacios
        return false; //rechaza un numero acompanado de caracteres adicionales
    }

    if (temporal <= 0 || temporal > numeric_limits<int>::max()) { //exige un limite positivo que quepa en el tipo usado
        return false; //rechaza cero valores negativos y numeros fuera de rango
    }

    valor = static_cast<int>(temporal); //guarda el limite ya validado

    return true; //confirma que el numero de oleadas es valido
}

//separa los alias por comas recorta sus extremos y rechaza nombres repetidos
static bool parsearListaJugadores(const string& texto, ConfiguracionPrograma& config) {
    config.cantidadJugadores = 0; //empieza a llenar el arreglo desde la primera posicion

    if (texto.empty()) { //comprueba si la lista completa esta vacia
        return false; //exige al menos un participante
    }

    size_t inicio = 0; //indica donde comienza el alias que se esta leyendo

    while (inicio <= texto.size()) { //recorre la lista incluyendo un posible campo vacio al final
        size_t coma = texto.find(',', inicio); //busca el separador del siguiente alias
        string alias; //guarda temporalmente el nombre del participante

        if (coma == string::npos) { //comprueba si este es el ultimo alias
            alias = texto.substr(inicio); //toma el texto restante
        }
        else { //atiende un alias seguido por otro participante
            alias = texto.substr(inicio, coma - inicio); //extrae el texto anterior a la coma
        }

        alias = recortarTexto(alias); //elimina los espacios de los extremos

        if (alias.empty()) { //comprueba si el nombre quedo vacio
            return false; //rechaza participantes sin alias
        }

        if (config.cantidadJugadores >= 6) { //comprueba si ya se ocuparon los seis espacios
            return false; //evita agregar un septimo participante
        }

        for (int i = 0; i < config.cantidadJugadores; i++) { //recorre los alias que ya fueron aceptados
            if (config.jugadores[i] == alias) { //comprueba si el nombre ya esta registrado
                cerr << "Alias repetido en la partida: " << alias << '\n'; //muestra el alias que impide continuar
                return false; //rechaza la lista de participantes
            }
        }

        config.jugadores[config.cantidadJugadores] = alias; //guarda el alias respetando el orden recibido
        config.cantidadJugadores++; //cuenta al participante aceptado

        if (coma == string::npos) { //comprueba si ya se proceso el ultimo alias
            break; //termina el recorrido
        }

        inicio = coma + 1; //continua despues del separador
    }

    return config.cantidadJugadores > 0; //confirma que se registro al menos un participante
}

//restablece todos los campos para que una lectura nueva no conserve opciones del comando anterior
void inicializarConfiguracionPrograma(ConfiguracionPrograma& config) {
    config.modo = MODO_INVALIDO; //mantiene la ejecucion pendiente hasta validar los argumentos

    for (int i = 0; i < 6; i++) { //recorre todas las posiciones reservadas para jugadores
        config.jugadores[i].clear(); //borra cualquier alias de una configuracion anterior
    }

    config.cantidadJugadores = 0; //indica que todavia no hay participantes recibidos

    config.semilla = 0; //deja un valor inicial hasta leer la opcion obligatoria en lotes
    config.oleadas = 10; //usa diez oleadas cuando no se indica otro limite

    config.rutaScript.clear(); //borra la ruta de acciones anterior
    config.tieneScript = false; //indica que el comando aun no incluye un script

    config.rutaHistorial = "historial.csv"; //establece el archivo usado si el modo no recibe otra ruta
}

//lee las opciones de lotes en cualquier orden sobre una configuracion ya inicializada
//exige jugadores y semilla permite oleadas y script y rechaza opciones repetidas o desconocidas
static bool parsearModoLotes(int argc, char* argv[], ConfiguracionPrograma& config) {
    bool tieneJugadores = false; //recuerda si ya se recibio la lista obligatoria
    bool tieneSemilla = false; //distingue una semilla recibida del cero predeterminado
    bool tieneOleadas = false; //permite detectar si se repite el limite opcional

    config.modo = MODO_LOTES; //selecciona el modo mientras revisa sus argumentos

    for (int i = 2; i < argc; i++) { //comienza despues del nombre del programa y del modo
        string opcion = argv[i]; //consulta la opcion que toca procesar

        if (opcion == "--jugadores") { //reconoce la lista de participantes
            if (tieneJugadores) { //comprueba si ya se habia recibido esta opcion
                return false; //rechaza una segunda lista de jugadores
            }

            if (!hayValorDespues(i, argc, argv)) { //comprueba que exista un valor para la lista
                return false; //rechaza la opcion si falta su argumento
            }

            if (!parsearListaJugadores(argv[i + 1], config)) { //separa y valida los alias recibidos
                return false; //detiene la lectura si la lista no es valida
            }

            tieneJugadores = true; //marca que la opcion obligatoria ya fue atendida
            i++; //salta el argumento que acaba de consumir como lista
        }
        else if (opcion == "--semilla") { //reconoce el valor que reproducira las oleadas
            if (tieneSemilla) { //comprueba si ya se habia recibido una semilla
                return false; //rechaza la opcion repetida
            }

            if (!hayValorDespues(i, argc, argv)) { //comprueba que haya un argumento numerico para revisar
                return false; //rechaza la opcion sin valor
            }

            if (!convertirSemilla(argv[i + 1], config.semilla)) { //valida el formato y el rango de la semilla
                return false; //detiene la lectura si la conversion falla
            }

            tieneSemilla = true; //recuerda que se recibio la semilla aunque su valor sea cero
            i++; //salta el argumento usado para la conversion
        }
        else if (opcion == "--oleadas") { //reconoce el limite opcional de la partida
            if (tieneOleadas) { //comprueba si ya se habia indicado otro limite
                return false; //rechaza repetir la opcion de oleadas
            }

            if (!hayValorDespues(i, argc, argv)) { //comprueba que la opcion tenga un valor
                return false; //rechaza el limite ausente
            }

            if (!convertirOleadas(argv[i + 1], config.oleadas)) { //valida que el limite sea un entero positivo
                return false; //rechaza el numero si no cumple el formato o el rango
            }

            tieneOleadas = true; //marca que el limite opcional ya se recibio
            i++; //salta el argumento usado como cantidad de oleadas
        }
        else if (opcion == "--script") { //reconoce la ruta opcional de acciones
            if (config.tieneScript) { //usa el mismo indicador que luego consultara la aplicacion
                return false; //rechaza una segunda opcion de script
            }

            if (!hayValorDespues(i, argc, argv)) { //comprueba que venga una ruta no vacia y que no parezca otra opcion
                return false; //rechaza el script sin un argumento utilizable
            }

            config.rutaScript = argv[i + 1]; //guarda la ruta tal como llego sin abrir el archivo
            config.tieneScript = true; //registra que la aplicacion debera cargar ese script

            i++; //salta la ruta que acaba de consumir
        }
        else { //atiende una opcion que no pertenece al modo por lotes
            return false; //rechaza el comando en lugar de ignorar el argumento
        }
    }

    if (!tieneJugadores) { //comprueba si se omitio la lista obligatoria
        return false; //rechaza lotes sin participantes
    }

    if (!tieneSemilla) { //comprueba si se omitio la semilla obligatoria
        return false; //rechaza lotes sin una semilla indicada por el usuario
    }

    return true; //confirma que las opciones de lotes estan completas
}

//reinicia la configuracion y selecciona el modo segun los argumentos recibidos
//sin opciones elige la interfaz grafica y ante un error devuelve falso con el modo invalido
bool parsearArgumentos(int argc, char* argv[], ConfiguracionPrograma& config) {
    inicializarConfiguracionPrograma(config); //descarta todos los valores de una lectura anterior

    if (argc <= 0 || argv == nullptr) { //comprueba que exista la lista de argumentos del programa
        return false; //rechaza una llamada sin argumentos utilizables
    }

    if (argc == 1) { //detecta que solo se recibio el nombre del ejecutable
        config.modo = MODO_GRAFICO; //selecciona la interfaz grafica sin opciones adicionales
        return true; //acepta la forma mas simple de iniciar el programa
    }

    string modo = argv[1]; //consulta la primera opcion despues del nombre del programa

    if (modo == "--lotes") { //selecciona la simulacion sin interfaz grafica
        if (!parsearModoLotes(argc, argv, config)) { //revisa las opciones obligatorias y opcionales de la partida
            config.modo = MODO_INVALIDO; //anula el modo elegido si alguna opcion fallo
            return false; //avisa que no se debe ejecutar esta configuracion
        }

        return true; //acepta el comando de lotes ya validado
    }

    if (modo == "--estadisticas") { //selecciona la consulta de resultados historicos
        if (argc != 3) { //exige una sola ruta despues del modo
            return false; //rechaza la ruta ausente o los argumentos adicionales
        }

        config.rutaHistorial = argv[2]; //guarda la ruta que recibira la consulta

        if (config.rutaHistorial.empty()) { //comprueba si la ruta tiene contenido
            return false; //rechaza un argumento vacio
        }

        config.modo = MODO_ESTADISTICAS; //habilita la consulta despues de validar sus argumentos
        return true; //confirma que la configuracion esta lista
    }

    if (modo == "--ranking") { //selecciona la consulta de clasificacion historica
        if (argc != 3) { //exige exactamente una ruta para el historial
            return false; //rechaza comandos incompletos o con argumentos sobrantes
        }

        config.rutaHistorial = argv[2]; //conserva el nombre del archivo que se va a consultar

        if (config.rutaHistorial.empty()) { //comprueba que la ruta no este vacia
            return false; //rechaza la consulta sin nombre de archivo
        }

        config.modo = MODO_RANKING; //habilita la consulta del ranking
        return true; //confirma que el comando tiene la forma esperada
    }

    if (modo == "--pruebas") { //selecciona la ejecucion de las pruebas del proyecto
        if (argc != 2) { //comprueba que el modo venga sin otras opciones
            return false; //rechaza cualquier argumento adicional
        }

        config.modo = MODO_PRUEBAS; //indica a la aplicacion que ejecute las pruebas
        return true; //acepta el comando de pruebas
    }

    return false; //rechaza un modo que no pertenece a los comandos disponibles
}

//muestra las formas de iniciar el programa y deja entre corchetes las opciones de lotes que se pueden omitir
void imprimirUso(ostream& salida) {
    salida << "Uso:" << '\n'; //abre el mensaje de ayuda
    salida << '\n'; //separa el titulo de los ejemplos de comandos

    salida << "faseorbital" << '\n'; //muestra como iniciar la interfaz grafica
    salida << "faseorbital --lotes --jugadores A,B,BOT1 " << "--semilla 42 [--oleadas 10] [--script partida.txt]" << '\n'; //muestra los datos obligatorios y opcionales para lotes

    salida << "faseorbital --estadisticas historial.csv" << '\n'; //muestra como consultar las estadisticas del historial
    salida << "faseorbital --ranking historial.csv" << '\n'; //muestra como consultar la clasificacion de jugadores
    salida << "faseorbital --pruebas" << '\n'; //muestra como ejecutar las pruebas del proyecto
}
