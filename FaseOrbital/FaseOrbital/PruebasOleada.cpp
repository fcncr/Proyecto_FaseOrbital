#include "PruebasOleada.h"
#include "../FaseOrbital/Oleada.h"

//compara todos los datos de dos apariciones sin considerar los nodos que las contienen
static bool spawnsIguales(const Spawn& a, const Spawn& b) {
    return a.tick == b.tick && a.tipo == b.tipo && a.color == b.color && a.angulo == b.angulo && a.sentido == b.sentido; //exige el mismo momento tipo fase angulo y sentido
}

//revisa que dos colas tengan la misma cantidad de apariciones y los mismos datos en el mismo orden
static bool colasMismosDatos(const ColaSpawn& a, const ColaSpawn& b) {
    if (a.tam != b.tam) { //descarta colas que registran distintas cantidades de elementos
        return false; //informa la diferencia de tamano
    }

    const NodoSpawn* nodoA = a.frente; //empieza a consultar la primera cola sin modificarla
    const NodoSpawn* nodoB = b.frente; //empieza desde la misma posicion de la segunda cola

    while (nodoA != nullptr && nodoB != nullptr) { //compara los nodos mientras ambas colas tengan datos
        if (!spawnsIguales(nodoA->dato, nodoB->dato)) { //detecta una aparicion diferente en la posicion actual
            return false; //rechaza las colas al encontrar datos distintos
        }

        nodoA = nodoA->sig; //avanza en la primera cola
        nodoB = nodoB->sig; //avanza a la misma posicion en la segunda
    }

    return nodoA == nullptr && nodoB == nullptr; //acepta solo si ambas colas terminaron al mismo tiempo
}

//comprueba que los nodos de cada posicion de dos colas sean distintos y que los recorridos terminen juntos
static bool nodosIndependientes(const ColaSpawn& a, const ColaSpawn& b) {
    if (a.tam != b.tam) { //comprueba que las colas tengan la misma cantidad para comparar sus posiciones
        return false; //rechaza una diferencia en los tamanos registrados
    }

    const NodoSpawn* nodoA = a.frente; //consulta la direccion del primer nodo de la primera cola
    const NodoSpawn* nodoB = b.frente; //consulta el nodo correspondiente de la otra cola

    while (nodoA != nullptr && nodoB != nullptr) { //revisa las direcciones de los nodos de ambas colas
        if (nodoA == nodoB) { //detecta si las colas comparten el nodo de esta posicion
            return false; //informa que la copia no es independiente en este punto
        }

        nodoA = nodoA->sig; //avanza en la primera cadena de nodos
        nodoB = nodoB->sig; //avanza en la segunda cadena de nodos
    }

    return nodoA == nullptr && nodoB == nullptr; //comprueba que no queden nodos adicionales en ninguna cola
}

//indica los tipos de enemigo que se esperan en cada una de las oleadas probadas
static bool tipoPermitido(int numeroOleada, TipoEnemigo tipo) {
    if (numeroOleada == 1) { //revisa el grupo inicial de enemigos
        return tipo == Dron; //acepta solamente drones en la primera oleada
    }

    if (numeroOleada == 2) { //revisa la oleada que incorpora giradores
        return tipo == Dron || tipo == Girador; //permite drones y giradores
    }

    if (numeroOleada == 3) { //revisa la incorporacion de divisores
        return tipo == Dron || tipo == Girador || tipo == Divisor; //acepta los tres tipos disponibles hasta esta oleada
    }

    if (numeroOleada == 4) { //revisa la oleada que incorpora tanques
        return tipo == Dron || tipo == Girador || tipo == Divisor || tipo == Tanque; //permite los tipos anteriores y el tanque
    }

    return tipo == Dron || tipo == Girador || tipo == Divisor || tipo == Tanque || tipo == Camaleon; //acepta los cinco tipos generables en los demas casos probados
}

