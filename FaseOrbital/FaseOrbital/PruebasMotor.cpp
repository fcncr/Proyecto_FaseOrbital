#include "PruebasMotor.h"
#include "Motor.h"
#include "ScriptAcciones.h"
#include <fstream>
#include <cstdio>
#include "ModoLotes.h"

using namespace std; //permite usar los archivos y su borrado sin escribir el prefijo

//compara dos valores decimales admitiendo una diferencia menor que una milesima
static bool casiIgualesOrden(double a, double b) {
    double diferencia = a - b; //calcula la diferencia entre los dos valores

    if (diferencia < 0.0) diferencia = -diferencia; //convierte la diferencia en su magnitud positiva

    return diferencia < 0.001; //acepta solo diferencias menores que la tolerancia elegida
}

//comprueba el estado inicial del turno y que la cola del motor sea una copia independiente
bool pruebaInicializacionMotor() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Spawn spawn{ 60, Dron, 1, 90, 0 }; //programa un dron para revisar que su aparicion se copie
    encolarSpawn(cola, spawn); //agrega la aparicion al final conservando el orden preparado

    Jugador jugador{ "Prueba", false, 80, 500, 7, 45 }; //prepara un jugador con vida puntaje combo y carga previos

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 3, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    if (motor.tickActual != 0) resultado = false; //comprueba que el turno empiece en el tick cero
    if (motor.numeroOleada != 3) resultado = false; //verifica que se conserve el numero de oleada recibido

    if (motor.nave.anguloActual != 270.0) resultado = false; //revisa la posicion angular inicial de la nave
    if (motor.nave.anguloObjetivo != 270.0) resultado = false; //comprueba que el destino inicial coincida con la posicion
    if (motor.nave.color != 0) resultado = false; //verifica que la fase inicial sea cian

    if (motor.pulso.activo) resultado = false; //rechaza un pulso que aparezca activo al iniciar
    if (motor.pulso.radioActual != 0.0) resultado = false; //comprueba que el pulso comience sin radio

    if (jugador.vidaNucleo != 80) resultado = false; //verifica que iniciar el motor conserve la vida del jugador
    if (jugador.puntaje != 500) resultado = false; //revisa que el puntaje previo no se borre
    if (jugador.combo != 0) resultado = false; //comprueba que el combo se reinicie para el nuevo turno
    if (jugador.cargaPulso != 45) resultado = false; //verifica que se conserve la carga previa del pulso

    if (motor.enemigos.tam != 0) resultado = false; //comprueba que todavia no haya enemigos activos
    if (motor.balas.tam != 0) resultado = false; //revisa que no existan disparos al comenzar

    if (motor.spawns.tam != 1) resultado = false; //comprueba que la aparicion se haya copiado al motor
    if (cola.tam != 1) resultado = false; //verifica que copiar no consuma la cola original

    if (motor.spawns.frente == cola.frente) resultado = false; //rechaza que ambas colas compartan el nodo de inicio

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor

    if (cola.tam != 1) resultado = false; //comprueba que destruir el motor no borre la cola original

    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//verifica que la nave avance como maximo seis grados por paso y se detenga en el destino
bool pruebaMovimientoNave() {
    Motor motor{}; //deja los campos del motor en cero para probar solo el movimiento o la accion

    motor.nave.anguloActual = 270.0; //situa la nave en el angulo inicial del recorrido
    motor.nave.anguloObjetivo = 290.0; //fija un destino que requiere varios pasos

    actualizarNave(motor); //acerca la nave a su destino un paso

    if (motor.nave.anguloActual != 276.0) return false; //comprueba el avance de seis grados del primer paso

    actualizarNave(motor); //acerca la nave a su destino un paso

    if (motor.nave.anguloActual != 282.0) return false; //revisa que el segundo paso avance otros seis grados

    actualizarNave(motor); //acerca la nave a su destino un paso

    if (motor.nave.anguloActual != 288.0) return false; //verifica que el tercer paso quede a dos grados del destino

    actualizarNave(motor); //acerca la nave a su destino un paso

    if (motor.nave.anguloActual != 290.0) return false; //comprueba que el ultimo paso no rebase el destino

    return true; //confirma que se cumplieron los resultados esperados
}

//comprueba que la nave cruce el angulo cero por el camino corto en ambos sentidos
bool pruebaCruceAngular() {
    Motor motor{}; //deja los campos del motor en cero para probar solo el movimiento o la accion

    motor.nave.anguloActual = 358.0; //situa la nave justo antes de completar la vuelta
    motor.nave.anguloObjetivo = 2.0; //elige un destino al otro lado del angulo cero

    actualizarNave(motor); //acerca la nave a su destino un paso

    if (motor.nave.anguloActual != 2.0) return false; //comprueba que alcance el destino cruzando por cero

    motor.nave.anguloActual = 2.0; //coloca la nave al inicio del cruce en sentido contrario
    motor.nave.anguloObjetivo = 358.0; //elige un destino justo antes de completar la vuelta

    actualizarNave(motor); //acerca la nave a su destino un paso

    if (motor.nave.anguloActual != 358.0) return false; //verifica el cruce corto en el sentido contrario

    motor.nave.anguloActual = 350.0; //prepara un cruce que necesita mas de seis grados
    motor.nave.anguloObjetivo = 20.0; //fija un destino despues de pasar por cero

    actualizarNave(motor); //acerca la nave a su destino un paso

    if (motor.nave.anguloActual != 356.0) return false; //comprueba que el avance siga limitado a seis grados

    return true; //confirma que se cumplieron los resultados esperados
}

//revisa el cambio de destino y color junto con la activacion del pulso al tener carga completa
bool pruebaAccionesMotor() {
    Motor motor{}; //deja los campos del motor en cero para probar solo el movimiento o la accion

    Jugador jugador{ "Prueba", false, 100, 0, 5, 100 }; //prepara un jugador con la carga necesaria para activar el pulso

    motor.jugador = &jugador; //asocia el motor con el jugador que aporta la carga
    motor.nave.color = 0; //establece una fase inicial distinta de la solicitada
    motor.pulso.activo = false; //deja el pulso apagado antes de aplicar la accion
    motor.pulso.radioActual = 0.0; //elimina cualquier radio previo del pulso

    AccionMotor accion{ true, 370.0, true, 2, true }; //solicita un giro que excede una vuelta junto con el cambio a ambar y el pulso

    aplicarAccion(motor, accion); //aplica los cambios y la solicitud de pulso indicados

    if (motor.nave.anguloObjetivo != 10.0) return false; //comprueba que el destino se normalice a diez grados
    if (motor.nave.color != 2) return false; //verifica que se aplique la fase ambar solicitada

    if (!motor.pulso.activo) return false; //rechaza que el pulso siga apagado con carga completa
    if (motor.pulso.radioActual != 20.0) return false; //comprueba que el pulso comience con radio veinte
    if (jugador.cargaPulso != 0) return false; //verifica que activar el pulso consuma toda su carga

    return true; //confirma que se cumplieron los resultados esperados
}

