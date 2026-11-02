#include "PruebasPersistencia.h"
#include "Persistencia.h"
#include "Estadisticas.h"
#include "Ranking.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>

using namespace std; //permite usar cadenas y flujos sin repetir el espacio de nombres

//crea un registro de prueba con todos sus campos inicializados
static RegistroPartida crearRegistroP13(int idPartida, uint32_t semilla, const string& modo, const string& jugador, bool esBot, int posicion, int puntaje, int oleada, int disparos, int correctos, int erroneos) {
    RegistroPartida registro{}; //deja los campos que no se asignen en cero

    registro.idPartida = idPartida; //identifica la partida de este registro
    registro.semilla = semilla; //guarda la semilla usada
    registro.modo = modo; //guarda el modo de ejecucion
    registro.jugador = jugador; //guarda el alias del participante
    registro.esBot = esBot; //indica si el jugador es automatico
    registro.posicion = posicion; //guarda la posicion final
    registro.puntaje = puntaje; //guarda el puntaje obtenido
    registro.oleada = oleada; //guarda la ultima oleada alcanzada
    registro.disparos = disparos; //guarda la cantidad de disparos
    registro.correctos = correctos; //guarda los impactos correctos
    registro.erroneos = erroneos; //guarda los impactos incorrectos
    registro.comboMax = 4; //usa un combo fijo para la prueba
    registro.pulsos = 1; //usa un pulso para la prueba
    registro.danoTotal = 20; //establece el dano total del registro
    registro.ticks = 500; //establece los ticks sobrevividos
    registro.bajasDron = 2; //establece las bajas de drones
    registro.bajasGirador = 1; //establece las bajas de giradores
    registro.bajasMini = 1; //establece una baja de mini

    return registro; //devuelve el registro completo
}

//compara dos decimales evitando problemas por diferencias muy pequenas
static bool doublesP13Iguales(double a, double b) {
    return fabs(a - b) < 0.000001; //acepta una diferencia pequena entre resultados
}

//compara las estadisticas antes y despues de guardar el archivo
static bool estadisticasP13Iguales(const EstadisticasJugador& a, const EstadisticasJugador& b) {
    if (a.alias != b.alias) return false; //comprueba que corresponden al mismo jugador
    if (a.partidasJugadas != b.partidasJugadas) return false; //compara las partidas registradas
    if (a.victorias != b.victorias) return false; //compara las victorias
    if (a.mejorPuntaje != b.mejorPuntaje) return false; //compara el mejor puntaje
    if (a.peorPuntaje != b.peorPuntaje) return false; //compara el peor puntaje
    if (!doublesP13Iguales(a.mediaPuntaje, b.mediaPuntaje)) return false; //compara la media
    if (!doublesP13Iguales(a.medianaPuntaje, b.medianaPuntaje)) return false; //compara la mediana
    if (!doublesP13Iguales(a.desviacionEstandar, b.desviacionEstandar)) return false; //compara la desviacion
    if (!doublesP13Iguales(a.precisionColorPromedio, b.precisionColorPromedio)) return false; //compara la precision
    if (a.mejorColorDisponible != b.mejorColorDisponible) return false; //compara la disponibilidad del color
    if (a.mejorColor != b.mejorColor) return false; //compara el mejor color
    if (a.tipoMasDanoDisponible != b.tipoMasDanoDisponible) return false; //compara la disponibilidad del dano
    if (a.tipoMasDano != b.tipoMasDano) return false; //compara el enemigo que mas dano causa
    if (!doublesP13Iguales(a.histograma.minimo, b.histograma.minimo)) return false; //compara el minimo del histograma
    if (!doublesP13Iguales(a.histograma.maximo, b.histograma.maximo)) return false; //compara el maximo del histograma
    if (!doublesP13Iguales(a.histograma.anchoIntervalo, b.histograma.anchoIntervalo)) return false; //compara el ancho de los intervalos
    if (a.histograma.total != b.histograma.total) return false; //compara la cantidad de puntajes

    for (int i = 0; i < 10; i++) { //recorre los diez intervalos del histograma
        if (a.histograma.frecuencias[i] != b.histograma.frecuencias[i]) { //comprueba la frecuencia del intervalo
            return false; //informa que los histogramas son diferentes
        }
    }

    return true; //indica que todas las estadisticas coinciden
}