//revisa la cantidad los datos y los intervalos de una oleada y comprueba que el ultimo nodo sea su final
static bool validarContenidoOleada(const ColaSpawn& cola, int numeroOleada) {
    int cantidadEsperada = 6 + 3 * numeroOleada; //calcula la cantidad que debe producir esta oleada

    if (cola.tam != cantidadEsperada) { //compara el tamano registrado con la regla de crecimiento
        return false; //rechaza una cantidad de apariciones incorrecta
    }

    if (cola.frente == nullptr || cola.final == nullptr) { //comprueba que existan los dos extremos de la cola generada
        return false; //rechaza una cola sin frente o sin final
    }

    if (cola.frente->dato.tick != 60) { //revisa el momento de la primera aparicion
        return false; //informa que la oleada no comienza en el tick sesenta
    }

    int intervaloBase = 35 - 2 * numeroOleada; //obtiene la separacion base que corresponde al numero de oleada

    if (intervaloBase < 9) { //aplica el minimo usado por el generador
        intervaloBase = 9; //deja la base en nueve ticks para las oleadas mas avanzadas
    }

    int intervaloMinimo = intervaloBase * 60 / 100; //calcula el menor intervalo permitido con division entera
    int intervaloMaximo = intervaloBase * 140 / 100; //calcula el mayor intervalo permitido con division entera

    const NodoSpawn* actual = cola.frente; //empieza a validar desde la primera aparicion
    const NodoSpawn* anterior = nullptr; //permite comparar los ticks y conserva el ultimo nodo al terminar el recorrido
    int contador = 0; //cuenta los nodos recorridos sin depender del tamano guardado

    while (actual != nullptr) { //revisa todos los datos de la cola generada
        const Spawn& spawn = actual->dato; //consulta la aparicion actual sin copiarla

        if (!tipoPermitido(numeroOleada, spawn.tipo)) { //detecta un enemigo que aun no deberia aparecer
            return false; //rechaza el tipo no permitido
        }

        if (spawn.color < 0 || spawn.color > 2) { //comprueba que la fase sea una de las tres disponibles
            return false; //rechaza un color fuera del rango
        }

        if (spawn.angulo < 0 || spawn.angulo > 359) { //revisa que el angulo pertenezca a una vuelta completa
            return false; //rechaza un angulo fuera del rango de generacion
        }

        if (spawn.tipo == Girador) { //los giradores deben elegir uno de los dos sentidos de giro
            if (spawn.sentido != 0 && spawn.sentido != 1) { //comprueba las dos representaciones aceptadas para el sentido
                return false; //rechaza un sentido de giro invalido
            }
        }
        else { //los demas tipos no sortean un sentido de giro
            if (spawn.sentido != 0) { //revisa el valor fijo esperado para esos enemigos
                return false; //rechaza un sentido que no corresponde al tipo
            }
        }

        if (anterior != nullptr) { //calcula el intervalo solo cuando ya se reviso otra aparicion
            int diferencia = spawn.tick - anterior->dato.tick; //mide la separacion respecto al nodo anterior

            if (numeroOleada < 7 && diferencia <= 0) { //exige ticks crecientes antes de habilitar apariciones simultaneas
                return false; //rechaza una repeticion o un retroceso del tiempo en estas oleadas
            }

            if (numeroOleada >= 7 && diferencia < 0) { //permite compartir tick desde la septima oleada pero no retroceder
                return false; //informa que los tiempos estan desordenados
            }

            if (diferencia != 0 && (diferencia < intervaloMinimo || diferencia > intervaloMaximo)) { //revisa los limites cuando las apariciones no son simultaneas
                return false; //rechaza una separacion fuera del margen esperado
            }
        }

        anterior = actual; //conserva este nodo para comparar el tick de la siguiente aparicion
        actual = actual->sig; //avanza a la siguiente aparicion
        contador++; //suma el nodo que acaba de validarse
    }

    if (contador != cantidadEsperada) { //compara los nodos encontrados con la cantidad calculada
        return false; //detecta una cadena cuya longitud no coincide con el tamano esperado
    }

    if (anterior != cola.final) { //comprueba que el extremo guardado sea el ultimo nodo recorrido
        return false; //rechaza un puntero final que no corresponda a la cadena
    }

    return true; //confirma que la oleada cumple todas las reglas revisadas
}

//busca dos apariciones consecutivas programadas para el mismo tick
static bool tieneTickCompartido(const ColaSpawn& cola) {
    const NodoSpawn* actual = cola.frente; //empieza desde la primera aparicion

    while (actual != nullptr && actual->sig != nullptr) { //compara parejas mientras quede un nodo siguiente
        if (actual->dato.tick == actual->sig->dato.tick) { //detecta dos enemigos que aparecen al mismo tiempo
            return true; //confirma que encontro al menos un tick compartido
        }

        actual = actual->sig; //avanza a la siguiente pareja de apariciones
    }

    return false; //informa que no hay apariciones consecutivas con el mismo tick
}

