#include "Persistencia.h"
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdint>
#include <cstdio>

using namespace std;

//mantiene las columnas oficiales del historial principal
static const char* ENCABEZADO_HISTORIAL = "id_partida,semilla,modo,jugador,es_bot,posicion,puntaje,oleada,disparos,correctos,erroneos,combo_max,pulsos,dano_total,ticks,bajas_dron,bajas_girador,bajas_divisor,bajas_mini,bajas_tanque,bajas_camaleon";

//guarda las columnas adicionales sin cambiar el formato oficial
static const char* ENCABEZADO_DETALLE_HISTORIAL = "id_partida,jugador,posicion,correctos_c,erroneos_c,correctos_m,erroneos_m,correctos_a,erroneos_a,dano_dron,dano_girador,dano_divisor,dano_mini,dano_tanque,dano_camaleon";

//devuelve el encabezado obligatorio del archivo principal
const char* encabezadoHistorialCSV() {
    return ENCABEZADO_HISTORIAL; //devuelve el texto sin crear una copia
}

//forma el nombre del archivo de detalle a partir del historial principal
string rutaDetalleHistorialCSV(const string& rutaHistorial) {
    size_t separador = rutaHistorial.find_last_of("/\\"); //busca el ultimo separador de carpetas
    size_t punto = rutaHistorial.find_last_of('.'); //busca el ultimo punto del nombre

    if (punto == string::npos || (separador != string::npos && punto < separador)) { //comprueba si el archivo no tiene extension
        return rutaHistorial + "_detalle.csv"; //agrega el nombre de detalle al final
    }

    return rutaHistorial.substr(0, punto) + "_detalle" + rutaHistorial.substr(punto); //inserta detalle antes de la extension
}

//comprueba si un archivo esta vacio o no se puede abrir
static bool archivoVacioONoExiste(const string& ruta) {
    ifstream entrada(ruta, ios::binary | ios::ate); //intenta abrir el archivo al final para conocer su tamano

    if (!entrada.is_open()) { //comprueba si no pudo abrirlo
        return true; //permite crear un archivo nuevo
    }

    return entrada.tellg() == 0; //comprueba si el archivo tiene cero bytes
}

//evita guardar textos que rompan una fila del csv
static bool textoValidoParaCSV(const string& texto) {
    return texto.find('\n') == string::npos && texto.find('\r') == string::npos; //rechaza saltos de linea dentro del campo
}

//agrega comillas cuando un campo contiene comas o comillas
static string escaparCampoCSV(const string& texto) {
    bool necesitaComillas = texto.find(',') != string::npos || texto.find('"') != string::npos; //busca caracteres especiales del formato

    if (!necesitaComillas) { //comprueba si el texto se puede guardar directamente
        return texto; //devuelve el mismo contenido
    }

    string resultado = "\""; //abre el campo entre comillas

    for (char caracter : texto) { //revisa todos los caracteres del texto
        if (caracter == '"') { //comprueba si encontro una comilla
            resultado += "\"\""; //duplica la comilla para que sea valida en csv
        }
        else {
            resultado += caracter; //copia el caracter normalmente
        }
    }

    resultado += "\""; //cierra el campo entre comillas

    return resultado; //devuelve el campo listo para escribir
}

//escribe una fila con las columnas oficiales del historial
static bool escribirRegistroCSV(ostream& salida, const RegistroPartida& registro) {
    if (!textoValidoParaCSV(registro.modo) || !textoValidoParaCSV(registro.jugador)) { //comprueba los campos de texto
        return false; //evita escribir una fila con saltos de linea
    }

    salida << registro.idPartida << ',' //escribe el identificador
        << registro.semilla << ',' //escribe la semilla
        << escaparCampoCSV(registro.modo) << ',' //escribe el modo
        << escaparCampoCSV(registro.jugador) << ',' //escribe el alias
        << (registro.esBot ? 1 : 0) << ',' //escribe el indicador de bot
        << registro.posicion << ',' //escribe la posicion
        << registro.puntaje << ',' //escribe el puntaje
        << registro.oleada << ',' //escribe la oleada
        << registro.disparos << ',' //escribe los disparos
        << registro.correctos << ',' //escribe los impactos correctos
        << registro.erroneos << ',' //escribe los impactos erroneos
        << registro.comboMax << ',' //escribe el combo maximo
        << registro.pulsos << ',' //escribe los pulsos
        << registro.danoTotal << ',' //escribe el dano total
        << registro.ticks << ',' //escribe los ticks
        << registro.bajasDron << ',' //escribe las bajas de drones
        << registro.bajasGirador << ',' //escribe las bajas de giradores
        << registro.bajasDivisor << ',' //escribe las bajas de divisores
        << registro.bajasMini << ',' //escribe las bajas de minis
        << registro.bajasTanque << ',' //escribe las bajas de tanques
        << registro.bajasCamaleon << '\n'; //termina la fila con las bajas de camaleones

    return salida.good(); //comprueba que la escritura no produjo un error
}

//agrega una fila individual al archivo principal
bool agregarRegistroCSV(const string& ruta, const RegistroPartida& registro) {
    bool escribirEncabezado = archivoVacioONoExiste(ruta); //comprueba si hace falta crear el encabezado

    ofstream salida(ruta, ios::app); //abre el archivo para agregar sin borrar lo anterior

    if (!salida.is_open()) { //comprueba si pudo abrir el archivo
        return false; //informa que no se puede escribir
    }

    if (escribirEncabezado) { //comprueba si el archivo es nuevo o esta vacio
        salida << ENCABEZADO_HISTORIAL << '\n'; //escribe el encabezado obligatorio
    }

    return escribirRegistroCSV(salida, registro); //agrega el registro y devuelve el resultado
}