//compara los jugadores y el orden que tienen dos rankings
static bool rankingsP13Iguales(const Ranking& a, const Ranking& b) {
    if (a.tam != b.tam) { //comprueba que ambos tienen la misma cantidad de jugadores
        return false; //no pueden ser iguales si tienen distinto tamano
    }

    const NodoRanking* actualA = a.cabeza; //empieza a recorrer el primer ranking
    const NodoRanking* actualB = b.cabeza; //empieza a recorrer el segundo ranking

    while (actualA != nullptr && actualB != nullptr) { //avanza mientras existan nodos en ambos
        if (actualA->dato.alias != actualB->dato.alias) { //comprueba que el jugador ocupa la misma posicion
            return false; //informa que el orden es distinto
        }

        if (actualA->dato.mejorPuntaje != actualB->dato.mejorPuntaje) { //compara los mejores puntajes
            return false; //informa que los datos son distintos
        }

        if (!doublesP13Iguales(actualA->dato.precisionColor, actualB->dato.precisionColor)) { //compara la precision usada para ordenar
            return false; //informa que las precisiones no coinciden
        }

        actualA = actualA->sig; //avanza en el primer ranking
        actualB = actualB->sig; //avanza en el segundo ranking
    }

    return actualA == nullptr && actualB == nullptr; //comprueba que ambos recorridos terminaron juntos
}

//cuenta las apariciones del encabezado oficial en un archivo
static int contarEncabezadosP13(const string& ruta) {
    ifstream entrada(ruta); //abre el archivo para leer sus lineas
    string linea; //guarda la linea que se esta leyendo
    int cantidad = 0; //cuenta las veces que aparece el encabezado

    while (getline(entrada, linea)) { //lee el archivo completo
        if (!linea.empty() && linea.back() == '\r') { //elimina el retorno de carro si existe
            linea.pop_back(); //deja la linea sin el caracter adicional
        }

        if (linea == encabezadoHistorialCSV()) { //busca una linea igual al encabezado obligatorio
            cantidad++; //suma una coincidencia
        }
    }

    return cantidad; //devuelve la cantidad encontrada
}

//lee todo el contenido para comprobar que no cambie ante un error
static string leerContenidoPruebaP13(const string& ruta) {
    ifstream entrada(ruta, ios::binary); //abre el archivo sin modificar sus bytes
    ostringstream contenido; //prepara una cadena para almacenar el contenido
    contenido << entrada.rdbuf(); //copia el contenido completo del archivo
    return contenido.str(); //devuelve el texto leido
}