//comprueba que aparezcan los enemigos cuyo tick ya llego con sus datos y sin consumir la cola original
bool pruebaSpawnsMotor() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Spawn spawn1{ 0, Dron, 0, 10, 0 }; //programa el primer enemigo para el inicio del turno
    Spawn spawn2{ 3, Girador, 1, 350, 0 }; //programa un girador para el tick tres
    Spawn spawn3{ 3, Tanque, 2, 90, 1 }; //agrega un tanque que debe aparecer junto al girador
    Spawn spawn4{ 7, Camaleon, 1, 180, 0 }; //deja un camaleon pendiente para el tick siete

    encolarSpawn(cola, spawn1); //agrega la aparicion al final conservando el orden preparado
    encolarSpawn(cola, spawn2); //agrega la aparicion al final conservando el orden preparado
    encolarSpawn(cola, spawn3); //agrega la aparicion al final conservando el orden preparado
    encolarSpawn(cola, spawn4); //agrega la aparicion al final conservando el orden preparado

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara un jugador con vida completa y sin puntos combo ni carga

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 5, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    motor.tickActual = 3; //situa el motor en el tick compartido por dos apariciones
    procesarSpawns(motor); //crea los enemigos cuyas apariciones ya deben ocurrir

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    if (motor.enemigos.tam != 3) resultado = false; //comprueba que se creen las tres apariciones vencidas
    if (motor.spawns.tam != 1) resultado = false; //verifica que solo quede pendiente el camaleon

    NodoEnemigo* dron = motor.enemigos.cabeza; //consulta el primer enemigo que debio aparecer
    NodoEnemigo* girador = dron != nullptr ? dron->sig : nullptr; //obtiene el segundo nodo sin desreferenciar un puntero nulo
    NodoEnemigo* tanque = girador != nullptr ? girador->sig : nullptr; //consulta el tercer nodo si el girador existe

    if (dron == nullptr || dron->dato.id != 1 || dron->dato.tipo != Dron) resultado = false; //comprueba el tipo y el identificador del primer enemigo
    if (dron != nullptr && (dron->dato.r != 300.0 || dron->dato.theta != 10.0 || dron->dato.vida != 1 || dron->dato.radio != 6)) resultado = false; //revisa la posicion la vida y el radio iniciales del dron

    if (girador == nullptr || girador->dato.id != 2 || girador->dato.tipo != Girador) resultado = false; //verifica que el segundo enemigo sea el girador esperado
    if (girador != nullptr && (girador->dato.color != 1 || girador->dato.radio != 6)) resultado = false; //comprueba la fase y el radio del girador
    if (girador != nullptr && (girador->dato.omega < -1.151 || girador->dato.omega > -1.149)) resultado = false; //revisa la velocidad angular negativa con tolerancia de una milesima

    if (tanque == nullptr || tanque->dato.id != 3 || tanque->dato.tipo != Tanque) resultado = false; //verifica que el tercer enemigo sea el tanque esperado
    if (tanque != nullptr && (tanque->dato.vida != 3 || tanque->dato.radio != 10)) resultado = false; //comprueba la vida y el radio propios del tanque

    if (motor.enemigos.cola != tanque) resultado = false; //revisa que el tanque cierre la lista actual

    motor.tickActual = 7; //avanza hasta el tick de la ultima aparicion
    procesarSpawns(motor); //crea los enemigos cuyas apariciones ya deben ocurrir

    if (motor.enemigos.tam != 4) resultado = false; //comprueba que ya esten los cuatro enemigos
    if (!colaSpawnVacia(motor.spawns)) resultado = false; //verifica que no queden apariciones dentro del motor

    NodoEnemigo* camaleon = motor.enemigos.cola; //consulta el ultimo enemigo creado

    if (camaleon == nullptr || camaleon->dato.id != 4 || camaleon->dato.tipo != Camaleon) resultado = false; //comprueba el tipo y el identificador del camaleon
    if (camaleon != nullptr && (camaleon->dato.vida != 1 || camaleon->dato.radio != 7 || camaleon->dato.ticksColor != 0)) resultado = false; //revisa la vida el radio y el contador inicial de cambio de color

    if (cola.tam != 4) resultado = false; //verifica que la cola original conserve sus cuatro apariciones

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//revisa el avance de los seis tipos de enemigos y el cambio de fase del camaleon
bool pruebaMovimientoEnemigos() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara un jugador con vida completa y sin puntos combo ni carga

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 5, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    Enemigo dron{ 1, Dron, 0, 300.0, 10.0, 0.0, 1, 6, 0 }; //prepara un dron para medir su avance radial
    Enemigo girador{ 2, Girador, 0, 300.0, 10.0, 1.15, 1, 6, 0 }; //prepara un girador con velocidad angular positiva
    Enemigo divisor{ 3, Divisor, 0, 300.0, 20.0, 0.0, 1, 8, 0 }; //agrega un divisor para comprobar su velocidad radial
    Enemigo mini{ 4, Mini, 1, 300.0, 20.0, -1.7, 1, 4, 0 }; //prepara un mini con giro en el sentido contrario
    Enemigo tanque{ 5, Tanque, 2, 300.0, 30.0, 0.0, 3, 10, 0 }; //agrega un tanque para revisar su avance mas lento
    Enemigo camaleon{ 6, Camaleon, 0, 300.0, 40.0, 0.0, 1, 7, 41 }; //deja al camaleon a un paso de cambiar de fase

    insertarFinal(motor.enemigos, dron); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, girador); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, divisor); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, mini); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, tanque); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, camaleon); //agrega el enemigo al final para conservar el orden del escenario

    moverEnemigos(motor); //avanza todos los enemigos un paso segun su tipo

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    NodoEnemigo* actual = motor.enemigos.cabeza; //comienza las comprobaciones por el dron

    if (actual == nullptr || actual->dato.r < 298.799 || actual->dato.r > 298.801) resultado = false; //verifica la distancia del dron despues de avanzar

    if (actual != nullptr) actual = actual->sig; //avanza al siguiente enemigo si el nodo actual existe

    if (actual == nullptr || actual->dato.r < 298.979 || actual->dato.r > 298.981) resultado = false; //revisa el avance radial del girador
    if (actual != nullptr && (actual->dato.theta < 11.149 || actual->dato.theta > 11.151)) resultado = false; //comprueba el nuevo angulo del girador

    if (actual != nullptr) actual = actual->sig; //avanza al siguiente enemigo si el nodo actual existe

    if (actual == nullptr || actual->dato.r < 299.039 || actual->dato.r > 299.041) resultado = false; //verifica la distancia recorrida por el divisor

    if (actual != nullptr) actual = actual->sig; //avanza al siguiente enemigo si el nodo actual existe

    if (actual == nullptr || actual->dato.r < 298.499 || actual->dato.r > 298.501) resultado = false; //revisa el avance radial del mini
    if (actual != nullptr && (actual->dato.theta < 18.299 || actual->dato.theta > 18.301)) resultado = false; //comprueba el giro negativo del mini

    if (actual != nullptr) actual = actual->sig; //avanza al siguiente enemigo si el nodo actual existe

    if (actual == nullptr || actual->dato.r < 299.399 || actual->dato.r > 299.401) resultado = false; //verifica la distancia del tanque despues de avanzar

    if (actual != nullptr) actual = actual->sig; //avanza al siguiente enemigo si el nodo actual existe

    if (actual == nullptr || actual->dato.r < 298.979 || actual->dato.r > 298.981) resultado = false; //revisa el avance radial del camaleon
    if (actual != nullptr && actual->dato.color != 1) resultado = false; //comprueba que el camaleon pase a la fase magenta
    if (actual != nullptr && actual->dato.ticksColor != 0) resultado = false; //verifica que el contador de cambio de fase vuelva a cero

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//comprueba la cadencia de cuatro ticks y la posicion velocidad y fase de cada disparo
bool pruebaDisparoAutomatico() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara un jugador con vida completa y sin puntos combo ni carga

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    motor.tickActual = 0; //elige el primer tick en que debe generarse un disparo
    motor.nave.anguloActual = 0.0; //situa la nave sobre el eje horizontal positivo
    motor.nave.color = 2; //selecciona ambar para el primer disparo

    dispararAutomaticamente(motor); //intenta crear un disparo segun el tick actual

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    if (motor.balas.tam != 1) resultado = false; //comprueba que el tick cero cree una sola bala

    NodoBala* bala1 = motor.balas.cabeza; //consulta el disparo que acaba de crearse

    if (bala1 == nullptr) resultado = false; //marca el fallo si no existe el primer disparo

    if (bala1 != nullptr && (bala1->dato.x < 99.999 || bala1->dato.x > 100.001)) resultado = false; //revisa que la bala salga a distancia cien sobre el eje horizontal
    if (bala1 != nullptr && (bala1->dato.y < -0.001 || bala1->dato.y > 0.001)) resultado = false; //comprueba que su posicion vertical sea practicamente cero
    if (bala1 != nullptr && (bala1->dato.vx < 8.999 || bala1->dato.vx > 9.001)) resultado = false; //verifica la velocidad horizontal de nueve unidades
    if (bala1 != nullptr && (bala1->dato.vy < -0.001 || bala1->dato.vy > 0.001)) resultado = false; //revisa que no tenga desplazamiento vertical
    if (bala1 != nullptr && bala1->dato.color != 2) resultado = false; //comprueba que el disparo conserve la fase ambar de la nave

    motor.tickActual = 1; //elige un tick en que no corresponde disparar
    dispararAutomaticamente(motor); //intenta crear un disparo segun el tick actual

    if (motor.balas.tam != 1) resultado = false; //verifica que no se agregue una bala fuera de la cadencia

    motor.tickActual = 4; //avanza al siguiente tick de disparo
    motor.nave.anguloActual = 90.0; //situa la nave sobre el eje vertical positivo
    motor.nave.color = 1; //cambia la fase para comprobar el segundo disparo

    dispararAutomaticamente(motor); //intenta crear un disparo segun el tick actual

    if (motor.balas.tam != 2) resultado = false; //comprueba que se haya agregado una segunda bala

    NodoBala* bala2 = motor.balas.cabeza; //consulta la bala nueva insertada al inicio de la lista

    if (bala2 == nullptr) resultado = false; //marca el fallo si falta el segundo disparo

    if (bala2 != nullptr && (bala2->dato.x < -0.001 || bala2->dato.x > 0.001)) resultado = false; //verifica que la posicion horizontal sea practicamente cero
    if (bala2 != nullptr && (bala2->dato.y < 99.999 || bala2->dato.y > 100.001)) resultado = false; //revisa que la bala salga a distancia cien sobre el eje vertical
    if (bala2 != nullptr && (bala2->dato.vx < -0.001 || bala2->dato.vx > 0.001)) resultado = false; //comprueba que no tenga desplazamiento horizontal
    if (bala2 != nullptr && (bala2->dato.vy < 8.999 || bala2->dato.vy > 9.001)) resultado = false; //verifica la velocidad vertical de nueve unidades
    if (bala2 != nullptr && bala2->dato.color != 1) resultado = false; //comprueba que el segundo disparo tenga la fase magenta

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//revisa el dano por choques con la nave y el nucleo y los enlaces del enemigo que sobrevive
bool pruebaChoquesEnemigos() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Jugador jugador{ "Prueba", false, 100, 0, 9, 0 }; //prepara el jugador que recibira el dano de los choques

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    motor.nave.anguloActual = 0.0; //coloca la nave frente al enemigo que debe chocar con ella

    Enemigo choqueNave{ 1, Dron, 0, 100.0, 0.0, 0.0, 1, 6, 0 }; //situa un dron sobre la posicion de la nave
    Enemigo nucleoDron{ 2, Dron, 0, 22.0, 180.0, 0.0, 1, 6, 0 }; //coloca otro dron en contacto con el nucleo
    Enemigo sobreviviente{ 3, Dron, 0, 200.0, 90.0, 0.0, 1, 6, 0 }; //deja un enemigo lejos de ambas zonas de choque
    Enemigo nucleoTanque{ 4, Tanque, 0, 24.0, 270.0, 0.0, 3, 10, 0 }; //situa un tanque en contacto con el nucleo

    insertarFinal(motor.enemigos, choqueNave); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, nucleoDron); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, sobreviviente); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, nucleoTanque); //agrega el enemigo al final para conservar el orden del escenario

    resolverChoquesEnemigos(motor); //resuelve los contactos con la nave y con el nucleo

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    if (jugador.vidaNucleo != 56) resultado = false; //comprueba la vida restante despues de los tres choques
    if (jugador.combo != 0) resultado = false; //verifica que el combo quede en cero

    if (motor.enemigos.tam != 1) resultado = false; //comprueba que solo quede el enemigo alejado
    if (motor.enemigos.cabeza == nullptr) resultado = false; //rechaza una lista sin cabeza pese a quedar un enemigo
    if (motor.enemigos.cola == nullptr) resultado = false; //rechaza una lista sin cola pese a quedar un enemigo

    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->dato.id != 3) resultado = false; //verifica que sobreviva el enemigo con identificador tres
    if (motor.enemigos.cola != motor.enemigos.cabeza) resultado = false; //comprueba que cabeza y cola apunten al unico sobreviviente

    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->ant != nullptr) resultado = false; //revisa que no queden enlaces hacia nodos anteriores eliminados
    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->sig != nullptr) resultado = false; //comprueba que no quede un enlace siguiente en el unico nodo

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//comprueba que un impacto de color incorrecto empuje al enemigo sin matarlo y rompa el combo sin dar puntos
bool pruebaP09() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Jugador jugador{ "Prueba", false, 100, 0, 5, 0 }; //prepara un jugador antes de configurar el combo que se probara

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    jugador.combo = 5; //restablece el combo despues de que la inicializacion del motor lo reinicia

    Enemigo enemigo{ 1, Dron, 0, 200.0, 0.0, 0.0, 1, 6, 0 }; //situa un dron cian en el camino de la bala
    insertarFinal(motor.enemigos, enemigo); //agrega el enemigo al final para conservar el orden del escenario

    Bala bala{ 191.0, 0.0, 9.0, 0.0, 1, 0 }; //prepara una bala magenta que alcanzara al dron tras avanzar
    insertarInicio(motor.balas, bala); //agrega el disparo a la lista que procesara el motor

    moverBalasYResolverImpactos(motor); //mueve las balas y resuelve sus contactos o su salida del escenario

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    if (motor.balas.tam != 0) resultado = false; //comprueba que el impacto consuma la bala
    if (motor.enemigos.tam != 1) resultado = false; //verifica que el enemigo siga en la lista
    if (motor.enemigos.cabeza == nullptr) resultado = false; //rechaza que falte el nodo del enemigo sobreviviente

    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->dato.vida != 1) resultado = false; //comprueba que el impacto incorrecto no le quite vida
    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->dato.r != 209.0) resultado = false; //verifica el empuje radial de nueve unidades

    if (jugador.combo != 0) resultado = false; //comprueba que el impacto incorrecto rompa el combo
    if (jugador.puntaje != 0) resultado = false; //revisa que el error no otorgue puntos

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//verifica que abatir un divisor cree dos minis en su lugar y conserve los enlaces y las recompensas
bool pruebaP10() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara un jugador con vida completa y sin puntos combo ni carga

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 3, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    motor.siguienteIdEnemigo = 4; //reserva identificadores nuevos para los minis que se creen

    Enemigo anterior{ 1, Dron, 0, 250.0, 90.0, 0.0, 1, 6, 0 }; //coloca un enemigo antes del divisor para revisar sus enlaces
    Enemigo divisor{ 2, Divisor, 0, 200.0, 0.0, 0.0, 1, 8, 0 }; //situa el divisor en el trayecto del disparo
    Enemigo siguiente{ 3, Tanque, 2, 260.0, 180.0, 0.0, 3, 10, 0 }; //coloca un enemigo despues del divisor para comprobar la reconexion

    insertarFinal(motor.enemigos, anterior); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, divisor); //agrega el enemigo al final para conservar el orden del escenario
    insertarFinal(motor.enemigos, siguiente); //agrega el enemigo al final para conservar el orden del escenario

    Bala bala{ 191.0, 0.0, 9.0, 0.0, 0, 0 }; //prepara una bala del mismo color para abatir al divisor
    insertarInicio(motor.balas, bala); //agrega el disparo a la lista que procesara el motor

    moverBalasYResolverImpactos(motor); //mueve las balas y resuelve sus contactos o su salida del escenario

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    if (motor.balas.tam != 0) resultado = false; //comprueba que la bala desaparezca al impactar
    if (motor.enemigos.tam != 4) resultado = false; //verifica que los dos minis reemplacen al divisor en la lista

    NodoEnemigo* nodoAnterior = motor.enemigos.cabeza; //consulta el vecino que estaba antes del divisor
    NodoEnemigo* mini1 = nodoAnterior != nullptr ? nodoAnterior->sig : nullptr; //localiza el primer mini si existe el nodo anterior
    NodoEnemigo* mini2 = mini1 != nullptr ? mini1->sig : nullptr; //consulta el segundo mini si se encontro el primero
    NodoEnemigo* nodoSiguiente = mini2 != nullptr ? mini2->sig : nullptr; //localiza el vecino que debe seguir a los dos minis

    if (nodoAnterior == nullptr || nodoAnterior->dato.id != 1) resultado = false; //comprueba que se conserve el enemigo anterior

    if (mini1 == nullptr || mini1->dato.tipo != Mini) resultado = false; //verifica que el primer nodo insertado sea un mini
    if (mini2 == nullptr || mini2->dato.tipo != Mini) resultado = false; //comprueba que el segundo nodo insertado sea otro mini

    if (mini1 != nullptr && mini1->dato.color != 1) resultado = false; //revisa la fase magenta del primer mini
    if (mini2 != nullptr && mini2->dato.color != 2) resultado = false; //verifica la fase ambar del segundo mini

    if (mini1 != nullptr && mini1->dato.theta != 356.0) resultado = false; //comprueba el angulo del primer mini al otro lado de cero
    if (mini2 != nullptr && mini2->dato.theta != 4.0) resultado = false; //revisa el angulo del segundo mini

    if (mini1 != nullptr && mini1->dato.omega != -1.7) resultado = false; //verifica el giro negativo del primer mini
    if (mini2 != nullptr && mini2->dato.omega != 1.7) resultado = false; //comprueba el giro positivo del segundo mini

    if (mini1 != nullptr && mini1->dato.r != 200.0) resultado = false; //revisa que el primer mini conserve la distancia del divisor
    if (mini2 != nullptr && mini2->dato.r != 200.0) resultado = false; //verifica la misma distancia para el segundo mini

    if (nodoSiguiente == nullptr || nodoSiguiente->dato.id != 3) resultado = false; //comprueba que se conserve el enemigo que seguia al divisor

    if (mini1 != nullptr && mini1->ant != nodoAnterior) resultado = false; //revisa el enlace anterior del primer mini
    if (mini1 != nullptr && mini1->sig != mini2) resultado = false; //comprueba que el primer mini apunte al segundo

    if (mini2 != nullptr && mini2->ant != mini1) resultado = false; //verifica el enlace de regreso del segundo mini
    if (mini2 != nullptr && mini2->sig != nodoSiguiente) resultado = false; //comprueba que el segundo mini se conecte con el vecino siguiente

    if (nodoSiguiente != nullptr && nodoSiguiente->ant != mini2) resultado = false; //revisa que el vecino siguiente apunte de regreso al segundo mini

    if (jugador.puntaje != 200) resultado = false; //verifica los puntos concedidos por abatir al divisor
    if (jugador.combo != 1) resultado = false; //comprueba que la baja aumente el combo en uno
    if (jugador.cargaPulso != 5) resultado = false; //revisa los cinco puntos de carga obtenidos por la baja

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//comprueba que una bala se retire de la lista al sobrepasar el radio permitido
bool pruebaEliminacionBalaPorRadio() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara un jugador con vida completa y sin puntos combo ni carga

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    Bala bala{ 319.0, 0.0, 9.0, 0.0, 0, 0 }; //situa una bala cerca del limite para que lo rebase al avanzar
    insertarInicio(motor.balas, bala); //agrega el disparo a la lista que procesara el motor

    moverBalasYResolverImpactos(motor); //mueve las balas y resuelve sus contactos o su salida del escenario

    bool resultado = motor.balas.tam == 0 && motor.balas.cabeza == nullptr; //comprueba que la lista quede vacia al retirar la bala

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//prueba el pulso con carga insuficiente y completa y revisa sus bajas y puntos sin aumentar el combo
bool pruebaP11() {
    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    {
        ColaSpawn cola; //guarda las apariciones originales que recibira el motor
        inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

        Jugador jugador{ "Prueba", false, 100, 0, 4, 99 }; //prepara un jugador al que le falta un punto de carga

        Motor motor; //reserva el estado de la simulacion para este caso
        inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

        Enemigo enemigo{ 1, Dron, 0, 32.0, 0.0, 0.0, 1, 6, 0 }; //coloca un enemigo al alcance de un pulso que no debe activarse
        insertarFinal(motor.enemigos, enemigo); //agrega el enemigo al final para conservar el orden del escenario

        AccionMotor accion{ false, 0.0, false, 0, true }; //solicita el pulso sin cambiar destino ni color

        aplicarAccion(motor, accion); //aplica los cambios y la solicitud de pulso indicados
        actualizarPulso(motor); //avanza el pulso activo y resuelve los enemigos que alcanza

        if (motor.pulso.activo) resultado = false; //rechaza que el pulso se active con carga insuficiente
        if (jugador.cargaPulso != 99) resultado = false; //comprueba que el intento fallido conserve la carga
        if (motor.enemigos.tam != 1) resultado = false; //verifica que el enemigo siga vivo al no activarse el pulso

        destruirMotor(motor); //libera las listas y la copia de apariciones del motor
        destruirColaSpawn(cola); //libera las apariciones originales de la prueba
    }

    {
        ColaSpawn cola; //guarda las apariciones originales que recibira el motor
        inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

        Jugador jugador{ "Prueba", false, 100, 0, 6, 100 }; //prepara un jugador con carga completa para el segundo caso

        Motor motor; //reserva el estado de la simulacion para este caso
        inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

        jugador.combo = 6; //fija el combo que el pulso debe conservar

        Enemigo dentro1{ 1, Dron, 0, 32.0, 0.0, 0.0, 1, 6, 0 }; //coloca un dron al alcance del primer avance del pulso
        Enemigo fuera{ 2, Dron, 1, 70.0, 90.0, 0.0, 1, 6, 0 }; //deja un dron fuera del alcance para comprobar que sobreviva
        Enemigo dentro2{ 3, Tanque, 2, 35.0, 180.0, 0.0, 3, 10, 0 }; //coloca un tanque que el pulso debe eliminar pese a su vida y su color

        insertarFinal(motor.enemigos, dentro1); //agrega el enemigo al final para conservar el orden del escenario
        insertarFinal(motor.enemigos, fuera); //agrega el enemigo al final para conservar el orden del escenario
        insertarFinal(motor.enemigos, dentro2); //agrega el enemigo al final para conservar el orden del escenario

        AccionMotor accion{ false, 0.0, false, 0, true }; //solicita el pulso sin cambiar destino ni color

        aplicarAccion(motor, accion); //aplica los cambios y la solicitud de pulso indicados

        if (!motor.pulso.activo) resultado = false; //comprueba que el pulso se active con carga completa
        if (motor.pulso.radioActual != 20.0) resultado = false; //revisa el radio inicial de veinte unidades
        if (jugador.cargaPulso != 0) resultado = false; //verifica que la activacion consuma toda la carga

        actualizarPulso(motor); //avanza el pulso activo y resuelve los enemigos que alcanza

        if (motor.pulso.radioActual != 32.0) resultado = false; //comprueba el aumento de doce unidades en el radio
        if (motor.enemigos.tam != 1) resultado = false; //verifica que solo quede el enemigo fuera de alcance

        if (motor.enemigos.cabeza == nullptr || motor.enemigos.cabeza->dato.id != 2) resultado = false; //comprueba que sobreviva el dron con identificador dos

        if (jugador.combo != 6) resultado = false; //revisa que el pulso conserve el combo previo
        if (jugador.cargaPulso != 0) resultado = false; //comprueba que las bajas del pulso no recarguen el pulso
        if (jugador.puntaje != 1000) resultado = false; //verifica los puntos del dron y el tanque usando el combo conservado

        destruirMotor(motor); //libera las listas y la copia de apariciones del motor
        destruirColaSpawn(cola); //libera las apariciones originales de la prueba
    }

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//revisa el cierre de un turno al completar la oleada o perder el nucleo y que no termine con apariciones pendientes
bool pruebaFinMotor() {
    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    {
        ColaSpawn cola; //guarda las apariciones originales que recibira el motor
        inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

        Jugador jugador{ "Prueba", false, 95, 0, 0, 0 }; //prepara un nucleo al que le faltan cinco puntos de vida

        Motor motor; //reserva el estado de la simulacion para este caso
        inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

        verificarFinMotor(motor); //evalua si el turno termina y si corresponde curar el nucleo

        if (!motor.oleadaTerminada) resultado = false; //comprueba que una cola y un escenario vacios completen la oleada
        if (!motor.turnoTerminado) resultado = false; //verifica que completar la oleada termine tambien el turno
        if (jugador.vidaNucleo != 100) resultado = false; //revisa que la curacion quede limitada a cien puntos de vida

        verificarFinMotor(motor); //evalua si el turno termina y si corresponde curar el nucleo

        if (jugador.vidaNucleo != 100) resultado = false; //comprueba que volver a revisar el fin no aumente la vida

        destruirMotor(motor); //libera las listas y la copia de apariciones del motor
        destruirColaSpawn(cola); //libera las apariciones originales de la prueba
    }

    {
        ColaSpawn cola; //guarda las apariciones originales que recibira el motor
        inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

        Spawn futuro{ 100, Dron, 0, 0, 0 }; //deja una aparicion futura que impide completar la oleada
        encolarSpawn(cola, futuro); //agrega la aparicion al final conservando el orden preparado

        Jugador jugador{ "Prueba", false, 80, 0, 0, 0 }; //prepara un jugador con vida parcial para comprobar que no se cure antes de tiempo

        Motor motor; //reserva el estado de la simulacion para este caso
        inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

        verificarFinMotor(motor); //evalua si el turno termina y si corresponde curar el nucleo

        if (motor.oleadaTerminada) resultado = false; //rechaza que la oleada termine con una aparicion pendiente
        if (motor.turnoTerminado) resultado = false; //comprueba que el turno siga activo mientras falta un enemigo
        if (jugador.vidaNucleo != 80) resultado = false; //verifica que no se aplique la curacion de fin de oleada

        destruirMotor(motor); //libera las listas y la copia de apariciones del motor
        destruirColaSpawn(cola); //libera las apariciones originales de la prueba
    }

    {
        ColaSpawn cola; //guarda las apariciones originales que recibira el motor
        inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

        Jugador jugador{ "Prueba", false, 0, 0, 0, 0 }; //prepara un jugador con el nucleo sin vida

        Motor motor; //reserva el estado de la simulacion para este caso
        inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

        verificarFinMotor(motor); //evalua si el turno termina y si corresponde curar el nucleo

        if (!motor.turnoTerminado) resultado = false; //comprueba que perder el nucleo termine el turno
        if (motor.oleadaTerminada) resultado = false; //rechaza que la derrota cuente como oleada completada
        if (jugador.vidaNucleo != 0) resultado = false; //verifica que el nucleo derrotado no reciba curacion

        destruirMotor(motor); //libera las listas y la copia de apariciones del motor
        destruirColaSpawn(cola); //libera las apariciones originales de la prueba
    }

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//comprueba que un tick aplique la accion mueva la nave y dispare sin adelantar una aparicion futura
bool pruebaProcesarTick() {
    ColaSpawn cola; //guarda las apariciones originales que recibira el motor
    inicializarColaSpawn(cola); //deja la cola vacia antes de cargar el escenario

    Spawn futuro{ 100, Dron, 0, 180, 0 }; //programa una aparicion que todavia no debe procesarse
    encolarSpawn(cola, futuro); //agrega la aparicion al final conservando el orden preparado

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara un jugador con vida completa y sin puntos combo ni carga

    Motor motor; //reserva el estado de la simulacion para este caso
    inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia propia de la cola y reinicia el combo

    AccionMotor accion{ true, 290.0, true, 2, false }; //solicita un destino y una fase nuevos sin activar el pulso

    procesarTick(motor, accion); //ejecuta un paso completo con la accion preparada

    bool resultado = true; //acumula las comprobaciones sin perder un fallo anterior

    if (motor.tickActual != 1) resultado = false; //comprueba que el contador avance despues del tick

    if (motor.nave.anguloActual != 276.0) resultado = false; //revisa el movimiento de seis grados aplicado en este paso
    if (motor.nave.anguloObjetivo != 290.0) resultado = false; //verifica que se conserve el destino solicitado
    if (motor.nave.color != 2) resultado = false; //comprueba que la nave cambie a ambar antes de disparar

    if (motor.spawns.tam != 1) resultado = false; //revisa que la aparicion futura permanezca en la cola
    if (motor.enemigos.tam != 0) resultado = false; //verifica que no se cree un enemigo antes de su tick

    if (motor.balas.tam != 1) resultado = false; //comprueba que el tick cero genere un disparo
    if (motor.balas.cabeza == nullptr) resultado = false; //rechaza que falte el nodo de la bala creada

    if (motor.balas.cabeza != nullptr && motor.balas.cabeza->dato.color != 2) resultado = false; //verifica que la bala use la nueva fase de la nave

    if (motor.oleadaTerminada) resultado = false; //comprueba que la oleada siga pendiente
    if (motor.turnoTerminado) resultado = false; //revisa que el turno no termine antes de procesar la aparicion

    destruirMotor(motor); //libera las listas y la copia de apariciones del motor
    destruirColaSpawn(cola); //libera las apariciones originales de la prueba

    return resultado; //devuelve si se cumplieron todas las comprobaciones del caso
}

//carga un script con varias acciones y una linea invalida y revisa el filtrado por jugador y tick
//comprueba tambien que liberar el script deje sus enlaces vacios
bool pruebaScriptAcciones() {
    const char* ruta = "prueba_script.txt"; //define el archivo temporal con las acciones de prueba

    {
        ofstream archivo(ruta); //abre el archivo para escribir el escenario

        archivo << "# script de prueba\n"; //agrega un comentario que el lector debe ignorar
        archivo << "Ana 1 0 COLOR M\n"; //cambia a magenta al iniciar el turno de ana
        archivo << "Ana 1 5 GIRAR_A 135\n"; //fija un giro para el tick cinco
        archivo << "Ana 1 5 COLOR A\n"; //combina el giro con un cambio a ambar en el mismo tick
        archivo << "Ana 1 10 PULSO\n"; //solicita el pulso en otro tick del turno
        archivo << "Luis 1 5 COLOR C\n"; //incluye una accion de otro jugador en el mismo tick
        archivo << "LINEA INVALIDA\n"; //introduce una linea que no debe convertirse en accion

        archivo.close(); //termina la escritura antes de cargar las acciones
    }

    ScriptAcciones script; //guarda las acciones que se leyeron del archivo
    inicializarScriptAcciones(script); //prepara la lista vacia

    bool resultado = true; //conserva los fallos encontrados durante la prueba

    if (!cargarScriptAcciones(script, ruta)) { //detecta si el archivo de prueba no se pudo abrir
        resultado = false; //marca el fallo de carga
    }

    if (script.tam != 5) { //revisa que solo se hayan agregado las cinco acciones validas
        resultado = false; //marca un conteo distinto al esperado
    }

    AccionMotor accion0 = obtenerAccionScript(script, "Ana", 1, 0); //consulta las acciones iniciales de ana

    if (!accion0.cambiarColor) resultado = false; //exige que se solicite el cambio de fase
    if (accion0.colorNuevo != 1) resultado = false; //comprueba que la fase solicitada sea magenta
    if (accion0.cambiarAngulo) resultado = false; //descarta un giro que no estaba en ese tick
    if (accion0.solicitarPulso) resultado = false; //descarta una activacion anticipada del pulso

    AccionMotor accion5 = obtenerAccionScript(script, "Ana", 1, 5); //reune el giro y el cambio de fase del tick cinco

    if (!accion5.cambiarAngulo) resultado = false; //exige que se conserve la solicitud de giro
    if (accion5.anguloObjetivo != 135.0) resultado = false; //revisa el destino indicado en el archivo
    if (!accion5.cambiarColor) resultado = false; //comprueba que el giro no haya descartado el cambio de fase
    if (accion5.colorNuevo != 2) resultado = false; //verifica que la nueva fase sea ambar
    if (accion5.solicitarPulso) resultado = false; //evita atribuir el pulso del tick diez a este momento

    AccionMotor accion10 = obtenerAccionScript(script, "Ana", 1, 10); //consulta el momento reservado para el pulso

    if (!accion10.solicitarPulso) resultado = false; //exige que la activacion aparezca en su tick

    AccionMotor accionVacia = obtenerAccionScript(script, "Ana", 1, 6); //consulta un tick sin instrucciones para ana

    if (accionVacia.cambiarAngulo) resultado = false; //comprueba que no se invente una solicitud de giro
    if (accionVacia.cambiarColor) resultado = false; //comprueba que no se invente un cambio de fase
    if (accionVacia.solicitarPulso) resultado = false; //comprueba que no se invente un pulso

    AccionMotor accionLuis = obtenerAccionScript(script, "Luis", 1, 5); //consulta por separado al otro jugador

    if (!accionLuis.cambiarColor) resultado = false; //exige que aparezca la instruccion de luis
    if (accionLuis.colorNuevo != 0) resultado = false; //revisa que use cian sin tomar el color de ana

    destruirScriptAcciones(script); //libera todas las acciones cargadas

    if (script.cabeza != nullptr) resultado = false; //comprueba que el inicio de la lista quede vacio
    if (script.cola != nullptr) resultado = false; //comprueba que tampoco quede un ultimo nodo
    if (script.tam != 0) resultado = false; //verifica que el contador se reinicie

    remove(ruta); //retira el archivo temporal de esta prueba

    return resultado; //devuelve si todas las consultas y la limpieza fueron correctas
}

//ejecuta dos turnos con el mismo script y la misma cola y compara sus resultados finales
bool pruebaModoLotesDeterminista() {
    const char* ruta = "prueba_lotes.txt"; //define el archivo temporal del turno automatico

    {
        ofstream archivo(ruta); //crea el script que usaran las dos ejecuciones
        archivo << "Auto 1 0 COLOR A\n"; //hace coincidir la fase inicial de la nave con la del enemigo
        archivo.close(); //deja el contenido listo para leerlo
    }

    ScriptAcciones script; //guarda las acciones compartidas por ambos turnos
    inicializarScriptAcciones(script); //prepara la lista antes de cargarla

    if (!cargarScriptAcciones(script, ruta)) { //detecta que no fue posible cargar el escenario
        remove(ruta); //retira el archivo antes de salir por error
        return false; //rechaza la prueba sin ejecutar turnos incompletos
    }

    ColaSpawn cola; //conserva la aparicion original para las dos ejecuciones
    inicializarColaSpawn(cola); //prepara la cola vacia

    Spawn enemigo{ 0, Dron, 2, 270, 0 }; //situa un dron ambar frente a la posicion inicial de la nave
    encolarSpawn(cola, enemigo); //programa su aparicion al comenzar el turno

    Jugador jugador1{ "Auto", false, 100, 0, 0, 0 }; //prepara al participante de la primera ejecucion
    Jugador jugador2{ "Auto", false, 100, 0, 0, 0 }; //repite los datos iniciales sin compartir el estado del jugador

    ResultadoModoLotes resultado1{}; //recibe el resumen de la primera simulacion
    ResultadoModoLotes resultado2{}; //recibe el resumen de la segunda simulacion

    bool termino1 = ejecutarModoLotes(jugador1, 1, cola, script, 1000, resultado1); //ejecuta el primer turno con un limite de mil ticks
    bool termino2 = ejecutarModoLotes(jugador2, 1, cola, script, 1000, resultado2); //repite el turno usando la misma entrada

    bool resultado = true; //mantiene los fallos de cualquiera de las comparaciones

    if (!termino1 || !termino2) resultado = false; //exige que ambos turnos terminen dentro del limite

    if (resultado1.ticksEjecutados != resultado2.ticksEjecutados) resultado = false; //compara la duracion de las dos simulaciones
    if (resultado1.vidaNucleo != resultado2.vidaNucleo) resultado = false; //revisa que quede la misma vida
    if (resultado1.puntaje != resultado2.puntaje) resultado = false; //compara los puntos obtenidos
    if (resultado1.combo != resultado2.combo) resultado = false; //revisa que el combo final coincida
    if (resultado1.cargaPulso != resultado2.cargaPulso) resultado = false; //compara la carga acumulada
    if (resultado1.oleadaTerminada != resultado2.oleadaTerminada) resultado = false; //exige el mismo estado final de oleada
    if (resultado1.turnoTerminado != resultado2.turnoTerminado) resultado = false; //exige el mismo estado final de turno

    if (!resultado1.oleadaTerminada) resultado = false; //comprueba que la oleada se haya completado
    if (!resultado1.turnoTerminado) resultado = false; //comprueba que el turno se haya cerrado

    if (resultado1.vidaNucleo != 100) resultado = false; //verifica que el nucleo termine con toda su vida
    if (resultado1.puntaje != 100) resultado = false; //revisa los puntos esperados por el dron abatido
    if (resultado1.combo != 1) resultado = false; //comprueba que se contabilice una baja consecutiva
    if (resultado1.cargaPulso != 5) resultado = false; //revisa la carga obtenida con esa baja

    if (cola.tam != 1) resultado = false; //comprueba que la cola original siga teniendo una aparicion

    destruirColaSpawn(cola); //libera la aparicion que se uso como entrada
    destruirScriptAcciones(script); //libera las acciones del escenario

    remove(ruta); //elimina el archivo temporal del script

    return resultado; //informa si se reprodujeron los resultados esperados
}

//comprueba el orden de las acciones los movimientos los choques los impactos y el pulso dentro de un tick
//compara tambien una simulacion con bitacora y otra sin ella y revisa sus metricas
bool pruebaOrdenExactoTick() {
    bool resultado = true; //acumula los fallos de los seis escenarios

    {
        ColaSpawn cola; //prepara el caso que exige mover y cambiar la fase antes de disparar
        inicializarColaSpawn(cola); //inicia la cola del escenario

        Spawn futuro{ 100, Dron, 0, 0, 0 }; //mantiene una aparicion pendiente para que el turno no termine
        encolarSpawn(cola, futuro); //reserva el enemigo para un tick posterior

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara un participante sin puntos ni carga

        Motor motor; //guarda la simulacion del escenario
        inicializarMotor(motor, &jugador, 1, cola); //inicia el turno con una copia de las apariciones

        motor.nave.anguloActual = 84.0; //situa la nave a seis grados del destino de prueba
        motor.nave.anguloObjetivo = 84.0; //deja la nave quieta hasta recibir la accion
        motor.nave.color = 0; //establece cian como fase anterior al cambio

        AccionMotor accion{ true, 90.0, true, 1, false }; //solicita llegar a noventa grados y cambiar a magenta

        procesarTick(motor, accion); //ejecuta la accion el movimiento y el disparo en un solo paso

        if (motor.nave.anguloActual != 90.0) resultado = false; //revisa que la nave alcance el nuevo destino
        if (motor.nave.color != 1) resultado = false; //comprueba que la accion haya cambiado la fase

        if (motor.balas.tam != 1) resultado = false; //exige que se haya creado el disparo del tick inicial

        NodoBala* bala = motor.balas.cabeza; //consulta la bala para revisar su estado despues del tick

        if (bala == nullptr) { //detecta que falta el disparo esperado
            resultado = false; //marca el fallo antes de intentar leer la bala
        }
        else { //revisa la bala solo cuando existe
            if (bala->dato.color != 1) resultado = false; //exige que el disparo use la fase nueva

            if (!casiIgualesOrden(bala->dato.x, 0.0)) resultado = false; //comprueba que salga sobre el eje vertical
            if (!casiIgualesOrden(bala->dato.y, 109.0)) resultado = false; //revisa que la bala ya haya avanzado nueve unidades desde la orbita

            if (!casiIgualesOrden(bala->dato.vx, 0.0)) resultado = false; //descarta movimiento horizontal en el disparo
            if (!casiIgualesOrden(bala->dato.vy, 9.0)) resultado = false; //comprueba la velocidad vertical del disparo
        }

        destruirMotor(motor); //libera las estructuras internas del turno
        destruirColaSpawn(cola); //libera la cola original del escenario
    }

    {
        ColaSpawn cola; //prepara el caso que exige crear y mover todas las apariciones vencidas en el mismo tick
        inicializarColaSpawn(cola); //deja lista la cola de apariciones

        Spawn spawn1{ 0, Dron, 0, 0, 0 }; //programa un dron para el tick inicial
        Spawn spawn2{ 0, Tanque, 1, 180, 0 }; //programa un tanque para ese mismo tick
        Spawn futuro{ 5, Dron, 2, 90, 0 }; //reserva un tercer enemigo para mas adelante

        encolarSpawn(cola, spawn1); //agrega la primera aparicion inmediata
        encolarSpawn(cola, spawn2); //agrega la segunda aparicion inmediata
        encolarSpawn(cola, futuro); //deja la aparicion futura al final

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara al jugador del escenario

        Motor motor; //guarda el estado del turno
        inicializarMotor(motor, &jugador, 1, cola); //copia las apariciones y prepara la primera oleada

        AccionMotor accion{ false, 0.0, false, 0, false }; //deja que el tick avance sin solicitudes del jugador

        procesarTick(motor, accion); //crea y mueve los enemigos que ya deben aparecer

        if (motor.enemigos.tam != 2) resultado = false; //exige que aparezcan ambos enemigos del tick cero
        if (motor.spawns.tam != 1) resultado = false; //comprueba que la aparicion futura siga pendiente

        NodoEnemigo* dron = motor.enemigos.cabeza; //consulta el primer enemigo creado
        NodoEnemigo* tanque = dron != nullptr ? dron->sig : nullptr; //busca el segundo sin desreferenciar un nodo inexistente

        if (dron == nullptr || dron->dato.tipo != Dron) { //revisa que la primera aparicion corresponda al dron
            resultado = false; //marca la ausencia o el tipo incorrecto
        }
        else { //comprueba el movimiento del dron encontrado
            if (!casiIgualesOrden(dron->dato.r, 299.12)) resultado = false; //exige que el dron ya se haya acercado al nucleo
        }

        if (tanque == nullptr || tanque->dato.tipo != Tanque) { //revisa que la segunda aparicion corresponda al tanque
            resultado = false; //marca la ausencia o el tipo incorrecto
        }
        else { //comprueba el movimiento del tanque encontrado
            if (!casiIgualesOrden(tanque->dato.r, 299.56)) resultado = false; //exige que el tanque avance segun su velocidad
        }

        destruirMotor(motor); //libera los enemigos las balas y las apariciones internas
        destruirColaSpawn(cola); //libera la cola original del escenario
    }

    {
        ColaSpawn cola; //prepara el caso que exige mover los enemigos antes de resolver los disparos
        inicializarColaSpawn(cola); //deja vacia la cola del escenario

        Spawn futuro{ 100, Dron, 0, 90, 0 }; //evita que el turno termine al eliminar al enemigo de prueba
        encolarSpawn(cola, futuro); //conserva una aparicion para despues

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //inicia los resultados del jugador en cero

        Motor motor; //guarda los objetos de este turno
        inicializarMotor(motor, &jugador, 1, cola); //prepara el motor para la primera oleada

        motor.tickActual = 1; //evita que el disparo automatico del tick cero interfiera

        Enemigo enemigo{ 1, Dron, 0, 206.5, 0.0, 0.0, 1, 6, 0 }; //coloca al dron donde la bala solo lo alcanza despues de que se mueva
        insertarFinal(motor.enemigos, enemigo); //agrega el objetivo del disparo

        Bala bala{ 191.0, 0.0, 9.0, 0.0, 0, 0 }; //prepara una bala del mismo color que avanzara hacia el dron
        insertarInicio(motor.balas, bala); //coloca la bala en el motor antes del tick

        AccionMotor accion{ false, 0.0, false, 0, false }; //mantiene los controles sin cambios

        procesarTick(motor, accion); //ejecuta el paso que debe producir el impacto

        if (motor.enemigos.tam != 0) resultado = false; //comprueba que el dron se elimine al entrar en el alcance de la bala
        if (motor.balas.tam != 0) resultado = false; //exige que la bala se consuma en el impacto

        if (jugador.puntaje != 100) resultado = false; //revisa los puntos de la baja
        if (jugador.combo != 1) resultado = false; //comprueba el aumento del combo por el acierto
        if (jugador.cargaPulso != 5) resultado = false; //revisa la carga ganada con el enemigo eliminado

        destruirMotor(motor); //libera las estructuras del turno
        destruirColaSpawn(cola); //libera la aparicion original pendiente
    }

    {
        ColaSpawn cola; //prepara el caso que exige resolver el choque con el nucleo antes de la bala
        inicializarColaSpawn(cola); //inicia la cola del escenario

        Spawn futuro{ 100, Dron, 0, 90, 0 }; //mantiene la oleada abierta despues del choque
        encolarSpawn(cola, futuro); //programa una aparicion que no interviene en este tick

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //prepara el nucleo con toda su vida

        Motor motor; //guarda el estado de la prueba
        inicializarMotor(motor, &jugador, 1, cola); //inicia el turno de la primera oleada

        motor.tickActual = 1; //omite el disparo automatico del tick inicial

        Enemigo enemigo{ 1, Dron, 0, 23.0, 0.0, 0.0, 1, 6, 0 }; //coloca al dron cerca del nucleo para que choque al avanzar
        insertarFinal(motor.enemigos, enemigo); //agrega el enemigo que debe causar dano

        Bala bala{ 13.12, 0.0, 9.0, 0.0, 0, 0 }; //prepara un disparo que llegaria al dron si el choque no se resolviera primero
        insertarInicio(motor.balas, bala); //agrega la bala antes de procesar el tick

        AccionMotor accion{ false, 0.0, false, 0, false }; //deja avanzar la simulacion sin modificar la nave

        procesarTick(motor, accion); //resuelve el avance y los choques del escenario

        if (jugador.vidaNucleo != 90) resultado = false; //exige que se aplique el dano del dron
        if (jugador.puntaje != 0) resultado = false; //comprueba que el choque no otorgue puntos por baja
        if (jugador.combo != 0) resultado = false; //comprueba que no se contabilice un acierto de bala

        if (motor.enemigos.tam != 0) resultado = false; //exige retirar al enemigo que choco

        if (motor.balas.tam != 1) resultado = false; //revisa que la bala siga presente al no encontrar al enemigo
        if (motor.balas.cabeza == nullptr) resultado = false; //comprueba que el nodo del disparo siga enlazado

        destruirMotor(motor); //libera la bala y las estructuras internas
        destruirColaSpawn(cola); //libera la cola original
    }

    {
        ColaSpawn cola; //prepara el caso que exige resolver los impactos antes de expandir el pulso
        inicializarColaSpawn(cola); //inicia la cola vacia del escenario

        Spawn futuro{ 100, Dron, 0, 90, 0 }; //evita que completar la baja termine la oleada
        encolarSpawn(cola, futuro); //deja una aparicion pendiente

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 }; //inicia los resultados del participante

        Motor motor; //guarda el estado compartido por bala y pulso
        inicializarMotor(motor, &jugador, 1, cola); //prepara el motor de la primera oleada

        motor.tickActual = 1; //evita crear un disparo automatico adicional

        motor.pulso.activo = true; //deja un pulso en curso antes de ejecutar el paso
        motor.pulso.radioActual = 188.0; //situa el pulso a una expansion de alcanzar al enemigo

        Enemigo enemigo{ 1, Dron, 0, 200.88, 0.0, 0.0, 1, 6, 0 }; //coloca un objetivo al que llegarian tanto la bala como el pulso
        insertarFinal(motor.enemigos, enemigo); //agrega el dron al escenario

        Bala bala{ 191.0, 0.0, 9.0, 0.0, 0, 0 }; //prepara el disparo correcto que debe resolver primero la baja
        insertarInicio(motor.balas, bala); //agrega el disparo al motor

        AccionMotor accion{ false, 0.0, false, 0, false }; //no solicita otra accion durante el paso

        procesarTick(motor, accion); //resuelve el disparo y despues expande el pulso

        if (motor.enemigos.tam != 0) resultado = false; //exige que el objetivo sea eliminado
        if (motor.balas.tam != 0) resultado = false; //comprueba que la baja haya consumido la bala

        if (jugador.puntaje != 100) resultado = false; //revisa que se otorguen los puntos de una unica baja
        if (jugador.combo != 1) resultado = false; //distingue una baja por bala porque incrementa el combo
        if (jugador.cargaPulso != 5) resultado = false; //distingue la carga recibida por el acierto de bala

        if (!motor.pulso.activo) resultado = false; //comprueba que el pulso siga expandiendose
        if (!casiIgualesOrden(motor.pulso.radioActual, 200.0)) resultado = false; //revisa que el radio crezca doce unidades

        destruirMotor(motor); //libera las estructuras internas de la simulacion
        destruirColaSpawn(cola); //libera la cola original pendiente
    }

    {
        ColaSpawn cola; //prepara la comparacion de un turno con bitacora y otro sin ella
        inicializarColaSpawn(cola); //inicia la entrada comun de ambos motores

        Spawn futuro{ 100, Dron, 0, 90, 0 }; //mantiene abiertos los dos turnos despues del tick
        encolarSpawn(cola, futuro); //agrega la aparicion que copiaran ambos motores

        Jugador jugadorA{ "Prueba", false, 100, 0, 0, 0 }; //prepara al jugador de la simulacion con bitacora
        Jugador jugadorB{ "Prueba", false, 100, 0, 0, 0 }; //repite el estado inicial para la simulacion sin bitacora

        BitacoraEventos bitacora; //guarda los eventos del primer motor
        inicializarBitacora(bitacora); //deja vacia la lista de eventos

        Motor motorA; //guarda el turno que registrara eventos
        Motor motorB; //guarda el turno usado para comparar los resultados

        inicializarMotor(motorA, &jugadorA, 1, cola, &bitacora); //conecta la bitacora externa al primer motor
        inicializarMotor(motorB, &jugadorB, 1, cola); //prepara el segundo motor sin registro de eventos

        motorA.tickActual = 1; //evita el disparo inicial en la simulacion con bitacora
        motorB.tickActual = 1; //usa el mismo tick en la simulacion de comparacion

        Enemigo enemigoA{ 1, Dron, 0, 206.5, 0.0, 0.0, 1, 6, 0 }; //define el objetivo del primer motor
        Enemigo enemigoB{ 1, Dron, 0, 206.5, 0.0, 0.0, 1, 6, 0 }; //repite los datos del objetivo en el segundo motor

        insertarFinal(motorA.enemigos, enemigoA); //agrega el enemigo de la simulacion con bitacora
        insertarFinal(motorB.enemigos, enemigoB); //agrega el enemigo equivalente en la otra simulacion

        Bala balaA{ 191.0, 0.0, 9.0, 0.0, 1, 0 }; //prepara un impacto de color incorrecto para el primer motor
        Bala balaB{ 191.0, 0.0, 9.0, 0.0, 1, 0 }; //repite el mismo disparo para el segundo motor

        insertarInicio(motorA.balas, balaA); //agrega la bala que generara el evento
        insertarInicio(motorB.balas, balaB); //agrega la bala equivalente sin bitacora

        AccionMotor accion{ false, 0.0, false, 0, false }; //deja los controles iguales para ambas ejecuciones

        procesarTick(motorA, accion); //ejecuta el tick registrando el impacto
        procesarTick(motorB, accion); //ejecuta el mismo tick sin registrar eventos

        if (jugadorA.vidaNucleo != jugadorB.vidaNucleo) resultado = false; //compara la vida restante con y sin bitacora
        if (jugadorA.puntaje != jugadorB.puntaje) resultado = false; //comprueba que registrar eventos no cambie el puntaje
        if (jugadorA.combo != jugadorB.combo) resultado = false; //compara el combo despues del impacto incorrecto
        if (jugadorA.cargaPulso != jugadorB.cargaPulso) resultado = false; //revisa que la carga final coincida

        if (motorA.tickActual != motorB.tickActual) resultado = false; //comprueba que ambos motores avancen el mismo tiempo
        if (motorA.enemigos.tam != motorB.enemigos.tam) resultado = false; //compara la cantidad de enemigos restantes
        if (motorA.balas.tam != motorB.balas.tam) resultado = false; //compara la cantidad de balas restantes

        if (motorA.enemigos.cabeza == nullptr || motorB.enemigos.cabeza == nullptr) { //detecta si falta el enemigo que debia sobrevivir en alguno de los motores
            resultado = false; //marca el fallo antes de consultar los nodos
        }
        else { //compara la posicion de los enemigos que sobrevivieron
            if (!casiIgualesOrden(motorA.enemigos.cabeza->dato.r, motorB.enemigos.cabeza->dato.r)) resultado = false; //exige el mismo retroceso con y sin bitacora
            if (!casiIgualesOrden(motorA.enemigos.cabeza->dato.r, 214.62)) resultado = false; //revisa la distancia final tras avanzar y recibir el impacto incorrecto
        }

        if (jugadorA.metricas.impactosErroneos != 1) resultado = false; //comprueba que el primer jugador registre un impacto incorrecto
        if (jugadorB.metricas.impactosErroneos != 1) resultado = false; //comprueba que las metricas tambien se actualicen sin bitacora

        if (jugadorA.metricas.ticksSobrevividos != 1) resultado = false; //revisa el tick sobrevivido en la primera simulacion
        if (jugadorB.metricas.ticksSobrevividos != 1) resultado = false; //revisa el mismo conteo en la segunda simulacion

        if (bitacora.tam != 1) resultado = false; //exige un unico evento por el impacto incorrecto

        destruirMotor(motorA); //libera las estructuras del primer motor sin destruir la bitacora externa
        destruirMotor(motorB); //libera las estructuras del segundo motor

        destruirBitacora(bitacora); //libera el evento registrado
        destruirColaSpawn(cola); //libera la entrada que compartieron las dos simulaciones
    }

    return resultado; //informa si todos los escenarios respetaron los resultados esperados
}

//ejecuta las comprobaciones auxiliares del motor en orden y termina al encontrar la primera que falla
bool pruebaMotorBase() {
    if (!pruebaInicializacionMotor()) return false; //detiene el grupo si el motor no conserva o reinicia correctamente sus datos
    if (!pruebaMovimientoNave()) return false; //revisa el avance de la nave hasta alcanzar su destino
    if (!pruebaCruceAngular()) return false; //comprueba el movimiento al cruzar entre cero y trescientos sesenta grados
    if (!pruebaAccionesMotor()) return false; //revisa los cambios de destino fase y activacion del pulso
    if (!pruebaSpawnsMotor()) return false; //comprueba la creacion de los enemigos programados
    if (!pruebaMovimientoEnemigos()) return false; //revisa el desplazamiento y los cambios propios de cada tipo
    if (!pruebaDisparoAutomatico()) return false; //comprueba la frecuencia la posicion y la velocidad de los disparos
    if (!pruebaChoquesEnemigos()) return false; //revisa el dano y la eliminacion de enemigos que chocan
    if (!pruebaEliminacionBalaPorRadio()) return false; //comprueba que se retiren los disparos que salen del escenario
    if (!pruebaFinMotor()) return false; //revisa las condiciones que cierran una oleada o un turno
    if (!pruebaProcesarTick()) return false; //comprueba el estado obtenido despues de un tick completo
    if (!pruebaOrdenExactoTick()) return false; //revisa el orden de operaciones y su efecto en los resultados
    if (!pruebaScriptAcciones()) return false; //comprueba la lectura y la consulta de acciones desde un archivo
    if (!pruebaModoLotesDeterminista()) return false; //revisa que entradas iguales produzcan turnos iguales

    return true; //confirma que todas las comprobaciones auxiliares pasaron
}