//agrega una lista de registros al historial principal
bool agregarHistorialCSV(const string& ruta, const HistorialPartidas& historial) {
    bool escribirEncabezado = archivoVacioONoExiste(ruta); //comprueba si necesita encabezado

    ofstream salida(ruta, ios::app); //abre el archivo para agregar registros

    if (!salida.is_open()) { //comprueba la apertura
        return false; //informa que no se puede guardar
    }

    if (escribirEncabezado) { //comprueba si el archivo esta vacio
        salida << ENCABEZADO_HISTORIAL << '\n'; //escribe el encabezado
    }

    const NodoRegistroPartida* actual = historial.cabeza; //empieza desde el primer registro de la lista

    while (actual != nullptr) { //recorre todos los registros
        if (!escribirRegistroCSV(salida, actual->dato)) { //intenta escribir el registro actual
            return false; //detiene la operacion si ocurre un error
        }

        actual = actual->sig; //avanza al siguiente registro
    }

    return salida.good(); //comprueba el estado de la escritura
}

//agrega el detalle de las partidas sin modificar el archivo principal
bool agregarDetalleHistorialCSV(const string& rutaHistorial, const HistorialPartidas& historial) {
    bool hayDetalle = false; //indica si existe algun registro con informacion adicional

    const NodoRegistroPartida* actual = historial.cabeza; //empieza a revisar la lista

    while (actual != nullptr) { //recorre los registros buscando informacion adicional
        if (actual->dato.detalleDisponible) { //comprueba si el registro tiene detalle
            hayDetalle = true; //marca que existe informacion para escribir
            break; //deja de buscar porque ya encontro un registro
        }

        actual = actual->sig; //continua con el siguiente registro
    }

    if (!hayDetalle) { //comprueba si ningun registro tiene detalle
        return true; //termina sin crear un archivo innecesario
    }

    string rutaDetalle = rutaDetalleHistorialCSV(rutaHistorial); //obtiene la ruta del archivo auxiliar
    bool escribirEncabezado = archivoVacioONoExiste(rutaDetalle); //comprueba si necesita encabezado

    ofstream salida(rutaDetalle, ios::app); //abre el archivo auxiliar para agregar filas

    if (!salida.is_open()) { //comprueba si pudo abrir el detalle
        return false; //informa que no se pudo guardar
    }

    if (escribirEncabezado) { //comprueba si el archivo estaba vacio
        salida << ENCABEZADO_DETALLE_HISTORIAL << '\n'; //escribe las columnas del detalle
    }

    actual = historial.cabeza; //vuelve al inicio para escribir los registros

    while (actual != nullptr) { //recorre la lista completa
        const RegistroPartida& registro = actual->dato; //consulta el registro actual

        if (registro.detalleDisponible) { //escribe solamente los registros que tienen detalle
            if (!textoValidoParaCSV(registro.jugador)) { //comprueba que el alias sea valido
                return false; //detiene la escritura si el alias rompe el formato
            }

            salida << registro.idPartida << ',' //escribe el identificador
                << escaparCampoCSV(registro.jugador) << ',' //escribe el alias
                << registro.posicion << ',' //escribe la posicion
                << registro.correctosC << ',' //escribe los aciertos en cian
                << registro.erroneosC << ',' //escribe los errores en cian
                << registro.correctosM << ',' //escribe los aciertos en magenta
                << registro.erroneosM << ',' //escribe los errores en magenta
                << registro.correctosA << ',' //escribe los aciertos en ambar
                << registro.erroneosA << ',' //escribe los errores en ambar
                << registro.danoDron << ',' //escribe el dano de drones
                << registro.danoGirador << ',' //escribe el dano de giradores
                << registro.danoDivisor << ',' //escribe el dano de divisores
                << registro.danoMini << ',' //escribe el dano de minis
                << registro.danoTanque << ',' //escribe el dano de tanques
                << registro.danoCamaleon << '\n'; //termina la fila con el dano de camaleones
        }

        actual = actual->sig; //avanza al siguiente registro
    }

    return salida.good(); //comprueba que los datos se escribieron correctamente
}

