#ifndef MOTOR_H
#define MOTOR_H

#include "ModelosCombate.h"
#include "ModelosJugador.h"
#include "ListaEnemigos.h"
#include "ListaBalas.h"
#include "ColaSpawn.h"
#include "BitacoraEventos.h"

struct Motor { //guarda el estado de la simulacion durante el turno de un jugador
    int tickActual; //numero del paso de simulacion que se esta procesando
    int numeroOleada; //oleada actual usada para calcular la velocidad de los enemigos

    Nave nave; //posicion angular y fase de la nave del turno
    Pulso pulso; //estado y alcance de la onda que sale del nucleo

    Jugador* jugador; //apunta al participante de la partida sin hacerse cargo de destruirlo
    BitacoraEventos* bitacora; //apunta a una bitacora externa opcional donde se guardan los eventos

    ListaEnemigos enemigos; //enemigos que ya aparecieron y siguen en el escenario
    ListaBalas balas; //disparos que todavia se estan moviendo
    ColaSpawn spawns; //copia propia de las apariciones que faltan por procesar

    int siguienteIdEnemigo; //identificador que se asignara al siguiente enemigo creado

    bool oleadaTerminada; //indica que se completo la oleada con el nucleo todavia vivo
    bool turnoTerminado; //indica que ya no se deben procesar mas ticks de este turno
};

//prepara un nuevo turno con una copia de las apariciones y reinicia el combo conservando los demas resultados del jugador
void inicializarMotor(Motor& motor, Jugador* jugador, int numeroOleada, const ColaSpawn& spawns, BitacoraEventos* bitacora = nullptr);

//libera los enemigos las balas y las apariciones sin destruir el jugador ni la bitacora externa
void destruirMotor(Motor& motor);

//quita o agrega vueltas completas para dejar el angulo entre cero incluido y trescientos sesenta excluido
double normalizarAngulo(double angulo);

//calcula el multiplicador de puntos segun el combo y lo limita entre uno y ocho
int calcularMultiplicador(int combo);

//devuelve los puntos base de cada tipo de enemigo antes de aplicar el multiplicador
int puntosBaseEnemigo(TipoEnemigo tipo);

//aplica los cambios de destino y fase y activa el pulso cuando se solicita y tiene carga completa
void aplicarAccion(Motor& motor, const AccionMotor& accion);

//acerca la nave a su angulo objetivo por el camino corto con un maximo de seis grados por tick
void actualizarNave(Motor& motor);

//crea los datos de un enemigo desde una aparicion y reserva su identificador sin insertarlo en la lista
Enemigo crearEnemigoDesdeSpawn(Motor& motor, const Spawn& spawn);

//retira de la cola todas las apariciones cuyo tick ya llego y agrega sus enemigos al escenario
void procesarSpawns(Motor& motor);

//mueve cada enemigo segun su tipo y actualiza los cambios de fase del camaleon
void moverEnemigos(Motor& motor);

//crea una bala hacia afuera cada cuatro ticks usando la posicion y la fase de la nave
void dispararAutomaticamente(Motor& motor);

//mueve cada bala resuelve su primer impacto segun el orden de la lista y retira las balas usadas o fuera del escenario
void moverBalasYResolverImpactos(Motor& motor);

//revisa los choques con nave y nucleo descuenta vida del nucleo y retira los enemigos que chocaron
void resolverChoquesEnemigos(Motor& motor);

//expande la onda activa elimina los enemigos que alcanza y la apaga cuando supera el radio permitido
void actualizarPulso(Motor& motor);

//marca el fin del turno si falta el jugador muere el nucleo o se completa la oleada y cura solo al completarla
void verificarFinMotor(Motor& motor);

//ejecuta en orden un paso de la simulacion y aumenta el tick salvo que el turno ya hubiera terminado
void procesarTick(Motor& motor, const AccionMotor& accion);

#endif
