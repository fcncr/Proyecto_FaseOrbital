#include "PruebasMotor.h"
#include "Motor.h"
#include "ScriptAcciones.h"
#include <fstream>
#include <cstdio>
#include "ModoLotes.h"

static bool casiIgualesOrden(double a, double b) {
    double diferencia = a - b;

    if (diferencia < 0.0) diferencia = -diferencia;

    return diferencia < 0.001;
}


bool pruebaInicializacionMotor() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Spawn spawn{ 60, Dron, 1, 90, 0 };
    encolarSpawn(cola, spawn);

    Jugador jugador{ "Prueba", false, 80, 500, 7, 45 };

    Motor motor;
    inicializarMotor(motor, &jugador, 3, cola);

    bool resultado = true;

    if (motor.tickActual != 0) resultado = false;
    if (motor.numeroOleada != 3) resultado = false;

    if (motor.nave.anguloActual != 270.0) resultado = false;
    if (motor.nave.anguloObjetivo != 270.0) resultado = false;
    if (motor.nave.color != 0) resultado = false;

    if (motor.pulso.activo) resultado = false;
    if (motor.pulso.radioActual != 0.0) resultado = false;

    if (jugador.vidaNucleo != 80) resultado = false;
    if (jugador.puntaje != 500) resultado = false;
    if (jugador.combo != 0) resultado = false;
    if (jugador.cargaPulso != 45) resultado = false;

    if (motor.enemigos.tam != 0) resultado = false;
    if (motor.balas.tam != 0) resultado = false;

    if (motor.spawns.tam != 1) resultado = false;
    if (cola.tam != 1) resultado = false;

    if (motor.spawns.frente == cola.frente) resultado = false;

    destruirMotor(motor);

    if (cola.tam != 1) resultado = false;

    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaMovimientoNave() {
    Motor motor{};

    motor.nave.anguloActual = 270.0;
    motor.nave.anguloObjetivo = 290.0;

    actualizarNave(motor);

    if (motor.nave.anguloActual != 276.0) return false;

    actualizarNave(motor);

    if (motor.nave.anguloActual != 282.0) return false;

    actualizarNave(motor);

    if (motor.nave.anguloActual != 288.0) return false;

    actualizarNave(motor);

    if (motor.nave.anguloActual != 290.0) return false;

    return true;
}

bool pruebaCruceAngular() {
    Motor motor{};

    motor.nave.anguloActual = 358.0;
    motor.nave.anguloObjetivo = 2.0;

    actualizarNave(motor);

    if (motor.nave.anguloActual != 2.0) return false;

    motor.nave.anguloActual = 2.0;
    motor.nave.anguloObjetivo = 358.0;

    actualizarNave(motor);

    if (motor.nave.anguloActual != 358.0) return false;

    motor.nave.anguloActual = 350.0;
    motor.nave.anguloObjetivo = 20.0;

    actualizarNave(motor);

    if (motor.nave.anguloActual != 356.0) return false;

    return true;
}

bool pruebaAccionesMotor() {
    Motor motor{};

    Jugador jugador{ "Prueba", false, 100, 0, 5, 100 };

    motor.jugador = &jugador;
    motor.nave.color = 0;
    motor.pulso.activo = false;
    motor.pulso.radioActual = 0.0;

    AccionMotor accion{ true, 370.0, true, 2, true };

    aplicarAccion(motor, accion);

    if (motor.nave.anguloObjetivo != 10.0) return false;
    if (motor.nave.color != 2) return false;

    if (!motor.pulso.activo) return false;
    if (motor.pulso.radioActual != 20.0) return false;
    if (jugador.cargaPulso != 0) return false;

    return true;
}