//separa los campos del csv respetando las comillas
static bool separarCamposCSV(const string& linea, string campos[21]) {
    int cantidad = 0; //cuenta las columnas encontradas
    string campo; //acumula el contenido de la columna actual
    bool enComillas = false; //indica si se esta leyendo un campo entre comillas
    bool comillasCerradas = false; //indica si acaba de cerrarse un campo

    for (size_t i = 0; i < linea.size(); i++) { //recorre la linea caracter por caracter
        char caracter = linea[i]; //obtiene el caracter actual

        if (enComillas) { //procesa los caracteres dentro de un campo entre comillas
            if (caracter == '"') { //comprueba si encontro una comilla
                if (i + 1 < linea.size() && linea[i + 1] == '"') { //detecta las comillas duplicadas
                    campo += '"'; //guarda una sola comilla
                    i++; //salta la segunda comilla
                }
                else {
                    enComillas = false; //marca el final del texto entre comillas
                    comillasCerradas = true; //indica que espera una coma o el final de la linea
                }
            }
            else {
                campo += caracter; //agrega el caracter al campo
            }

            continue; //continua leyendo el mismo campo
        }

        if (comillasCerradas) { //comprueba si acaba de finalizar un texto entre comillas
            if (caracter != ',') { //despues de las comillas solo puede aparecer una coma
                return false; //rechaza la fila si tiene otro caracter
            }

            if (cantidad >= 21) { //evita guardar mas columnas de las permitidas
                return false; //informa que hay demasiados campos
            }

            campos[cantidad] = campo; //guarda la columna que acaba de terminar
            cantidad++; //aumenta la cantidad de columnas
            campo.clear(); //limpia el texto para la siguiente columna
            comillasCerradas = false; //permite leer otro campo normalmente
            continue; //pasa al siguiente caracter
        }

        if (caracter == ',') { //comprueba si encontro un separador
            if (cantidad >= 21) { //evita salir del arreglo de columnas
                return false; //rechaza las filas con demasiadas columnas
            }

            campos[cantidad] = campo; //guarda el campo actual
            cantidad++; //cuenta la columna
            campo.clear(); //prepara la siguiente columna
        }
        else if (caracter == '"') { //comprueba si comienza un texto entre comillas
            if (!campo.empty()) { //las comillas solo pueden aparecer al inicio del campo
                return false; //rechaza las comillas mal colocadas
            }

            enComillas = true; //activa la lectura entre comillas
        }
        else {
            campo += caracter; //agrega el caracter a la columna actual
        }
    }

    if (enComillas) { //comprueba si la linea termino con comillas sin cerrar
        return false; //rechaza la fila incompleta
    }

    if (cantidad >= 21) { //comprueba que quede espacio para la ultima columna
        return false; //rechaza los campos adicionales
    }

    campos[cantidad] = campo; //guarda el ultimo campo de la linea
    cantidad++; //cuenta la ultima columna

    return cantidad == 21; //acepta solamente las filas con las columnas obligatorias
}

//convierte un texto a entero comprobando su rango
static bool convertirEntero(const string& texto, int& valor) {
    if (texto.empty()) { //comprueba si el campo esta vacio
        return false; //no permite convertir un campo sin contenido
    }

    istringstream entrada(texto); //prepara la lectura del numero desde la cadena
    long long temporal; //usa un entero mas grande para comprobar el rango
    char extra; //permite detectar caracteres despues del numero

    if (!(entrada >> temporal)) { //intenta convertir el texto en un numero
        return false; //rechaza el campo si no contiene un entero valido
    }

    if (entrada >> extra) { //comprueba si quedaron caracteres adicionales
        return false; //rechaza los datos que no son solo un entero
    }

    if (temporal < numeric_limits<int>::min() || temporal > numeric_limits<int>::max()) { //comprueba que el valor pueda guardarse en un int
        return false; //rechaza los valores fuera de rango
    }

    valor = static_cast<int>(temporal); //guarda el entero convertido
    return true; //confirma que la conversion funciono
}

//convierte la semilla y evita valores negativos o demasiado grandes
static bool convertirSemilla(const string& texto, uint32_t& valor) {
    if (texto.empty()) { //comprueba si no hay texto
        return false; //rechaza las semillas vacias
    }

    size_t primero = texto.find_first_not_of(" \t"); //busca el primer caracter que no sea espacio

    if (primero == string::npos || texto[primero] == '-') { //comprueba si el texto esta vacio o comienza con menos
        return false; //rechaza semillas negativas
    }

    istringstream entrada(texto); //prepara la conversion desde texto
    unsigned long long temporal; //permite comprobar un rango mayor al de la semilla
    char extra; //permite detectar caracteres sobrantes

    if (!(entrada >> temporal)) { //intenta leer el numero
        return false; //rechaza una conversion incorrecta
    }

    if (entrada >> extra) { //comprueba si hay caracteres adicionales
        return false; //rechaza un campo que no sea numerico
    }

    if (temporal > numeric_limits<uint32_t>::max()) { //comprueba el limite de la semilla
        return false; //rechaza una semilla demasiado grande
    }

    valor = static_cast<uint32_t>(temporal); //guarda la semilla convertida
    return true; //confirma que la conversion funciono
}

//inicializa el detalle de un registro cargado desde el archivo principal
static void limpiarDetalleRegistro(RegistroPartida& registro) {
    registro.detalleDisponible = false; //indica que aun no se ha cargado informacion adicional

    registro.correctosC = 0; //inicializa los aciertos de cian
    registro.erroneosC = 0; //inicializa los errores de cian
    registro.correctosM = 0; //inicializa los aciertos de magenta
    registro.erroneosM = 0; //inicializa los errores de magenta
    registro.correctosA = 0; //inicializa los aciertos de ambar
    registro.erroneosA = 0; //inicializa los errores de ambar

    registro.danoDron = 0; //inicializa el dano de drones
    registro.danoGirador = 0; //inicializa el dano de giradores
    registro.danoDivisor = 0; //inicializa el dano de divisores
    registro.danoMini = 0; //inicializa el dano de minis
    registro.danoTanque = 0; //inicializa el dano de tanques
    registro.danoCamaleon = 0; //inicializa el dano de camaleones
}