//genera dos veces una oleada y revisa su contenido la igualdad de datos y la independencia de sus nodos
//puede exigir apariciones simultaneas y comprueba que destruir una cola no altere la otra
static bool probarOleada(int numeroOleada, uint32_t semilla, bool exigirTickCompartido) {
    ColaSpawn colaA; //recibe la primera generacion de apariciones
    ColaSpawn colaB; //recibe la segunda generacion con los mismos parametros
    inicializarColaSpawn(colaA); //prepara la primera cola vacia
    inicializarColaSpawn(colaB); //prepara la segunda cola vacia

    Oleada oleada{ numeroOleada, semilla }; //reune los datos que se usaran en ambas generaciones

    generarOleada(oleada, colaA); //produce las apariciones de la primera cola
    generarOleada(oleada, colaB); //repite la generacion para comparar sus resultados

    bool resultado = true; //conserva cualquier fallo detectado en este caso

    if (!validarContenidoOleada(colaA, numeroOleada) || !validarContenidoOleada(colaB, numeroOleada)) { //revisa que ambas colas respeten las reglas de la oleada
        resultado = false; //marca un fallo en los datos generados
    }

    if (!colasMismosDatos(colaA, colaB)) { //comprueba que repetir los parametros produzca las mismas apariciones
        resultado = false; //registra una diferencia entre las dos generaciones
    }

    if (!nodosIndependientes(colaA, colaB)) { //revisa que cada cola conserve sus propios nodos
        resultado = false; //marca un fallo en la independencia de las colas
    }

    if (exigirTickCompartido && !tieneTickCompartido(colaA)) { //busca apariciones simultaneas cuando el escenario las exige
        resultado = false; //informa que falta el tick compartido esperado
    }

    destruirColaSpawn(colaA); //libera la primera cola antes de volver a revisar la segunda

    if (!validarContenidoOleada(colaB, numeroOleada)) { //comprueba que la segunda cola siga siendo valida despues de destruir la primera
        resultado = false; //registra que la otra cola ya no cumple el contenido esperado
    }

    destruirColaSpawn(colaB); //libera las apariciones que quedan en el escenario

    return resultado; //devuelve si la generacion repetida paso las comprobaciones
}

//genera la quinta oleada con dos semillas concretas y comprueba que sus datos difieran sin compartir nodos
static bool probarSemillaDiferente() {
    ColaSpawn colaA; //guarda las apariciones de la primera semilla
    ColaSpawn colaB; //guarda las apariciones de la segunda semilla
    inicializarColaSpawn(colaA); //inicia la primera cola sin datos
    inicializarColaSpawn(colaB); //inicia la segunda cola sin datos

    Oleada oleadaA{ 5, 42u }; //define la quinta oleada con semilla cuarenta y dos
    Oleada oleadaB{ 5, 99u }; //conserva el numero de oleada y cambia la semilla a noventa y nueve

    generarOleada(oleadaA, colaA); //genera las apariciones con la primera semilla
    generarOleada(oleadaB, colaB); //genera las apariciones con la segunda semilla

    bool resultado = true; //acumula los fallos de esta comparacion

    if (!validarContenidoOleada(colaA, 5) || !validarContenidoOleada(colaB, 5)) { //revisa las reglas de la quinta oleada para las dos semillas
        resultado = false; //marca un contenido que no cumple las reglas
    }

    if (colasMismosDatos(colaA, colaB)) { //detecta si las dos semillas concretas produjeron datos identicos
        resultado = false; //informa que no se obtuvo la diferencia esperada en este caso
    }

    if (!nodosIndependientes(colaA, colaB)) { //comprueba que las generaciones usen nodos separados
        resultado = false; //registra un problema de independencia
    }

    destruirColaSpawn(colaA); //libera la cola de la primera semilla

    if (!validarContenidoOleada(colaB, 5)) { //vuelve a revisar la cola restante tras destruir la otra
        resultado = false; //marca cualquier alteracion del contenido esperado
    }

    destruirColaSpawn(colaB); //libera los nodos de la segunda semilla

    return resultado; //devuelve si ambas generaciones respetaron lo esperado para estas semillas
}

//prueba las oleadas uno dos cinco y siete con la misma semilla y compara despues dos semillas distintas
//detiene el grupo al encontrar el primer caso que falla
bool pruebaOleadasDeterministas() {
    if (!probarOleada(1, 42u, false)) { //revisa la oleada inicial que solo genera drones
        return false; //detiene el grupo si falla el primer escenario
    }

    if (!probarOleada(2, 42u, false)) { //comprueba la oleada que permite incorporar giradores
        return false; //informa el fallo de la segunda oleada
    }

    if (!probarOleada(5, 42u, false)) { //revisa una oleada que admite todos los tipos generables
        return false; //detiene el grupo si ese escenario no cumple las reglas
    }

    if (!probarOleada(7, 42u, true)) { //exige que la septima oleada incluya al menos un tick compartido
        return false; //informa el fallo del escenario con apariciones simultaneas
    }

    if (!probarSemillaDiferente()) { //compara la quinta oleada generada con semillas cuarenta y dos y noventa y nueve
        return false; //informa que esa comparacion no produjo los resultados esperados
    }

    return true; //confirma que todos los escenarios de oleadas pasaron
}