bool pruebaSpawnsMotor() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Spawn spawn1{ 0, Dron, 0, 10, 0 };
    Spawn spawn2{ 3, Girador, 1, 350, 0 };
    Spawn spawn3{ 3, Tanque, 2, 90, 1 };
    Spawn spawn4{ 7, Camaleon, 1, 180, 0 };

    encolarSpawn(cola, spawn1);
    encolarSpawn(cola, spawn2);
    encolarSpawn(cola, spawn3);
    encolarSpawn(cola, spawn4);

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 5, cola);

    motor.tickActual = 3;
    procesarSpawns(motor);

    bool resultado = true;

    if (motor.enemigos.tam != 3) resultado = false;
    if (motor.spawns.tam != 1) resultado = false;

    NodoEnemigo* dron = motor.enemigos.cabeza;
    NodoEnemigo* girador = dron != nullptr ? dron->sig : nullptr;
    NodoEnemigo* tanque = girador != nullptr ? girador->sig : nullptr;

    if (dron == nullptr || dron->dato.id != 1 || dron->dato.tipo != Dron) resultado = false;
    if (dron != nullptr && (dron->dato.r != 300.0 || dron->dato.theta != 10.0 || dron->dato.vida != 1 || dron->dato.radio != 6)) resultado = false;

    if (girador == nullptr || girador->dato.id != 2 || girador->dato.tipo != Girador) resultado = false;
    if (girador != nullptr && (girador->dato.color != 1 || girador->dato.radio != 6)) resultado = false;
    if (girador != nullptr && (girador->dato.omega < -1.151 || girador->dato.omega > -1.149)) resultado = false;

    if (tanque == nullptr || tanque->dato.id != 3 || tanque->dato.tipo != Tanque) resultado = false;
    if (tanque != nullptr && (tanque->dato.vida != 3 || tanque->dato.radio != 10)) resultado = false;

    if (motor.enemigos.cola != tanque) resultado = false;

    motor.tickActual = 7;
    procesarSpawns(motor);

    if (motor.enemigos.tam != 4) resultado = false;
    if (!colaSpawnVacia(motor.spawns)) resultado = false;

    NodoEnemigo* camaleon = motor.enemigos.cola;

    if (camaleon == nullptr || camaleon->dato.id != 4 || camaleon->dato.tipo != Camaleon) resultado = false;
    if (camaleon != nullptr && (camaleon->dato.vida != 1 || camaleon->dato.radio != 7 || camaleon->dato.ticksColor != 0)) resultado = false;

    if (cola.tam != 4) resultado = false;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaMovimientoEnemigos() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 5, cola);

    Enemigo dron{ 1, Dron, 0, 300.0, 10.0, 0.0, 1, 6, 0 };
    Enemigo girador{ 2, Girador, 0, 300.0, 10.0, 1.15, 1, 6, 0 };
    Enemigo divisor{ 3, Divisor, 0, 300.0, 20.0, 0.0, 1, 8, 0 };
    Enemigo mini{ 4, Mini, 1, 300.0, 20.0, -1.7, 1, 4, 0 };
    Enemigo tanque{ 5, Tanque, 2, 300.0, 30.0, 0.0, 3, 10, 0 };
    Enemigo camaleon{ 6, Camaleon, 0, 300.0, 40.0, 0.0, 1, 7, 41 };

    insertarFinal(motor.enemigos, dron);
    insertarFinal(motor.enemigos, girador);
    insertarFinal(motor.enemigos, divisor);
    insertarFinal(motor.enemigos, mini);
    insertarFinal(motor.enemigos, tanque);
    insertarFinal(motor.enemigos, camaleon);

    moverEnemigos(motor);

    bool resultado = true;

    NodoEnemigo* actual = motor.enemigos.cabeza;

    if (actual == nullptr || actual->dato.r < 298.799 || actual->dato.r > 298.801) resultado = false;

    if (actual != nullptr) actual = actual->sig;

    if (actual == nullptr || actual->dato.r < 298.979 || actual->dato.r > 298.981) resultado = false;
    if (actual != nullptr && (actual->dato.theta < 11.149 || actual->dato.theta > 11.151)) resultado = false;

    if (actual != nullptr) actual = actual->sig;

    if (actual == nullptr || actual->dato.r < 299.039 || actual->dato.r > 299.041) resultado = false;

    if (actual != nullptr) actual = actual->sig;

    if (actual == nullptr || actual->dato.r < 298.499 || actual->dato.r > 298.501) resultado = false;
    if (actual != nullptr && (actual->dato.theta < 18.299 || actual->dato.theta > 18.301)) resultado = false;

    if (actual != nullptr) actual = actual->sig;

    if (actual == nullptr || actual->dato.r < 299.399 || actual->dato.r > 299.401) resultado = false;

    if (actual != nullptr) actual = actual->sig;

    if (actual == nullptr || actual->dato.r < 298.979 || actual->dato.r > 298.981) resultado = false;
    if (actual != nullptr && actual->dato.color != 1) resultado = false;
    if (actual != nullptr && actual->dato.ticksColor != 0) resultado = false;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaDisparoAutomatico() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 1, cola);

    motor.tickActual = 0;
    motor.nave.anguloActual = 0.0;
    motor.nave.color = 2;

    dispararAutomaticamente(motor);

    bool resultado = true;

    if (motor.balas.tam != 1) resultado = false;

    NodoBala* bala1 = motor.balas.cabeza;

    if (bala1 == nullptr) resultado = false;

    if (bala1 != nullptr && (bala1->dato.x < 99.999 || bala1->dato.x > 100.001)) resultado = false;
    if (bala1 != nullptr && (bala1->dato.y < -0.001 || bala1->dato.y > 0.001)) resultado = false;
    if (bala1 != nullptr && (bala1->dato.vx < 8.999 || bala1->dato.vx > 9.001)) resultado = false;
    if (bala1 != nullptr && (bala1->dato.vy < -0.001 || bala1->dato.vy > 0.001)) resultado = false;
    if (bala1 != nullptr && bala1->dato.color != 2) resultado = false;

    motor.tickActual = 1;
    dispararAutomaticamente(motor);

    if (motor.balas.tam != 1) resultado = false;

    motor.tickActual = 4;
    motor.nave.anguloActual = 90.0;
    motor.nave.color = 1;

    dispararAutomaticamente(motor);

    if (motor.balas.tam != 2) resultado = false;

    NodoBala* bala2 = motor.balas.cabeza;

    if (bala2 == nullptr) resultado = false;

    if (bala2 != nullptr && (bala2->dato.x < -0.001 || bala2->dato.x > 0.001)) resultado = false;
    if (bala2 != nullptr && (bala2->dato.y < 99.999 || bala2->dato.y > 100.001)) resultado = false;
    if (bala2 != nullptr && (bala2->dato.vx < -0.001 || bala2->dato.vx > 0.001)) resultado = false;
    if (bala2 != nullptr && (bala2->dato.vy < 8.999 || bala2->dato.vy > 9.001)) resultado = false;
    if (bala2 != nullptr && bala2->dato.color != 1) resultado = false;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaChoquesEnemigos() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Jugador jugador{ "Prueba", false, 100, 0, 9, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 1, cola);

    motor.nave.anguloActual = 0.0;

    Enemigo choqueNave{ 1, Dron, 0, 100.0, 0.0, 0.0, 1, 6, 0 };
    Enemigo nucleoDron{ 2, Dron, 0, 22.0, 180.0, 0.0, 1, 6, 0 };
    Enemigo sobreviviente{ 3, Dron, 0, 200.0, 90.0, 0.0, 1, 6, 0 };
    Enemigo nucleoTanque{ 4, Tanque, 0, 24.0, 270.0, 0.0, 3, 10, 0 };

    insertarFinal(motor.enemigos, choqueNave);
    insertarFinal(motor.enemigos, nucleoDron);
    insertarFinal(motor.enemigos, sobreviviente);
    insertarFinal(motor.enemigos, nucleoTanque);

    resolverChoquesEnemigos(motor);

    bool resultado = true;

    if (jugador.vidaNucleo != 56) resultado = false;
    if (jugador.combo != 0) resultado = false;

    if (motor.enemigos.tam != 1) resultado = false;
    if (motor.enemigos.cabeza == nullptr) resultado = false;
    if (motor.enemigos.cola == nullptr) resultado = false;

    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->dato.id != 3) resultado = false;
    if (motor.enemigos.cola != motor.enemigos.cabeza) resultado = false;

    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->ant != nullptr) resultado = false;
    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->sig != nullptr) resultado = false;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaP09() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Jugador jugador{ "Prueba", false, 100, 0, 5, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 1, cola);

    jugador.combo = 5;

    Enemigo enemigo{ 1, Dron, 0, 200.0, 0.0, 0.0, 1, 6, 0 };
    insertarFinal(motor.enemigos, enemigo);

    Bala bala{ 191.0, 0.0, 9.0, 0.0, 1, 0 };
    insertarInicio(motor.balas, bala);

    moverBalasYResolverImpactos(motor);

    bool resultado = true;

    if (motor.balas.tam != 0) resultado = false;
    if (motor.enemigos.tam != 1) resultado = false;
    if (motor.enemigos.cabeza == nullptr) resultado = false;

    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->dato.vida != 1) resultado = false;
    if (motor.enemigos.cabeza != nullptr && motor.enemigos.cabeza->dato.r != 209.0) resultado = false;

    if (jugador.combo != 0) resultado = false;
    if (jugador.puntaje != 0) resultado = false;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaP10() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 3, cola);

    motor.siguienteIdEnemigo = 4;

    Enemigo anterior{ 1, Dron, 0, 250.0, 90.0, 0.0, 1, 6, 0 };
    Enemigo divisor{ 2, Divisor, 0, 200.0, 0.0, 0.0, 1, 8, 0 };
    Enemigo siguiente{ 3, Tanque, 2, 260.0, 180.0, 0.0, 3, 10, 0 };

    insertarFinal(motor.enemigos, anterior);
    insertarFinal(motor.enemigos, divisor);
    insertarFinal(motor.enemigos, siguiente);

    Bala bala{ 191.0, 0.0, 9.0, 0.0, 0, 0 };
    insertarInicio(motor.balas, bala);

    moverBalasYResolverImpactos(motor);

    bool resultado = true;

    if (motor.balas.tam != 0) resultado = false;
    if (motor.enemigos.tam != 4) resultado = false;

    NodoEnemigo* nodoAnterior = motor.enemigos.cabeza;
    NodoEnemigo* mini1 = nodoAnterior != nullptr ? nodoAnterior->sig : nullptr;
    NodoEnemigo* mini2 = mini1 != nullptr ? mini1->sig : nullptr;
    NodoEnemigo* nodoSiguiente = mini2 != nullptr ? mini2->sig : nullptr;

    if (nodoAnterior == nullptr || nodoAnterior->dato.id != 1) resultado = false;

    if (mini1 == nullptr || mini1->dato.tipo != Mini) resultado = false;
    if (mini2 == nullptr || mini2->dato.tipo != Mini) resultado = false;

    if (mini1 != nullptr && mini1->dato.color != 1) resultado = false;
    if (mini2 != nullptr && mini2->dato.color != 2) resultado = false;

    if (mini1 != nullptr && mini1->dato.theta != 356.0) resultado = false;
    if (mini2 != nullptr && mini2->dato.theta != 4.0) resultado = false;

    if (mini1 != nullptr && mini1->dato.omega != -1.7) resultado = false;
    if (mini2 != nullptr && mini2->dato.omega != 1.7) resultado = false;

    if (mini1 != nullptr && mini1->dato.r != 200.0) resultado = false;
    if (mini2 != nullptr && mini2->dato.r != 200.0) resultado = false;

    if (nodoSiguiente == nullptr || nodoSiguiente->dato.id != 3) resultado = false;

    if (mini1 != nullptr && mini1->ant != nodoAnterior) resultado = false;
    if (mini1 != nullptr && mini1->sig != mini2) resultado = false;

    if (mini2 != nullptr && mini2->ant != mini1) resultado = false;
    if (mini2 != nullptr && mini2->sig != nodoSiguiente) resultado = false;

    if (nodoSiguiente != nullptr && nodoSiguiente->ant != mini2) resultado = false;

    if (jugador.puntaje != 200) resultado = false;
    if (jugador.combo != 1) resultado = false;
    if (jugador.cargaPulso != 5) resultado = false;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaEliminacionBalaPorRadio() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 1, cola);

    Bala bala{ 319.0, 0.0, 9.0, 0.0, 0, 0 };
    insertarInicio(motor.balas, bala);

    moverBalasYResolverImpactos(motor);

    bool resultado = motor.balas.tam == 0 && motor.balas.cabeza == nullptr;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaP11() {
    bool resultado = true;

    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Jugador jugador{ "Prueba", false, 100, 0, 4, 99 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        Enemigo enemigo{ 1, Dron, 0, 32.0, 0.0, 0.0, 1, 6, 0 };
        insertarFinal(motor.enemigos, enemigo);

        AccionMotor accion{ false, 0.0, false, 0, true };

        aplicarAccion(motor, accion);
        actualizarPulso(motor);

        if (motor.pulso.activo) resultado = false;
        if (jugador.cargaPulso != 99) resultado = false;
        if (motor.enemigos.tam != 1) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Jugador jugador{ "Prueba", false, 100, 0, 6, 100 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        jugador.combo = 6;

        Enemigo dentro1{ 1, Dron, 0, 32.0, 0.0, 0.0, 1, 6, 0 };
        Enemigo fuera{ 2, Dron, 1, 70.0, 90.0, 0.0, 1, 6, 0 };
        Enemigo dentro2{ 3, Tanque, 2, 35.0, 180.0, 0.0, 3, 10, 0 };

        insertarFinal(motor.enemigos, dentro1);
        insertarFinal(motor.enemigos, fuera);
        insertarFinal(motor.enemigos, dentro2);

        AccionMotor accion{ false, 0.0, false, 0, true };

        aplicarAccion(motor, accion);

        if (!motor.pulso.activo) resultado = false;
        if (motor.pulso.radioActual != 20.0) resultado = false;
        if (jugador.cargaPulso != 0) resultado = false;

        actualizarPulso(motor);

        if (motor.pulso.radioActual != 32.0) resultado = false;
        if (motor.enemigos.tam != 1) resultado = false;

        if (motor.enemigos.cabeza == nullptr || motor.enemigos.cabeza->dato.id != 2) resultado = false;

        if (jugador.combo != 6) resultado = false;
        if (jugador.cargaPulso != 0) resultado = false;
        if (jugador.puntaje != 1000) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    return resultado;
}

bool pruebaFinMotor() {
    bool resultado = true;

    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Jugador jugador{ "Prueba", false, 95, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        verificarFinMotor(motor);

        if (!motor.oleadaTerminada) resultado = false;
        if (!motor.turnoTerminado) resultado = false;
        if (jugador.vidaNucleo != 100) resultado = false;

        verificarFinMotor(motor);

        if (jugador.vidaNucleo != 100) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Spawn futuro{ 100, Dron, 0, 0, 0 };
        encolarSpawn(cola, futuro);

        Jugador jugador{ "Prueba", false, 80, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        verificarFinMotor(motor);

        if (motor.oleadaTerminada) resultado = false;
        if (motor.turnoTerminado) resultado = false;
        if (jugador.vidaNucleo != 80) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Jugador jugador{ "Prueba", false, 0, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        verificarFinMotor(motor);

        if (!motor.turnoTerminado) resultado = false;
        if (motor.oleadaTerminada) resultado = false;
        if (jugador.vidaNucleo != 0) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    return resultado;
}

bool pruebaProcesarTick() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Spawn futuro{ 100, Dron, 0, 180, 0 };
    encolarSpawn(cola, futuro);

    Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

    Motor motor;
    inicializarMotor(motor, &jugador, 1, cola);

    AccionMotor accion{ true, 290.0, true, 2, false };

    procesarTick(motor, accion);

    bool resultado = true;

    if (motor.tickActual != 1) resultado = false;

    if (motor.nave.anguloActual != 276.0) resultado = false;
    if (motor.nave.anguloObjetivo != 290.0) resultado = false;
    if (motor.nave.color != 2) resultado = false;

    if (motor.spawns.tam != 1) resultado = false;
    if (motor.enemigos.tam != 0) resultado = false;

    if (motor.balas.tam != 1) resultado = false;
    if (motor.balas.cabeza == nullptr) resultado = false;

    if (motor.balas.cabeza != nullptr && motor.balas.cabeza->dato.color != 2) resultado = false;

    if (motor.oleadaTerminada) resultado = false;
    if (motor.turnoTerminado) resultado = false;

    destruirMotor(motor);
    destruirColaSpawn(cola);

    return resultado;
}
bool pruebaScriptAcciones() {
    const char* ruta = "prueba_script.txt";

    {
        std::ofstream archivo(ruta);

        archivo << "# script de prueba\n";
        archivo << "Ana 1 0 COLOR M\n";
        archivo << "Ana 1 5 GIRAR_A 135\n";
        archivo << "Ana 1 5 COLOR A\n";
        archivo << "Ana 1 10 PULSO\n";
        archivo << "Luis 1 5 COLOR C\n";
        archivo << "LINEA INVALIDA\n";

        archivo.close();
    }

    ScriptAcciones script;
    inicializarScriptAcciones(script);

    bool resultado = true;

    if (!cargarScriptAcciones(script, ruta)) {
        resultado = false;
    }

    if (script.tam != 5) {
        resultado = false;
    }

    AccionMotor accion0 = obtenerAccionScript(script, "Ana", 1, 0);

    if (!accion0.cambiarColor) resultado = false;
    if (accion0.colorNuevo != 1) resultado = false;
    if (accion0.cambiarAngulo) resultado = false;
    if (accion0.solicitarPulso) resultado = false;

    AccionMotor accion5 = obtenerAccionScript(script, "Ana", 1, 5);

    if (!accion5.cambiarAngulo) resultado = false;
    if (accion5.anguloObjetivo != 135.0) resultado = false;
    if (!accion5.cambiarColor) resultado = false;
    if (accion5.colorNuevo != 2) resultado = false;
    if (accion5.solicitarPulso) resultado = false;

    AccionMotor accion10 = obtenerAccionScript(script, "Ana", 1, 10);

    if (!accion10.solicitarPulso) resultado = false;

    AccionMotor accionVacia = obtenerAccionScript(script, "Ana", 1, 6);

    if (accionVacia.cambiarAngulo) resultado = false;
    if (accionVacia.cambiarColor) resultado = false;
    if (accionVacia.solicitarPulso) resultado = false;

    AccionMotor accionLuis = obtenerAccionScript(script, "Luis", 1, 5);

    if (!accionLuis.cambiarColor) resultado = false;
    if (accionLuis.colorNuevo != 0) resultado = false;

    destruirScriptAcciones(script);

    if (script.cabeza != nullptr) resultado = false;
    if (script.cola != nullptr) resultado = false;
    if (script.tam != 0) resultado = false;

    std::remove(ruta);

    return resultado;
}
bool pruebaModoLotesDeterminista() {
    const char* ruta = "prueba_lotes.txt";

    {
        std::ofstream archivo(ruta);
        archivo << "Auto 1 0 COLOR A\n";
        archivo.close();
    }

    ScriptAcciones script;
    inicializarScriptAcciones(script);

    if (!cargarScriptAcciones(script, ruta)) {
        std::remove(ruta);
        return false;
    }

    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Spawn enemigo{ 0, Dron, 2, 270, 0 };
    encolarSpawn(cola, enemigo);

    Jugador jugador1{ "Auto", false, 100, 0, 0, 0 };
    Jugador jugador2{ "Auto", false, 100, 0, 0, 0 };

    ResultadoModoLotes resultado1{};
    ResultadoModoLotes resultado2{};

    bool termino1 = ejecutarModoLotes(jugador1, 1, cola, script, 1000, resultado1);
    bool termino2 = ejecutarModoLotes(jugador2, 1, cola, script, 1000, resultado2);

    bool resultado = true;

    if (!termino1 || !termino2) resultado = false;

    if (resultado1.ticksEjecutados != resultado2.ticksEjecutados) resultado = false;
    if (resultado1.vidaNucleo != resultado2.vidaNucleo) resultado = false;
    if (resultado1.puntaje != resultado2.puntaje) resultado = false;
    if (resultado1.combo != resultado2.combo) resultado = false;
    if (resultado1.cargaPulso != resultado2.cargaPulso) resultado = false;
    if (resultado1.oleadaTerminada != resultado2.oleadaTerminada) resultado = false;
    if (resultado1.turnoTerminado != resultado2.turnoTerminado) resultado = false;

    if (!resultado1.oleadaTerminada) resultado = false;
    if (!resultado1.turnoTerminado) resultado = false;

    if (resultado1.vidaNucleo != 100) resultado = false;
    if (resultado1.puntaje != 100) resultado = false;
    if (resultado1.combo != 1) resultado = false;
    if (resultado1.cargaPulso != 5) resultado = false;

    if (cola.tam != 1) resultado = false;

    destruirColaSpawn(cola);
    destruirScriptAcciones(script);

    std::remove(ruta);

    return resultado;
}
bool pruebaOrdenExactoTick() {
    bool resultado = true;

    // CASO 1:
    // La accion y el movimiento de la nave deben ocurrir antes del disparo.
    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Spawn futuro{ 100, Dron, 0, 0, 0 };
        encolarSpawn(cola, futuro);

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        motor.nave.anguloActual = 84.0;
        motor.nave.anguloObjetivo = 84.0;
        motor.nave.color = 0;

        AccionMotor accion{ true, 90.0, true, 1, false };

        procesarTick(motor, accion);

        if (motor.nave.anguloActual != 90.0) resultado = false;
        if (motor.nave.color != 1) resultado = false;

        if (motor.balas.tam != 1) resultado = false;

        NodoBala* bala = motor.balas.cabeza;

        if (bala == nullptr) {
            resultado = false;
        }
        else {
            if (bala->dato.color != 1) resultado = false;

            if (!casiIgualesOrden(bala->dato.x, 0.0)) resultado = false;
            if (!casiIgualesOrden(bala->dato.y, 109.0)) resultado = false;

            if (!casiIgualesOrden(bala->dato.vx, 0.0)) resultado = false;
            if (!casiIgualesOrden(bala->dato.vy, 9.0)) resultado = false;
        }

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    // CASO 2:
    // Todos los spawns vencidos deben aparecer en el mismo tick
    // y moverse durante ese mismo tick.
    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Spawn spawn1{ 0, Dron, 0, 0, 0 };
        Spawn spawn2{ 0, Tanque, 1, 180, 0 };
        Spawn futuro{ 5, Dron, 2, 90, 0 };

        encolarSpawn(cola, spawn1);
        encolarSpawn(cola, spawn2);
        encolarSpawn(cola, futuro);

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        AccionMotor accion{ false, 0.0, false, 0, false };

        procesarTick(motor, accion);

        if (motor.enemigos.tam != 2) resultado = false;
        if (motor.spawns.tam != 1) resultado = false;

        NodoEnemigo* dron = motor.enemigos.cabeza;
        NodoEnemigo* tanque = dron != nullptr ? dron->sig : nullptr;

        if (dron == nullptr || dron->dato.tipo != Dron) {
            resultado = false;
        }
        else {
            if (!casiIgualesOrden(dron->dato.r, 299.12)) resultado = false;
        }

        if (tanque == nullptr || tanque->dato.tipo != Tanque) {
            resultado = false;
        }
        else {
            if (!casiIgualesOrden(tanque->dato.r, 299.56)) resultado = false;
        }

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    // CASO 3:
    // Los enemigos deben moverse antes de que se muevan las balas.
    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Spawn futuro{ 100, Dron, 0, 90, 0 };
        encolarSpawn(cola, futuro);

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        motor.tickActual = 1;

        Enemigo enemigo{ 1, Dron, 0, 206.5, 0.0, 0.0, 1, 6, 0 };
        insertarFinal(motor.enemigos, enemigo);

        Bala bala{ 191.0, 0.0, 9.0, 0.0, 0, 0 };
        insertarInicio(motor.balas, bala);

        AccionMotor accion{ false, 0.0, false, 0, false };

        procesarTick(motor, accion);

        if (motor.enemigos.tam != 0) resultado = false;
        if (motor.balas.tam != 0) resultado = false;

        if (jugador.puntaje != 100) resultado = false;
        if (jugador.combo != 1) resultado = false;
        if (jugador.cargaPulso != 5) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    // CASO 4:
    // Los choques con el nucleo deben resolverse antes de las balas.
    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Spawn futuro{ 100, Dron, 0, 90, 0 };
        encolarSpawn(cola, futuro);

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        motor.tickActual = 1;

        Enemigo enemigo{ 1, Dron, 0, 23.0, 0.0, 0.0, 1, 6, 0 };
        insertarFinal(motor.enemigos, enemigo);

        Bala bala{ 13.12, 0.0, 9.0, 0.0, 0, 0 };
        insertarInicio(motor.balas, bala);

        AccionMotor accion{ false, 0.0, false, 0, false };

        procesarTick(motor, accion);

        if (jugador.vidaNucleo != 90) resultado = false;
        if (jugador.puntaje != 0) resultado = false;
        if (jugador.combo != 0) resultado = false;

        if (motor.enemigos.tam != 0) resultado = false;

        if (motor.balas.tam != 1) resultado = false;
        if (motor.balas.cabeza == nullptr) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    // CASO 5:
    // Las balas deben resolverse antes del pulso.
    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Spawn futuro{ 100, Dron, 0, 90, 0 };
        encolarSpawn(cola, futuro);

        Jugador jugador{ "Prueba", false, 100, 0, 0, 0 };

        Motor motor;
        inicializarMotor(motor, &jugador, 1, cola);

        motor.tickActual = 1;

        motor.pulso.activo = true;
        motor.pulso.radioActual = 188.0;

        Enemigo enemigo{ 1, Dron, 0, 200.88, 0.0, 0.0, 1, 6, 0 };
        insertarFinal(motor.enemigos, enemigo);

        Bala bala{ 191.0, 0.0, 9.0, 0.0, 0, 0 };
        insertarInicio(motor.balas, bala);

        AccionMotor accion{ false, 0.0, false, 0, false };

        procesarTick(motor, accion);

        if (motor.enemigos.tam != 0) resultado = false;
        if (motor.balas.tam != 0) resultado = false;

        if (jugador.puntaje != 100) resultado = false;
        if (jugador.combo != 1) resultado = false;
        if (jugador.cargaPulso != 5) resultado = false;

        if (!motor.pulso.activo) resultado = false;
        if (!casiIgualesOrden(motor.pulso.radioActual, 200.0)) resultado = false;

        destruirMotor(motor);
        destruirColaSpawn(cola);
    }

    // CASO 6:
    // La bitacora y las metricas no deben cambiar el resultado del gameplay.
    {
        ColaSpawn cola;
        inicializarColaSpawn(cola);

        Spawn futuro{ 100, Dron, 0, 90, 0 };
        encolarSpawn(cola, futuro);

        Jugador jugadorA{ "Prueba", false, 100, 0, 0, 0 };
        Jugador jugadorB{ "Prueba", false, 100, 0, 0, 0 };

        BitacoraEventos bitacora;
        inicializarBitacora(bitacora);

        Motor motorA;
        Motor motorB;

        inicializarMotor(motorA, &jugadorA, 1, cola, &bitacora);
        inicializarMotor(motorB, &jugadorB, 1, cola);

        motorA.tickActual = 1;
        motorB.tickActual = 1;

        Enemigo enemigoA{ 1, Dron, 0, 206.5, 0.0, 0.0, 1, 6, 0 };
        Enemigo enemigoB{ 1, Dron, 0, 206.5, 0.0, 0.0, 1, 6, 0 };

        insertarFinal(motorA.enemigos, enemigoA);
        insertarFinal(motorB.enemigos, enemigoB);

        Bala balaA{ 191.0, 0.0, 9.0, 0.0, 1, 0 };
        Bala balaB{ 191.0, 0.0, 9.0, 0.0, 1, 0 };

        insertarInicio(motorA.balas, balaA);
        insertarInicio(motorB.balas, balaB);

        AccionMotor accion{ false, 0.0, false, 0, false };

        procesarTick(motorA, accion);
        procesarTick(motorB, accion);

        if (jugadorA.vidaNucleo != jugadorB.vidaNucleo) resultado = false;
        if (jugadorA.puntaje != jugadorB.puntaje) resultado = false;
        if (jugadorA.combo != jugadorB.combo) resultado = false;
        if (jugadorA.cargaPulso != jugadorB.cargaPulso) resultado = false;

        if (motorA.tickActual != motorB.tickActual) resultado = false;
        if (motorA.enemigos.tam != motorB.enemigos.tam) resultado = false;
        if (motorA.balas.tam != motorB.balas.tam) resultado = false;

        if (motorA.enemigos.cabeza == nullptr || motorB.enemigos.cabeza == nullptr) {
            resultado = false;
        }
        else {
            if (!casiIgualesOrden(motorA.enemigos.cabeza->dato.r, motorB.enemigos.cabeza->dato.r)) resultado = false;
            if (!casiIgualesOrden(motorA.enemigos.cabeza->dato.r, 214.62)) resultado = false;
        }

        if (jugadorA.metricas.impactosErroneos != 1) resultado = false;
        if (jugadorB.metricas.impactosErroneos != 1) resultado = false;

        if (jugadorA.metricas.ticksSobrevividos != 1) resultado = false;
        if (jugadorB.metricas.ticksSobrevividos != 1) resultado = false;

        if (bitacora.tam != 1) resultado = false;

        destruirMotor(motorA);
        destruirMotor(motorB);

        destruirBitacora(bitacora);
        destruirColaSpawn(cola);
    }

    return resultado;
}

bool pruebaMotorBase() {
    if (!pruebaInicializacionMotor()) return false;
    if (!pruebaMovimientoNave()) return false;
    if (!pruebaCruceAngular()) return false;
    if (!pruebaAccionesMotor()) return false;
    if (!pruebaSpawnsMotor()) return false;
    if (!pruebaMovimientoEnemigos()) return false;
    if (!pruebaDisparoAutomatico()) return false;
    if (!pruebaChoquesEnemigos()) return false;
    if (!pruebaEliminacionBalaPorRadio()) return false;
    if (!pruebaFinMotor()) return false;
    if (!pruebaProcesarTick()) return false;
    if (!pruebaOrdenExactoTick()) return false;
    if (!pruebaScriptAcciones()) return false;
    if (!pruebaModoLotesDeterminista()) return false;

    return true;
}