//convierte las columnas oficiales a un registro de partida
static bool convertirRegistroCSV(const string campos[21], RegistroPartida& registro) {
    int esBot; //recibe temporalmente el indicador numerico de bot

    if (!convertirEntero(campos[0], registro.idPartida)) return false; //lee el identificador
    if (!convertirSemilla(campos[1], registro.semilla)) return false; //lee la semilla

    registro.modo = campos[2]; //guarda el modo de la partida
    registro.jugador = campos[3]; //guarda el alias del participante

    if (!convertirEntero(campos[4], esBot)) return false; //lee el indicador de bot
    if (esBot != 0 && esBot != 1) return false; //permite solamente cero o uno

    registro.esBot = esBot == 1; //convierte el indicador a verdadero o falso

    if (!convertirEntero(campos[5], registro.posicion)) return false; //lee la posicion final
    if (!convertirEntero(campos[6], registro.puntaje)) return false; //lee el puntaje
    if (!convertirEntero(campos[7], registro.oleada)) return false; //lee la oleada alcanzada
    if (!convertirEntero(campos[8], registro.disparos)) return false; //lee los disparos
    if (!convertirEntero(campos[9], registro.correctos)) return false; //lee los impactos correctos
    if (!convertirEntero(campos[10], registro.erroneos)) return false; //lee los impactos incorrectos
    if (!convertirEntero(campos[11], registro.comboMax)) return false; //lee el combo maximo
    if (!convertirEntero(campos[12], registro.pulsos)) return false; //lee los pulsos usados
    if (!convertirEntero(campos[13], registro.danoTotal)) return false; //lee el dano total
    if (!convertirEntero(campos[14], registro.ticks)) return false; //lee los ticks sobrevividos
    if (!convertirEntero(campos[15], registro.bajasDron)) return false; //lee las bajas de drones
    if (!convertirEntero(campos[16], registro.bajasGirador)) return false; //lee las bajas de giradores
    if (!convertirEntero(campos[17], registro.bajasDivisor)) return false; //lee las bajas de divisores
    if (!convertirEntero(campos[18], registro.bajasMini)) return false; //lee las bajas de minis
    if (!convertirEntero(campos[19], registro.bajasTanque)) return false; //lee las bajas de tanques
    if (!convertirEntero(campos[20], registro.bajasCamaleon)) return false; //lee las bajas de camaleones

    limpiarDetalleRegistro(registro); //deja el detalle pendiente de cargar
    return true; //confirma que los campos fueron convertidos
}

//separa una linea y convierte sus columnas en un registro
static bool convertirLineaCSV(const string& linea, RegistroPartida& registro) {
    string campos[21]; //guarda temporalmente las columnas de la fila

    if (!separarCamposCSV(linea, campos)) { //comprueba que tenga las columnas correctas
        return false; //rechaza la fila mal formada
    }

    return convertirRegistroCSV(campos, registro); //convierte los campos a sus tipos correspondientes
}

//carga el historial principal e informa las filas invalidas
bool cargarHistorialCSV(const string& ruta, HistorialPartidas& historial, ostream& errores) {
    ifstream entrada(ruta); //abre el historial principal para lectura

    if (!entrada.is_open()) { //comprueba si el archivo pudo abrirse
        errores << "No se pudo abrir el archivo: " << ruta << '\n'; //informa la ruta que produjo el error
        return false; //termina porque no puede cargar el historial
    }

    string linea; //guarda cada linea leida del archivo

    if (!getline(entrada, linea)) { //intenta leer el encabezado
        errores << "El archivo CSV esta vacio o no contiene encabezado." << '\n'; //informa el problema encontrado
        return false; //no continua sin el encabezado
    }

    if (!linea.empty() && linea.back() == '\r') { //comprueba si el encabezado termina con retorno de carro
        linea.pop_back(); //elimina el caracter adicional
    }

    if (linea != ENCABEZADO_HISTORIAL) { //comprueba que el encabezado sea el oficial
        errores << "Encabezado CSV invalido." << '\n'; //informa que las columnas no corresponden
        return false; //evita cargar un archivo con formato incorrecto
    }

    HistorialPartidas temporal; //crea una lista para cargar los registros nuevos
    inicializarHistorial(temporal); //inicializa la lista vacia

    int numeroLinea = 1; //comienza contando la linea del encabezado

    while (getline(entrada, linea)) { //lee cada fila del archivo
        numeroLinea++; //actualiza la posicion de la linea

        if (!linea.empty() && linea.back() == '\r') { //comprueba si la fila termina con retorno de carro
            linea.pop_back(); //quita ese caracter
        }

        RegistroPartida registro; //crea el registro que recibira los datos

        if (!convertirLineaCSV(linea, registro)) { //intenta convertir la fila
            errores << "Linea " << numeroLinea << " invalida; se omite." << '\n'; //informa donde encontro el error
            continue; //salta la fila y continua con las demas
        }

        agregarRegistroFinal(temporal, registro); //agrega el registro valido a la lista temporal
    }

    destruirHistorial(historial); //libera los datos anteriores del historial

    historial.cabeza = temporal.cabeza; //transfiere el primer nodo al historial definitivo
    historial.cola = temporal.cola; //transfiere el ultimo nodo
    historial.tam = temporal.tam; //transfiere la cantidad de registros

    temporal.cabeza = nullptr; //evita conservar el primer nodo en ambas listas
    temporal.cola = nullptr; //evita conservar el ultimo nodo en ambas listas
    temporal.tam = 0; //deja la lista temporal sin registros

    return true; //confirma que el historial se cargo
}