//comprueba el guardado conjunto y que los archivos se conserven ante los errores probados
static bool pruebaGuardarPartidaCompletaP13() {
    const string ruta = "historial_pareja_p13.csv"; //usa un archivo separado del historial real
    const string rutaDetalle = rutaDetalleHistorialCSV(ruta); //obtiene el nombre del detalle
    const string temporalPrincipal = ruta + ".nuevo.tmp"; //guarda el nombre del temporal principal
    const string temporalDetalle = rutaDetalle + ".nuevo.tmp"; //guarda el nombre del temporal de detalle
    const string respaldoPrincipal = ruta + ".anterior.bak"; //guarda el nombre del respaldo principal
    const string respaldoDetalle = rutaDetalle + ".anterior.bak"; //guarda el nombre del respaldo de detalle

    remove(ruta.c_str()); //quita un archivo de prueba anterior
    remove(rutaDetalle.c_str()); //quita el detalle de una prueba anterior
    remove(temporalPrincipal.c_str()); //quita el temporal principal de una prueba anterior
    remove(temporalDetalle.c_str()); //quita el temporal de detalle de una prueba anterior
    remove(respaldoPrincipal.c_str()); //quita el respaldo principal de una prueba anterior
    remove(respaldoDetalle.c_str()); //quita el respaldo de detalle de una prueba anterior

    bool resultado = true; //guarda el resultado general de la prueba
    HistorialPartidas nuevosResultados; //crea el historial que se va a guardar
    inicializarHistorial(nuevosResultados); //inicializa la lista vacia

    RegistroPartida primero = crearRegistroP13(1, 42u, "lotes", "ANA", false, 1, 1000, 2, 100, 80, 20); //crea el primer participante
    RegistroPartida segundo = crearRegistroP13(1, 42u, "lotes", "BOT1", true, 2, 700, 2, 40, 20, 20); //crea el segundo participante

    primero.detalleDisponible = true; //indica que el primer registro tiene detalle
    primero.correctosC = 50; //registra los aciertos en cian
    primero.erroneosC = 10; //registra los errores en cian
    primero.correctosM = 30; //registra los aciertos en magenta
    primero.erroneosM = 10; //registra los errores en magenta
    primero.danoDron = 20; //asigna el dano recibido de los drones

    segundo.detalleDisponible = true; //indica que el segundo registro tiene detalle
    segundo.correctosA = 20; //registra los aciertos en ambar
    segundo.erroneosA = 20; //registra los errores en ambar
    segundo.danoDron = 20; //asigna el dano recibido de los drones

    agregarRegistroFinal(nuevosResultados, primero); //agrega el primer jugador a la lista
    agregarRegistroFinal(nuevosResultados, segundo); //agrega el segundo jugador a la lista

    ostringstream errores; //recibe los mensajes de error sin imprimirlos en consola

    if (!guardarPartidaCompletaCSV(ruta, nuevosResultados, errores)) { //intenta guardar los dos archivos
        resultado = false; //marca el fallo si no se completo el guardado
    }

    HistorialPartidas cargado; //crea la lista para leer los registros guardados
    inicializarHistorial(cargado); //deja la lista vacia antes de cargarla

    if (!cargarHistorialCSV(ruta, cargado, errores) || !cargarDetalleHistorialCSV(ruta, cargado, errores)) { //vuelve a cargar el historial y su detalle
        resultado = false; //marca el error si alguno de los archivos no se puede cargar
    }

    if (contarRegistros(cargado) != 2 || cargado.cabeza == nullptr || cargado.cabeza->sig == nullptr) { //comprueba que se guardaron los dos jugadores
        resultado = false; //marca el error si falta alguno
    }
    else if (!cargado.cabeza->dato.detalleDisponible || cargado.cabeza->dato.correctosC != 50 || !cargado.cabeza->sig->dato.detalleDisponible || cargado.cabeza->sig->dato.correctosA != 20) { //comprueba que se recupero el detalle
        resultado = false; //marca el error si algun dato no coincide
    }

    string principalAntes = leerContenidoPruebaP13(ruta); //conserva el contenido original del archivo principal
    string detalleAntes = leerContenidoPruebaP13(rutaDetalle); //conserva el contenido original del archivo de detalle

    //una partida ya guardada no debe aparecer otra vez
    errores.str(""); //separa los mensajes de este intento de los anteriores
    errores.clear(); //deja el flujo listo para recibir el error esperado

    if (guardarPartidaCompletaCSV(ruta, nuevosResultados, errores)) { //intenta guardar la misma partida nuevamente
        resultado = false; //marca un error si se permite duplicarla
    }

    if (errores.str() != "La partida ya aparece en el historial principal.\n") { //comprueba que el rechazo se deba al identificador repetido
        resultado = false; //marca un error si fallo por otro motivo
    }

    if (leerContenidoPruebaP13(ruta) != principalAntes || leerContenidoPruebaP13(rutaDetalle) != detalleAntes) { //comprueba que ambos archivos siguen iguales
        resultado = false; //marca el error si se modifico alguno
    }

    NodoRegistroPartida* actual = nuevosResultados.cabeza; //prepara una partida distinta para probar los siguientes errores

    while (actual != nullptr) { //actualiza el identificador de todos los participantes
        actual->dato.idPartida = 2; //evita que la validacion de duplicados detenga las pruebas siguientes
        actual = actual->sig; //continua con el siguiente registro
    }

    //un archivo temporal pendiente debe impedir que se sobrescriban los datos
    {
        ofstream bloqueado(temporalDetalle, ios::binary); //crea un temporal como si hubiera quedado de un guardado anterior
        bloqueado << "pendiente"; //escribe contenido para que el archivo exista
    }

    errores.str(""); //descarta el mensaje de partida repetida
    errores.clear(); //prepara el flujo para comprobar el rechazo del temporal

    if (guardarPartidaCompletaCSV(ruta, nuevosResultados, errores)) { //intenta guardar mientras existe el temporal
        resultado = false; //marca un error si no se detecta el temporal pendiente
    }

    if (errores.str() != "Hay archivos temporales o respaldos pendientes. Revise el historial antes de continuar.\n") { //comprueba que se alcanzo la validacion del temporal
        resultado = false; //marca un error si el guardado fallo por otra causa
    }

    if (leerContenidoPruebaP13(ruta) != principalAntes || leerContenidoPruebaP13(rutaDetalle) != detalleAntes) { //comprueba que los archivos reales no cambiaron
        resultado = false; //marca el error si fueron modificados
    }

    if (leerContenidoPruebaP13(temporalDetalle) != "pendiente") { //comprueba que el temporal anterior siga intacto
        resultado = false; //marca un error si el intento borro o reemplazo el archivo pendiente
    }

    remove(temporalDetalle.c_str()); //elimina el temporal creado para la prueba

    //un detalle con encabezado incorrecto no debe afectar el historial principal
    {
        ofstream invalido(rutaDetalle, ios::binary | ios::trunc); //abre el detalle para simular un archivo invalido
        invalido << "encabezado_invalido" << '\n'; //escribe un encabezado que no corresponde
    }

    errores.str(""); //descarta el mensaje del temporal pendiente
    errores.clear(); //prepara el flujo para comprobar el error del detalle

    if (guardarPartidaCompletaCSV(ruta, nuevosResultados, errores)) { //intenta guardar con el detalle invalido
        resultado = false; //marca un error si acepta el archivo incorrecto
    }

    if (errores.str() != "No se pudo preparar el detalle historico.\n") { //comprueba que el fallo ocurrio al preparar el archivo auxiliar
        resultado = false; //marca un error si no se alcanzo la revision del detalle
    }

    if (leerContenidoPruebaP13(ruta) != principalAntes || leerContenidoPruebaP13(rutaDetalle) != "encabezado_invalido\n") { //comprueba que se conservaron ambos archivos anteriores al intento
        resultado = false; //marca el error si se modifico el principal o el detalle invalido
    }

    //restaura los archivos de prueba para comprobar su limpieza final
    {
        ofstream restaurar(rutaDetalle, ios::binary | ios::trunc); //vuelve a abrir el archivo auxiliar
        restaurar << detalleAntes; //recupera su contenido original
    }

    const string rutasPendientes[] = { temporalPrincipal, temporalDetalle, respaldoPrincipal, respaldoDetalle }; //reune los archivos que no deben quedar despues del intento

    for (const string& rutaPendiente : rutasPendientes) { //revisa los temporales y respaldos de ambos archivos
        ifstream pendiente(rutaPendiente); //abre la ruta solo durante esta vuelta para permitir la limpieza posterior
        if (pendiente.is_open()) resultado = false; //marca un error si quedo un archivo pendiente
    }

    if (rutaDetalleHistorialCSV("datos.v1/historial") != "datos.v1/historial_detalle.csv") { //prueba una ruta con punto en el directorio
        resultado = false; //marca un error si la ruta resultante es incorrecta
    }

    destruirHistorial(cargado); //libera la lista que se cargo desde los archivos
    destruirHistorial(nuevosResultados); //libera la lista de resultados nuevos
    remove(ruta.c_str()); //elimina el archivo principal de prueba
    remove(rutaDetalle.c_str()); //elimina el detalle de prueba
    remove(temporalPrincipal.c_str()); //elimina el temporal principal
    remove(temporalDetalle.c_str()); //elimina el temporal de detalle
    remove(respaldoPrincipal.c_str()); //elimina el respaldo principal
    remove(respaldoDetalle.c_str()); //elimina el respaldo de detalle

    return resultado; //devuelve el resultado completo de la prueba
}