//separa las columnas del archivo de detalle respetando comillas
static bool separarCamposDetalleCSV(const string& linea, string campos[15]) {
    int cantidad = 0; //cuenta las columnas encontradas
    string campo; //guarda el texto del campo actual
    bool enComillas = false; //indica si esta dentro de un texto entre comillas
    bool comillasCerradas = false; //indica si se acaba de cerrar un campo

    for (size_t i = 0; i < linea.size(); i++) { //recorre todos los caracteres de la linea
        char caracter = linea[i]; //consulta el caracter actual

        if (enComillas) { //comprueba si esta leyendo un campo entre comillas
            if (caracter == '"') { //comprueba si encontro una comilla
                if (i + 1 < linea.size() && linea[i + 1] == '"') { //detecta dos comillas seguidas
                    campo += '"'; //agrega una sola comilla al campo
                    i++; //salta la segunda comilla
                }
                else {
                    enComillas = false; //indica que termino el texto entre comillas
                    comillasCerradas = true; //espera la coma o el final de la linea
                }
            }
            else {
                campo += caracter; //agrega el caracter normalmente
            }

            continue; //continua con el siguiente caracter
        }

        if (comillasCerradas) { //comprueba si ya cerro un campo entre comillas
            if (caracter != ',') { //solamente puede venir una coma despues de cerrar comillas
                return false; //rechaza los caracteres adicionales
            }

            if (cantidad >= 15) { //comprueba el limite de columnas
                return false; //rechaza una fila con demasiadas columnas
            }

            campos[cantidad] = campo; //guarda la columna completada
            cantidad++; //suma una columna
            campo.clear(); //limpia el texto para el siguiente campo
            comillasCerradas = false; //vuelve a la lectura normal
            continue; //pasa al siguiente caracter
        }

        if (caracter == ',') { //comprueba si encontro el separador de columnas
            if (cantidad >= 15) { //evita acceder fuera del arreglo
                return false; //rechaza la fila con columnas adicionales
            }

            campos[cantidad] = campo; //guarda el campo actual
            cantidad++; //aumenta la cantidad de columnas
            campo.clear(); //prepara el siguiente campo
        }
        else if (caracter == '"') { //comprueba si comienzan las comillas de un campo
            if (!campo.empty()) { //las comillas deben comenzar al inicio del campo
                return false; //rechaza las comillas fuera de posicion
            }

            enComillas = true; //activa la lectura entre comillas
        }
        else {
            campo += caracter; //agrega el caracter al campo actual
        }
    }

    if (enComillas) { //comprueba si quedaron comillas sin cerrar
        return false; //rechaza una fila incompleta
    }

    if (cantidad >= 15) { //comprueba que haya espacio para el ultimo campo
        return false; //rechaza las columnas adicionales
    }

    campos[cantidad] = campo; //guarda el ultimo campo
    cantidad++; //cuenta la ultima columna

    return cantidad == 15; //acepta solamente las filas con quince columnas
}

//busca el registro que corresponde a una fila del archivo auxiliar
static NodoRegistroPartida* buscarRegistroParaDetalle(HistorialPartidas& historial, int idPartida, const string& jugador, int posicion) {
    NodoRegistroPartida* actual = historial.cabeza; //comienza a buscar desde el primer registro

    while (actual != nullptr) { //recorre el historial hasta encontrar una coincidencia
        RegistroPartida& registro = actual->dato; //consulta el registro actual

        if (registro.idPartida == idPartida && registro.jugador == jugador && registro.posicion == posicion && !registro.detalleDisponible) { //comprueba la partida, el jugador y la posicion
            return actual; //devuelve el nodo que necesita recibir el detalle
        }

        actual = actual->sig; //continua con el siguiente registro
    }

    return nullptr; //informa que no encontro un registro correspondiente
}

//carga el archivo auxiliar y completa los datos del historial principal
bool cargarDetalleHistorialCSV(const string& rutaHistorial, HistorialPartidas& historial, ostream& errores) {
    string rutaDetalle = rutaDetalleHistorialCSV(rutaHistorial); //obtiene el nombre del archivo auxiliar

    ifstream entrada(rutaDetalle); //intenta abrir el archivo de detalle

    if (!entrada.is_open()) { //comprueba si el archivo auxiliar no existe o no se puede abrir
        return true; //permite historiales antiguos que todavia no tienen detalle
    }

    string linea; //guarda cada linea leida

    if (!getline(entrada, linea)) { //intenta leer el encabezado auxiliar
        errores << "El archivo de detalle esta vacio." << '\n'; //informa que no tiene encabezado
        return false; //rechaza el archivo vacio
    }

    if (!linea.empty() && linea.back() == '\r') { //comprueba el retorno de carro
        linea.pop_back(); //lo elimina del encabezado
    }

    if (linea != ENCABEZADO_DETALLE_HISTORIAL) { //comprueba que las columnas sean correctas
        errores << "Encabezado de historial_detalle.csv invalido." << '\n'; //informa el error de formato
        return false; //rechaza el detalle con encabezado distinto
    }

    int numeroLinea = 1; //comienza contando la linea del encabezado

    while (getline(entrada, linea)) { //recorre las filas del detalle
        numeroLinea++; //actualiza el numero de linea

        if (!linea.empty() && linea.back() == '\r') { //comprueba si tiene retorno de carro
            linea.pop_back(); //lo elimina
        }

        string campos[15]; //guarda las columnas de detalle

        if (!separarCamposDetalleCSV(linea, campos)) { //comprueba el formato de la fila
            errores << "Linea " << numeroLinea << " invalida en historial_detalle.csv; se omite." << '\n'; //informa donde esta el error
            continue; //omite la fila y continua
        }

        int idPartida; //recibe el identificador de la partida
        int posicion; //recibe la posicion final
        int correctosC; //recibe los aciertos en cian
        int erroneosC; //recibe los errores en cian
        int correctosM; //recibe los aciertos en magenta
        int erroneosM; //recibe los errores en magenta
        int correctosA; //recibe los aciertos en ambar
        int erroneosA; //recibe los errores en ambar
        int danoDron; //recibe el dano de drones
        int danoGirador; //recibe el dano de giradores
        int danoDivisor; //recibe el dano de divisores
        int danoMini; //recibe el dano de minis
        int danoTanque; //recibe el dano de tanques
        int danoCamaleon; //recibe el dano de camaleones

        bool valido = //comprueba que todos los campos numericos se puedan convertir
            convertirEntero(campos[0], idPartida) &&
            convertirEntero(campos[2], posicion) &&
            convertirEntero(campos[3], correctosC) &&
            convertirEntero(campos[4], erroneosC) &&
            convertirEntero(campos[5], correctosM) &&
            convertirEntero(campos[6], erroneosM) &&
            convertirEntero(campos[7], correctosA) &&
            convertirEntero(campos[8], erroneosA) &&
            convertirEntero(campos[9], danoDron) &&
            convertirEntero(campos[10], danoGirador) &&
            convertirEntero(campos[11], danoDivisor) &&
            convertirEntero(campos[12], danoMini) &&
            convertirEntero(campos[13], danoTanque) &&
            convertirEntero(campos[14], danoCamaleon);

        if (!valido) { //comprueba si alguna conversion fallo
            errores << "Linea " << numeroLinea << " invalida en historial_detalle.csv; se omite." << '\n'; //informa el numero de linea
            continue; //omite los datos incorrectos
        }

        NodoRegistroPartida* nodo = buscarRegistroParaDetalle(historial, idPartida, campos[1], posicion); //busca el registro principal correspondiente

        if (nodo == nullptr) { //comprueba si no existe una coincidencia
            errores << "Linea " << numeroLinea << " de historial_detalle.csv no corresponde a un registro principal; se omite." << '\n'; //informa el detalle sin registro
            continue; //omite el detalle que no se puede relacionar
        }

        RegistroPartida& registro = nodo->dato; //obtiene el registro que recibira las metricas

        registro.detalleDisponible = true; //marca que el detalle ya fue cargado

        registro.correctosC = correctosC; //guarda los aciertos en cian
        registro.erroneosC = erroneosC; //guarda los errores en cian
        registro.correctosM = correctosM; //guarda los aciertos en magenta
        registro.erroneosM = erroneosM; //guarda los errores en magenta
        registro.correctosA = correctosA; //guarda los aciertos en ambar
        registro.erroneosA = erroneosA; //guarda los errores en ambar

        registro.danoDron = danoDron; //guarda el dano de drones
        registro.danoGirador = danoGirador; //guarda el dano de giradores
        registro.danoDivisor = danoDivisor; //guarda el dano de divisores
        registro.danoMini = danoMini; //guarda el dano de minis
        registro.danoTanque = danoTanque; //guarda el dano de tanques
        registro.danoCamaleon = danoCamaleon; //guarda el dano de camaleones
    }

    return true; //confirma que termino de procesar el archivo auxiliar
}

//comprueba si existe un archivo temporal o de respaldo
static bool existeArchivoGuardado(const string& ruta) {
    ifstream archivo(ruta, ios::binary); //intenta abrir el archivo indicado
    return archivo.is_open(); //indica si pudo encontrarlo y abrirlo
}

//escribe una fila con las metricas del archivo auxiliar
static bool escribirRegistroDetalleCSV(ostream& salida, const RegistroPartida& registro) {
    if (!textoValidoParaCSV(registro.jugador)) { //comprueba que el alias no contenga saltos de linea
        return false; //rechaza los datos que alteran el formato
    }

    salida << registro.idPartida << ',' << escaparCampoCSV(registro.jugador) << ',' << registro.posicion << ','; //escribe los datos que relacionan el detalle con el principal
    salida << registro.correctosC << ',' << registro.erroneosC << ',' << registro.correctosM << ',' << registro.erroneosM << ','; //escribe los impactos de cian y magenta
    salida << registro.correctosA << ',' << registro.erroneosA << ',' << registro.danoDron << ',' << registro.danoGirador << ','; //escribe los impactos de ambar y los primeros tipos de dano
    salida << registro.danoDivisor << ',' << registro.danoMini << ',' << registro.danoTanque << ',' << registro.danoCamaleon << '\n'; //termina la fila con los otros tipos de dano

    return salida.good(); //comprueba que la escritura termino sin error
}