//comprueba la lectura del historial las estadisticas el ranking y el guardado conjunto con su detalle
bool pruebaP13() {
    const string ruta = "historial_p13_prueba.csv"; //usa un archivo independiente del historial real

    remove(ruta.c_str()); //elimina los datos de una ejecucion anterior

    RegistroPartida registro1 = crearRegistroP13(1, 42u, "lotes", "ANA", false, 1, 1000, 5, 100, 80, 20); //crea el primer registro de ana
    RegistroPartida registro2 = crearRegistroP13(1, 42u, "lotes", "BOB", false, 2, 1200, 5, 100, 90, 10); //crea el primer registro de bob
    RegistroPartida registro3 = crearRegistroP13(1, 42u, "lotes", "BOT1", true, 3, 900, 4, 100, 50, 50); //crea el registro del bot
    RegistroPartida registro4 = crearRegistroP13(2, 99u, "lotes", "ANA", false, 2, 1500, 7, 50, 45, 5); //crea el segundo registro de ana
    RegistroPartida registro5 = crearRegistroP13(2, 99u, "lotes", "BOB", false, 1, 1500, 7, 100, 80, 20); //crea el segundo registro de bob

    HistorialPartidas historialAntes; //crea la lista antes de guardar
    inicializarHistorial(historialAntes); //inicializa el historial vacio

    agregarRegistroFinal(historialAntes, registro1); //agrega el primer registro
    agregarRegistroFinal(historialAntes, registro2); //agrega el segundo registro
    agregarRegistroFinal(historialAntes, registro3); //agrega el tercer registro
    agregarRegistroFinal(historialAntes, registro4); //agrega el cuarto registro
    agregarRegistroFinal(historialAntes, registro5); //agrega el quinto registro

    EstadisticasJugador estadisticasAntes; //guarda las estadisticas calculadas en memoria

    if (!calcularEstadisticasJugador(historialAntes, "ANA", estadisticasAntes)) { //calcula las estadisticas de ana
        destruirHistorial(historialAntes); //libera el historial si falla el calculo
        return false; //finaliza la prueba indicando error
    }

    Ranking rankingAntes; //crea el ranking original
    inicializarRanking(rankingAntes); //inicializa el ranking vacio
    construirRankingDesdeHistorial(historialAntes, rankingAntes); //ordena los jugadores del historial original

    HistorialPartidas primerLote; //prepara los registros que se guardaran primero
    inicializarHistorial(primerLote); //deja la lista vacia

    agregarRegistroFinal(primerLote, registro1); //agrega el primer registro del lote
    agregarRegistroFinal(primerLote, registro2); //agrega el segundo registro del lote
    agregarRegistroFinal(primerLote, registro3); //agrega el tercer registro del lote

    bool resultado = true; //conserva el resultado general de la prueba

    if (!agregarHistorialCSV(ruta, primerLote)) { //guarda el primer lote de registros
        resultado = false; //marca el error si falla la escritura
    }

    destruirHistorial(primerLote); //libera la lista del primer lote

    //agrega filas invalidas para comprobar que se omitan al cargar
    {
        ofstream salida(ruta, ios::app); //abre el archivo sin borrar sus datos anteriores

        if (!salida.is_open()) { //comprueba si pudo abrirse
            resultado = false; //marca un error si fallo la apertura
        }
        else { //agrega las filas invalidas cuando el archivo esta abierto
            salida << "esto,no,es,una,fila,valida" << '\n'; //escribe una fila con columnas incorrectas
            salida << "99,NO_NUMERO,lotes,ERROR,0,1,100,1,1,1,0,0,0,0,10,0,0,0,0,0,0" << '\n'; //escribe una fila con semilla invalida
        }
    }

    if (!agregarRegistroCSV(ruta, registro4)) { //agrega el cuarto registro al archivo
        resultado = false; //marca el error si la escritura falla
    }

    if (!agregarRegistroCSV(ruta, registro5)) { //agrega el quinto registro al archivo
        resultado = false; //marca el error si la escritura falla
    }

    if (contarEncabezadosP13(ruta) != 1) { //comprueba que no se haya repetido el encabezado
        resultado = false; //marca el error si aparece mas de una vez
    }

    destruirHistorial(historialAntes); //libera los datos originales para simular un reinicio

    HistorialPartidas historialDespues; //crea una lista nueva para cargar desde el archivo
    inicializarHistorial(historialDespues); //inicializa la lista vacia

    ostringstream errores; //guarda los mensajes emitidos durante la lectura

    if (!cargarHistorialCSV(ruta, historialDespues, errores)) { //carga el historial desde el archivo
        resultado = false; //marca el error si falla la lectura
    }

    if (contarRegistros(historialDespues) != 5) { //comprueba que se conservaron solo las cinco filas validas
        resultado = false; //marca el error si el numero no coincide
    }

    string textoErrores = errores.str(); //obtiene los mensajes de las filas invalidas

    if (textoErrores.find("Linea 5") == string::npos) { //comprueba que se reporto la primera fila invalida
        resultado = false; //marca el error si no se encontro su numero
    }

    if (textoErrores.find("Linea 6") == string::npos) { //comprueba que se reporto la segunda fila invalida
        resultado = false; //marca el error si no se encontro su numero
    }

    EstadisticasJugador estadisticasDespues; //guarda las estadisticas despues de cargar

    if (!calcularEstadisticasJugador(historialDespues, "ANA", estadisticasDespues)) { //calcula nuevamente las estadisticas
        resultado = false; //marca el error si falla el calculo
    }

    if (!estadisticasP13Iguales(estadisticasAntes, estadisticasDespues)) { //compara ambos resultados estadisticos
        resultado = false; //marca el error si cambiaron al guardar y cargar
    }

    Ranking rankingDespues; //crea el ranking con los registros recuperados
    inicializarRanking(rankingDespues); //deja el ranking vacio
    construirRankingDesdeHistorial(historialDespues, rankingDespues); //ordena los registros recuperados

    if (!rankingsP13Iguales(rankingAntes, rankingDespues)) { //comprueba que el ranking conserve el mismo orden
        resultado = false; //marca el error si se modificaron las posiciones
    }

    destruirRanking(rankingAntes); //libera los nodos del ranking original
    destruirRanking(rankingDespues); //libera los nodos del ranking recuperado
    destruirHistorial(historialDespues); //libera los registros cargados

    remove(ruta.c_str()); //elimina el archivo creado para esta prueba

    if (!pruebaGuardarPartidaCompletaP13()) { //ejecuta las comprobaciones del guardado conjunto
        resultado = false; //marca el error si falla alguna de ellas
    }

    return resultado; //devuelve el resultado final de p13
}