//prepara un archivo temporal con los datos antiguos y la partida nueva
static bool prepararArchivoTemporal(const string& rutaOriginal, const string& rutaTemporal, const char* encabezado, const HistorialPartidas& nuevosResultados, bool detalle) {
    ifstream anterior(rutaOriginal, ios::binary); //intenta leer el archivo anterior
    ofstream nuevo(rutaTemporal, ios::binary | ios::trunc); //crea el archivo temporal donde se preparara el resultado

    if (!nuevo.is_open()) { //comprueba si pudo crear el temporal
        return false; //no continua si no puede preparar el archivo
    }

    bool hayContenidoAnterior = false; //indica si hay datos antiguos que copiar
    char ultimoCaracter = '\n'; //guarda el ultimo caracter para no unir dos filas

    if (anterior.is_open()) { //comprueba si existe un archivo anterior
        string primeraLinea; //recibe su encabezado

        if (getline(anterior, primeraLinea)) { //intenta leer la primera linea
            if (!primeraLinea.empty() && primeraLinea.back() == '\r') { //comprueba si tiene retorno de carro
                primeraLinea.pop_back(); //lo elimina antes de comparar
            }

            if (primeraLinea != encabezado) { //comprueba que sea el encabezado esperado
                return false; //evita copiar datos de un archivo con formato distinto
            }

            hayContenidoAnterior = true; //indica que el archivo anterior contiene encabezado
            anterior.clear(); //limpia el estado del flujo antes de volver al inicio
            anterior.seekg(0); //vuelve al comienzo para copiar todos los bytes

            char bloque[4096]; //usa un bloque fijo para copiar sin cargar el archivo completo

            while (anterior.read(bloque, sizeof(bloque)) || anterior.gcount() > 0) { //copia todos los bloques disponibles
                streamsize cantidad = anterior.gcount(); //obtiene la cantidad de bytes leidos
                nuevo.write(bloque, cantidad); //escribe esos bytes en el temporal
                ultimoCaracter = bloque[cantidad - 1]; //recuerda el ultimo caracter copiado

                if (!nuevo.good()) { //comprueba si ocurrio un error de escritura
                    return false; //detiene la preparacion si el temporal fallo
                }
            }

            if (anterior.bad()) { //comprueba si ocurrio un error real de lectura
                return false; //evita guardar una copia incompleta
            }
        }
        else if (anterior.bad()) { //comprueba si fallo la lectura del archivo anterior
            return false; //no reemplaza un archivo que no pudo leerse
        }
    }

    if (!hayContenidoAnterior) { //comprueba si no habia informacion anterior
        nuevo << encabezado << '\n'; //escribe el encabezado del archivo nuevo
    }
    else if (ultimoCaracter != '\n') { //comprueba si el archivo anterior terminaba sin salto de linea
        nuevo << '\n'; //separa las nuevas filas de la ultima fila anterior
    }

    const NodoRegistroPartida* actual = nuevosResultados.cabeza; //comienza a recorrer los registros de la partida nueva

    while (actual != nullptr) { //escribe todos los participantes de la partida
        bool correcto = detalle ? escribirRegistroDetalleCSV(nuevo, actual->dato) : escribirRegistroCSV(nuevo, actual->dato); //selecciona el formato principal o de detalle

        if (!correcto) { //comprueba si fallo la escritura de un registro
            return false; //interrumpe la preparacion del temporal
        }

        actual = actual->sig; //continua con el siguiente participante
    }

    nuevo.flush(); //envia al archivo los datos pendientes de escritura
    bool correcto = nuevo.good(); //comprueba que el flujo siga funcionando
    nuevo.close(); //cierra el temporal para permitir su reemplazo

    return correcto && !nuevo.fail(); //confirma que tambien pudo cerrarse sin error
}

//recupera un respaldo cuando falla el reemplazo de archivos
static bool restaurarArchivoGuardado(const string& ruta, const string& respaldo, bool existiaAntes) {
    if (existeArchivoGuardado(ruta) && remove(ruta.c_str()) != 0) { //quita el archivo nuevo antes de restaurar el anterior
        return false; //informa si no pudo eliminarlo
    }

    if (!existiaAntes) { //comprueba si no habia un archivo antes del guardado
        return true; //no necesita recuperar un archivo que no existia
    }

    return rename(respaldo.c_str(), ruta.c_str()) == 0; //devuelve el respaldo a su nombre original
}

//comprueba si una partida ya esta guardada
static bool identificadorGuardadoCSV(const string& ruta, int idPartida) {
    ifstream entrada(ruta); //abre el historial principal para buscar identificadores
    string linea; //guarda la fila actual

    while (getline(entrada, linea)) { //recorre las filas del historial
        size_t coma = linea.find(','); //ubica el final del identificador
        int idActual = 0; //recibe el identificador de la fila

        if (coma != string::npos && convertirEntero(linea.substr(0, coma), idActual) && idActual == idPartida) { //compara el identificador con el de la partida nueva
            return true; //informa que la partida ya existe
        }
    }

    return false; //indica que no encontro el identificador
}

//guarda el historial principal y su detalle conservando respaldos durante el reemplazo
bool guardarPartidaCompletaCSV(const string& rutaHistorial, const HistorialPartidas& nuevosResultados, ostream& errores) {
    if (nuevosResultados.cabeza == nullptr) { //comprueba si la partida no tiene registros
        errores << "No hay resultados nuevos para guardar." << '\n'; //informa que no hay datos para escribir
        return false; //rechaza un guardado vacio
    }

    const NodoRegistroPartida* actual = nuevosResultados.cabeza; //comienza a revisar los registros nuevos
    int idPartida = actual->dato.idPartida; //obtiene el identificador comun de la partida

    while (actual != nullptr) { //valida todos los registros antes de escribir
        if (!actual->dato.detalleDisponible || actual->dato.idPartida != idPartida || !textoValidoParaCSV(actual->dato.jugador) || !textoValidoParaCSV(actual->dato.modo)) { //comprueba detalle, identificador y campos de texto
            errores << "Los resultados nuevos contienen un registro invalido." << '\n'; //informa que los datos no corresponden
            return false; //evita modificar los archivos con registros incorrectos
        }

        actual = actual->sig; //continua con el siguiente participante
    }

    if (identificadorGuardadoCSV(rutaHistorial, idPartida)) { //comprueba si ya existe esta partida en el historial
        errores << "La partida ya aparece en el historial principal." << '\n'; //informa el identificador repetido
        return false; //evita guardar la partida dos veces
    }

    string rutaDetalle = rutaDetalleHistorialCSV(rutaHistorial); //obtiene la ruta del archivo auxiliar
    string temporalPrincipal = rutaHistorial + ".nuevo.tmp"; //define el archivo temporal principal
    string temporalDetalle = rutaDetalle + ".nuevo.tmp"; //define el temporal del detalle
    string respaldoPrincipal = rutaHistorial + ".anterior.bak"; //define el respaldo principal
    string respaldoDetalle = rutaDetalle + ".anterior.bak"; //define el respaldo del detalle

    if (existeArchivoGuardado(temporalPrincipal) || existeArchivoGuardado(temporalDetalle) || existeArchivoGuardado(respaldoPrincipal) || existeArchivoGuardado(respaldoDetalle)) { //comprueba si quedaron archivos de una operacion anterior
        errores << "Hay archivos temporales o respaldos pendientes. Revise el historial antes de continuar." << '\n'; //informa que se necesita revisar los archivos
        return false; //no sobrescribe archivos que podrian servir para recuperar datos
    }

    //prepara el historial principal sin modificar el original
    if (!prepararArchivoTemporal(rutaHistorial, temporalPrincipal, ENCABEZADO_HISTORIAL, nuevosResultados, false)) { //copia lo anterior y agrega los nuevos registros
        errores << "No se pudo preparar el historial principal." << '\n'; //informa el error de preparacion
        remove(temporalPrincipal.c_str()); //quita el temporal incompleto
        return false; //no modifica el historial anterior
    }

    //prepara tambien el detalle antes de comenzar cualquier reemplazo
    if (!prepararArchivoTemporal(rutaDetalle, temporalDetalle, ENCABEZADO_DETALLE_HISTORIAL, nuevosResultados, true)) { //copia el detalle anterior y agrega los nuevos registros
        errores << "No se pudo preparar el detalle historico." << '\n'; //informa que fallo el archivo auxiliar
        remove(temporalPrincipal.c_str()); //quita el temporal principal que ya no se utilizara
        remove(temporalDetalle.c_str()); //quita el temporal auxiliar incompleto
        return false; //conserva los archivos originales
    }

    bool principalExistia = existeArchivoGuardado(rutaHistorial); //recuerda si habia historial principal
    bool detalleExistia = existeArchivoGuardado(rutaDetalle); //recuerda si habia archivo de detalle

    if (principalExistia && rename(rutaHistorial.c_str(), respaldoPrincipal.c_str()) != 0) { //intenta respaldar el archivo principal anterior
        errores << "No se pudo respaldar el historial principal." << '\n'; //informa el error de respaldo
        remove(temporalPrincipal.c_str()); //quita el temporal principal
        remove(temporalDetalle.c_str()); //quita el temporal de detalle
        return false; //no continua sin poder proteger el historial
    }

    if (detalleExistia && rename(rutaDetalle.c_str(), respaldoDetalle.c_str()) != 0) { //intenta respaldar el detalle anterior
        errores << "No se pudo respaldar el detalle historico." << '\n'; //informa que fallo el respaldo

        if (principalExistia && rename(respaldoPrincipal.c_str(), rutaHistorial.c_str()) != 0) { //intenta recuperar el principal que ya habia respaldado
            errores << "No se pudo restaurar el historial principal. Conserve el archivo .bak." << '\n'; //avisa que el respaldo necesita revision
        }

        remove(temporalPrincipal.c_str()); //quita el temporal que ya no se utilizara
        remove(temporalDetalle.c_str()); //quita el otro temporal
        return false; //termina sin confirmar el guardado
    }

    if (rename(temporalPrincipal.c_str(), rutaHistorial.c_str()) != 0) { //intenta colocar el nuevo historial principal
        errores << "No se pudo reemplazar el historial principal." << '\n'; //informa el error del reemplazo

        if (principalExistia && rename(respaldoPrincipal.c_str(), rutaHistorial.c_str()) != 0) { //intenta recuperar el historial anterior
            errores << "No se pudo restaurar el historial principal. Conserve el archivo .bak." << '\n'; //informa que debe conservarse el respaldo
        }

        if (detalleExistia && rename(respaldoDetalle.c_str(), rutaDetalle.c_str()) != 0) { //intenta recuperar el detalle anterior
            errores << "No se pudo restaurar el detalle. Conserve el archivo .bak." << '\n'; //informa que fallo la recuperacion
        }

        remove(temporalPrincipal.c_str()); //quita cualquier temporal principal restante
        remove(temporalDetalle.c_str()); //quita el temporal de detalle
        return false; //informa que el guardado no termino
    }

    if (rename(temporalDetalle.c_str(), rutaDetalle.c_str()) != 0) { //intenta colocar el nuevo archivo de detalle
        errores << "No se pudo reemplazar el detalle historico." << '\n'; //informa el error del reemplazo

        if (!restaurarArchivoGuardado(rutaHistorial, respaldoPrincipal, principalExistia)) { //intenta devolver el historial principal a su estado anterior
            errores << "No se pudo restaurar el historial principal. Conserve el archivo .bak." << '\n'; //advierte que el respaldo puede ser necesario
        }

        if (detalleExistia && rename(respaldoDetalle.c_str(), rutaDetalle.c_str()) != 0) { //intenta recuperar el detalle anterior
            errores << "No se pudo restaurar el detalle. Conserve el archivo .bak." << '\n'; //informa que fallo la recuperacion
        }

        remove(temporalDetalle.c_str()); //quita el temporal auxiliar que no pudo reemplazarse
        return false; //informa que el guardado quedo incompleto
    }

    //solo borra los respaldos cuando los dos archivos nuevos ya estan colocados
    if (principalExistia && remove(respaldoPrincipal.c_str()) != 0) { //intenta eliminar el respaldo principal
        errores << "No se pudo borrar el respaldo del historial principal." << '\n'; //informa que quedo un archivo de respaldo
    }

    if (detalleExistia && remove(respaldoDetalle.c_str()) != 0) { //intenta eliminar el respaldo auxiliar
        errores << "No se pudo borrar el respaldo del detalle historico." << '\n'; //informa que quedo un archivo de respaldo
    }

    return true; //confirma que ambos archivos se reemplazaron correctamente
}