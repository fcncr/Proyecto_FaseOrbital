#include "InterfazAllegro.h"

#include <allegro5/allegro_primitives.h>

#include "Estadisticas.h"
#include "Ranking.h"

//reconoce los alias que comienzan con las tres letras de bot en mayusculas
static bool aliasEmpiezaConBOT(const string& alias) {
    return alias.size() >= 3 && alias[0] == 'B' && alias[1] == 'O' && alias[2] == 'T'; //comprueba el prefijo solo cuando hay al menos tres caracteres
}

//comprueba que el alias tenga contenido sin revisar sus caracteres ni si esta repetido
static bool aliasValido(const string& alias) {
    return !alias.empty(); //acepta cualquier cadena que no este vacia
}

//limpia el dibujo anterior y prepara el fondo oscuro con el marco comun de las pantallas
static void dibujarFondoInterfaz(const VistaAllegro& vista) {
    al_clear_to_color(al_map_rgb(10, 12, 22)); //borra el contenido anterior con el color de fondo

    al_draw_rectangle(80.0f, 60.0f, static_cast<float>(vista.ancho) - 80.0f, static_cast<float>(vista.alto) - 60.0f, al_map_rgb(80, 100, 150), 2.0f); //traza el marco dejando los mismos margenes respecto a la ventana
}

//dibuja el titulo recibido centrado en la parte superior de la ventana
static void dibujarTitulo(const VistaAllegro& vista, const char* titulo) {
    al_draw_text(vista.fuente, al_map_rgb(255, 255, 255), static_cast<float>(vista.ancho) / 2.0f, 100.0f, ALLEGRO_ALIGN_CENTER, titulo); //coloca el texto blanco en el centro horizontal a la altura del titulo
}

//muestra las cuatro opciones del menu y destaca la que esta seleccionada
static void dibujarMenuPrincipal(const VistaAllegro& vista, const EstadoInterfaz& estado) {
    const char* opciones[4] = { "JUGAR", "ESTADISTICAS", "RANKING", "SALIR" }; //conserva los textos en el mismo orden que las opciones del menu

    dibujarFondoInterfaz(vista); //prepara el fondo y el marco antes de escribir las opciones
    dibujarTitulo(vista, "FASE ORBITAL"); //coloca el nombre del juego en el encabezado

    for (int i = 0; i < 4; i++) { //dibuja las cuatro opciones una debajo de otra
        ALLEGRO_COLOR color; //recibira el color que corresponde a esta opcion

        if (i == estado.opcionSeleccionada) { //comprueba si esta es la opcion que el usuario tiene marcada
            color = al_map_rgb(255, 210, 30); //resalta la seleccion con el color destacado
        }
        else { //atiende las opciones que no estan seleccionadas
            color = al_map_rgb(220, 220, 220); //usa el color normal para las otras opciones
        }

        al_draw_text(vista.fuente, color, static_cast<float>(vista.ancho) / 2.0f, 220.0f + static_cast<float>(i * 45), ALLEGRO_ALIGN_CENTER, opciones[i]); //dibuja esta opcion centrada con la separacion vertical del menu
    }

    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), static_cast<float>(vista.ancho) / 2.0f, 470.0f, ALLEGRO_ALIGN_CENTER, "Flechas: seleccionar | ENTER: aceptar | ESC: salir"); //recuerda las teclas para recorrer aceptar o salir

    al_flip_display(); //presenta el menu que se acaba de dibujar
}

//muestra la cantidad de jugadores elegida y las teclas para cambiarla o continuar
static void dibujarRegistroCantidad(const VistaAllegro& vista, int cantidad) {
    dibujarFondoInterfaz(vista); //limpia la pantalla y conserva el marco del registro
    dibujarTitulo(vista, "REGISTRO DE JUGADORES"); //identifica la pantalla donde se preparan los participantes

    al_draw_textf(vista.fuente, al_map_rgb(255, 255, 255), static_cast<float>(vista.ancho) / 2.0f, 240.0f, ALLEGRO_ALIGN_CENTER, "Cantidad de jugadores: %d", cantidad); //muestra la cantidad que se ha elegido hasta ahora

    al_draw_text(vista.fuente, al_map_rgb(180, 190, 210), static_cast<float>(vista.ancho) / 2.0f, 300.0f, ALLEGRO_ALIGN_CENTER, "Izquierda / Derecha para cambiar"); //indica como aumentar o reducir la cantidad

    al_draw_text(vista.fuente, al_map_rgb(180, 190, 210), static_cast<float>(vista.ancho) / 2.0f, 325.0f, ALLEGRO_ALIGN_CENTER, "ENTER para continuar | ESC para volver"); //indica como aceptar la cantidad o cancelar el registro

    al_flip_display(); //presenta la pantalla de seleccion de cantidad
}

//muestra el alias que se esta escribiendo su tipo de jugador y cualquier mensaje de validacion
static void dibujarRegistroAlias(const VistaAllegro& vista, const EstadoInterfaz& estado, const RegistroGrafico& registro) {
    int indice = estado.jugadorRegistroActual; //identifica la posicion del participante que se esta registrando

    const string& alias = registro.aliasJugadores[indice]; //consulta el alias actual sin copiarlo ni modificarlo

    const char* tipo = aliasEmpiezaConBOT(alias) ? "BOT" : "HUMANO"; //elige la etiqueta de bot o humano segun el prefijo del alias

    dibujarFondoInterfaz(vista); //prepara el fondo antes de mostrar los datos del participante
    dibujarTitulo(vista, "REGISTRO DE JUGADORES"); //mantiene el titulo de la etapa de registro

    al_draw_textf(vista.fuente, al_map_rgb(255, 255, 255), static_cast<float>(vista.ancho) / 2.0f, 200.0f, ALLEGRO_ALIGN_CENTER, "Jugador %d de %d", indice + 1, registro.cantidadJugadores); //muestra que participante se esta completando contando desde uno

    al_draw_textf(vista.fuente, al_map_rgb(255, 210, 30), static_cast<float>(vista.ancho) / 2.0f, 250.0f, ALLEGRO_ALIGN_CENTER, "Alias: %s_", alias.c_str()); //destaca el texto escrito y agrega una marca fija al final

    al_draw_textf(vista.fuente, al_map_rgb(220, 220, 220), static_cast<float>(vista.ancho) / 2.0f, 300.0f, ALLEGRO_ALIGN_CENTER, "Tipo: %s", tipo); //muestra si ese alias corresponde a un bot o a un humano

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 350.0f, ALLEGRO_ALIGN_CENTER, "Los alias que comienzan con BOT son jugadores automaticos"); //explica el prefijo que se usa para registrar jugadores automaticos

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 375.0f, ALLEGRO_ALIGN_CENTER, "ENTER: confirmar | BACKSPACE: borrar | ESC: volver al menu"); //recuerda las teclas para confirmar borrar o cancelar

    if (!estado.mensaje.empty()) { //comprueba si existe un aviso que deba ver el usuario
        al_draw_text(vista.fuente, al_map_rgb(255, 100, 100), static_cast<float>(vista.ancho) / 2.0f, 420.0f, ALLEGRO_ALIGN_CENTER, estado.mensaje.c_str()); //dibuja el mensaje pendiente con el color de advertencia
    }

    al_flip_display(); //presenta el alias y los avisos que se acaban de dibujar
}

//presenta al jugador que va a participar junto con su oleada vida y puntaje actuales
static void dibujarAnuncioTurno(const VistaAllegro& vista, const Jugador& jugador, int numeroOleada) {
    dibujarFondoInterfaz(vista); //prepara la pantalla antes de anunciar al participante

    al_draw_text(vista.fuente, al_map_rgb(255, 210, 30), static_cast<float>(vista.ancho) / 2.0f, 160.0f, ALLEGRO_ALIGN_CENTER, "TURNO DE"); //destaca el encabezado del anuncio de turno

    al_draw_text(vista.fuente, al_map_rgb(255, 255, 255), static_cast<float>(vista.ancho) / 2.0f, 210.0f, ALLEGRO_ALIGN_CENTER, jugador.alias.c_str()); //muestra el alias del jugador al que le corresponde participar

    al_draw_textf(vista.fuente, al_map_rgb(200, 210, 230), static_cast<float>(vista.ancho) / 2.0f, 260.0f, ALLEGRO_ALIGN_CENTER, "Oleada: %d", numeroOleada); //indica la oleada que va a jugar

    al_draw_textf(vista.fuente, al_map_rgb(200, 210, 230), static_cast<float>(vista.ancho) / 2.0f, 290.0f, ALLEGRO_ALIGN_CENTER, "Vida: %d", jugador.vidaNucleo); //muestra la vida que conserva su nucleo

    al_draw_textf(vista.fuente, al_map_rgb(200, 210, 230), static_cast<float>(vista.ancho) / 2.0f, 320.0f, ALLEGRO_ALIGN_CENTER, "Puntaje: %d", jugador.puntaje); //muestra los puntos que lleva acumulados

    if (jugador.esBot) { //comprueba si este participante esta controlado por el programa
        al_draw_text(vista.fuente, al_map_rgb(255, 100, 190), static_cast<float>(vista.ancho) / 2.0f, 360.0f, ALLEGRO_ALIGN_CENTER, "BOT"); //identifica visualmente el turno automatico
    }
    else { //atiende el caso de un participante controlado por el usuario
        al_draw_text(vista.fuente, al_map_rgb(0, 220, 255), static_cast<float>(vista.ancho) / 2.0f, 360.0f, ALLEGRO_ALIGN_CENTER, "HUMANO"); //identifica visualmente el turno de un jugador humano
    }

    al_draw_text(vista.fuente, al_map_rgb(170, 180, 200), static_cast<float>(vista.ancho) / 2.0f, 420.0f, ALLEGRO_ALIGN_CENTER, "Presione ENTER para comenzar"); //indica la tecla que permite comenzar el turno

    al_flip_display(); //presenta el anuncio con los datos actuales del jugador
}

//dibuja las frecuencias con barras proporcionales a la mayor y usa una sola barra si todos los puntajes son iguales
static void dibujarHistogramaEstadisticas(const VistaAllegro& vista, const HistogramaPuntajes& histograma) {
    float inicioX = 500.0f; //marca el borde izquierdo de las barras
    float inicioY = 190.0f; //marca la altura de la primera fila del histograma normal
    float anchoMaximo = 150.0f; //reserva el ancho que tendra la barra de mayor frecuencia
    float altoBarra = 16.0f; //fija el grosor vertical de cada barra
    float separacion = 27.0f; //separa las filas para dejar espacio a sus etiquetas

    al_draw_text(vista.fuente, al_map_rgb(255, 210, 30), 560.0f, 150.0f, ALLEGRO_ALIGN_CENTER, "HISTOGRAMA DE PUNTAJES"); //identifica el grafico de distribucion de puntajes

    if (histograma.total <= 0) { //comprueba si faltan puntajes para formar el grafico
        al_draw_text(vista.fuente, al_map_rgb(180, 190, 210), 560.0f, 220.0f, ALLEGRO_ALIGN_CENTER, "Sin puntajes registrados"); //muestra el aviso cuando el histograma no contiene datos
        return; //termina sin dibujar barras vacias
    }

    if (histograma.minimo == histograma.maximo) { //detecta que todos los puntajes se concentran en un solo valor
        int frecuencia = histograma.frecuencias[0]; //consulta el conteo que se concentro en el primer intervalo

        al_draw_textf(vista.fuente, al_map_rgb(220, 220, 220), inicioX - 15.0f, 220.0f, ALLEGRO_ALIGN_RIGHT, "%.0f pts", histograma.minimo); //muestra el puntaje comun como etiqueta de la unica barra

        al_draw_rectangle(inicioX, 220.0f, inicioX + anchoMaximo, 220.0f + altoBarra, al_map_rgb(80, 100, 140), 1.0f); //dibuja el contorno completo reservado para esa barra

        if (frecuencia > 0) { //comprueba si hay puntajes contados en ese valor
            al_draw_filled_rectangle(inicioX, 220.0f, inicioX + anchoMaximo, 220.0f + altoBarra, al_map_rgb(0, 190, 230)); //llena todo el ancho porque representa la unica frecuencia
        }

        al_draw_textf(vista.fuente, al_map_rgb(255, 255, 255), inicioX + anchoMaximo + 10.0f, 220.0f, 0, "%d", frecuencia); //muestra cuantas partidas tienen ese puntaje

        return; //termina el caso de puntajes iguales sin dibujar los diez intervalos
    }

    int frecuenciaMaxima = 0; //prepara la busqueda del conteo que servira como escala

    for (int i = 0; i < 10; i++) { //revisa las diez frecuencias para encontrar la mayor
        if (histograma.frecuencias[i] > frecuenciaMaxima) { //comprueba si este intervalo supera la frecuencia encontrada
            frecuenciaMaxima = histograma.frecuencias[i]; //conserva el mayor conteo para calcular los anchos
        }
    }

    for (int i = 0; i < 10; i++) { //dibuja una fila para cada intervalo de puntajes
        float y = inicioY + separacion * static_cast<float>(i); //calcula la altura de esta fila a partir de su posicion

        double inferior = histograma.minimo + histograma.anchoIntervalo * static_cast<double>(i); //obtiene el limite inferior del intervalo actual
        double superior = inferior + histograma.anchoIntervalo; //calcula el limite superior usando el ancho del intervalo

        if (i == 9) { //comprueba si se esta dibujando el ultimo intervalo
            superior = histograma.maximo; //usa el maximo guardado para evitar diferencias por redondeo
        }

        float ancho = 0.0f; //deja la barra sin relleno hasta calcular una frecuencia representable

        if (frecuenciaMaxima > 0) { //comprueba que la escala permita dividir sin usar cero
            ancho = anchoMaximo * static_cast<float>(histograma.frecuencias[i]) / static_cast<float>(frecuenciaMaxima); //ajusta el ancho de la barra respecto a la mayor frecuencia
        }

        al_draw_textf(vista.fuente, al_map_rgb(180, 190, 210), inicioX - 15.0f, y, ALLEGRO_ALIGN_RIGHT, "%.0f - %.0f", inferior, superior); //muestra los limites del intervalo sin decimales

        al_draw_rectangle(inicioX, y, inicioX + anchoMaximo, y + altoBarra, al_map_rgb(80, 100, 140), 1.0f); //dibuja el contorno de referencia con el ancho maximo

        if (ancho > 0.0f) { //comprueba si la frecuencia produce una barra visible
            al_draw_filled_rectangle(inicioX, y, inicioX + ancho, y + altoBarra, al_map_rgb(0, 190, 230)); //rellena la parte proporcional al conteo de este intervalo
        }

        al_draw_textf(vista.fuente, al_map_rgb(255, 255, 255), inicioX + anchoMaximo + 10.0f, y, 0, "%d", histograma.frecuencias[i]); //escribe la frecuencia exacta a la derecha de la barra
    }
}

//muestra hasta los diez primeros registros del ranking recibido sin cambiar su orden
static void dibujarRankingGrafico(const VistaAllegro& vista, const Ranking& ranking) {
    dibujarFondoInterfaz(vista); //prepara el fondo de la tabla de clasificacion
    dibujarTitulo(vista, "RANKING TOP 10"); //identifica la lista de los diez primeros puestos

    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 130.0f, 155.0f, 0, "POS"); //rotula la columna de posiciones
    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 190.0f, 155.0f, 0, "JUGADOR"); //rotula la columna de alias
    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 430.0f, 155.0f, 0, "PUNTAJE"); //rotula la columna del mejor puntaje
    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 570.0f, 155.0f, 0, "PRECISION"); //rotula la columna de precision promedio

    const NodoRanking* actual = ranking.cabeza; //comienza por el primer registro de la lista ya ordenada
    int posicion = 1; //numera los puestos mostrados desde uno
    float y = 195.0f; //situa la primera fila debajo de los encabezados

    while (actual != nullptr && posicion <= 10) { //recorre hasta diez jugadores o hasta el final de la lista
        ALLEGRO_COLOR color = al_map_rgb(220, 220, 220); //prepara el color normal para los datos de esta fila

        if (posicion == 1) { //comprueba si se esta mostrando el primer puesto
            color = al_map_rgb(255, 210, 30); //destaca los datos del jugador que encabeza la lista
        }

        al_draw_textf(vista.fuente, color, 140.0f, y, ALLEGRO_ALIGN_CENTER, "%d", posicion); //muestra el numero de puesto de esta fila
        al_draw_text(vista.fuente, color, 190.0f, y, 0, actual->dato.alias.c_str()); //muestra el alias guardado en el nodo actual
        al_draw_textf(vista.fuente, color, 430.0f, y, 0, "%d", actual->dato.mejorPuntaje); //muestra el mayor puntaje historico de ese jugador
        al_draw_textf(vista.fuente, color, 570.0f, y, 0, "%.2f%%", actual->dato.precisionColor * 100.0); //convierte la precision a porcentaje y la muestra con dos decimales

        actual = actual->sig; //continua con el siguiente registro del ranking
        posicion++; //actualiza el numero de puesto que se mostrara
        y += 32.0f; //baja a la altura de la siguiente fila
    }

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 510.0f, ALLEGRO_ALIGN_CENTER, "ESC o ENTER: volver al menu"); //indica las teclas para cerrar la consulta y volver al menu

    al_flip_display(); //presenta la tabla de clasificacion completa
}

//muestra el aviso de ranking sin datos y las teclas para regresar al menu
static void dibujarRankingVacio(const VistaAllegro& vista) {
    dibujarFondoInterfaz(vista); //prepara el fondo de la consulta sin resultados
    dibujarTitulo(vista, "RANKING TOP 10"); //conserva el titulo de la pantalla de ranking

    al_draw_text(vista.fuente, al_map_rgb(220, 220, 220), static_cast<float>(vista.ancho) / 2.0f, 270.0f, ALLEGRO_ALIGN_CENTER, "No hay partidas registradas."); //informa que no hay partidas para construir la clasificacion

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 330.0f, ALLEGRO_ALIGN_CENTER, "ENTER o ESC para volver"); //indica como regresar desde la pantalla vacia

    al_flip_display(); //presenta el aviso de ranking sin datos
}

//presenta las estadisticas ya calculadas de un jugador junto con su histograma y las indicaciones de navegacion
static void dibujarEstadisticasJugadorGrafico(const VistaAllegro& vista, const EstadisticasJugador& estadisticas, int posicion, int totalJugadores) {
    dibujarFondoInterfaz(vista); //prepara el fondo antes de escribir las estadisticas
    dibujarTitulo(vista, "ESTADISTICAS HISTORICAS"); //identifica la consulta de resultados historicos

    ALLEGRO_COLOR texto = al_map_rgb(220, 220, 220); //guarda el color compartido por las cifras del resumen
    ALLEGRO_COLOR destacado = al_map_rgb(255, 210, 30); //guarda el color que destaca el alias del jugador

    al_draw_textf(vista.fuente, destacado, 130.0f, 145.0f, 0, "Jugador: %s", estadisticas.alias.c_str()); //identifica al participante al que pertenecen los datos

    al_draw_textf(vista.fuente, texto, 130.0f, 180.0f, 0, "Partidas: %d", estadisticas.partidasJugadas); //muestra cuantas partidas tiene registradas
    al_draw_textf(vista.fuente, texto, 130.0f, 210.0f, 0, "Victorias: %d", estadisticas.victorias); //muestra cuantas veces termino en primer puesto
    al_draw_textf(vista.fuente, texto, 130.0f, 240.0f, 0, "Mejor puntaje: %d", estadisticas.mejorPuntaje); //muestra su mayor puntaje historico
    al_draw_textf(vista.fuente, texto, 130.0f, 270.0f, 0, "Peor puntaje: %d", estadisticas.peorPuntaje); //muestra su menor puntaje historico
    al_draw_textf(vista.fuente, texto, 130.0f, 300.0f, 0, "Media: %.2f", estadisticas.mediaPuntaje); //presenta el promedio de puntajes con dos decimales
    al_draw_textf(vista.fuente, texto, 130.0f, 330.0f, 0, "Mediana: %.2f", estadisticas.medianaPuntaje); //presenta la mediana que ya se calculo a partir de los puntajes
    al_draw_textf(vista.fuente, texto, 130.0f, 360.0f, 0, "Desviacion: %.2f", estadisticas.desviacionEstandar); //muestra la dispersion de sus puntajes respecto al promedio
    al_draw_textf(vista.fuente, texto, 130.0f, 390.0f, 0, "Precision: %.2f%%", estadisticas.precisionColorPromedio * 100.0); //presenta la precision promedio por partida como porcentaje

    if (estadisticas.mejorColorDisponible) { //comprueba si hay impactos por color para elegir una fase
        al_draw_textf(vista.fuente, texto, 130.0f, 420.0f, 0, "Mejor color: %s", nombreColorEstadistica(estadisticas.mejorColor)); //muestra el nombre del color con mayor precision
    }
    else { //atiende la ausencia de datos suficientes para comparar colores
        al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 130.0f, 420.0f, 0, "Mejor color: no disponible"); //informa que no se pudo determinar el mejor color
    }

    if (estadisticas.tipoMasDanoDisponible) { //comprueba si se pudo identificar un tipo de enemigo con dano positivo
        al_draw_textf(vista.fuente, texto, 130.0f, 450.0f, 0, "Tipo mas dano: %s", nombreTipoEnemigoEstadistica(estadisticas.tipoMasDano)); //muestra el nombre del enemigo que acumulo mas dano al nucleo
    }
    else { //atiende la ausencia de un tipo de enemigo que se pueda mostrar
        al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 130.0f, 450.0f, 0, "Tipo mas dano: no disponible"); //informa que ese dato no esta disponible
    }

    dibujarHistogramaEstadisticas(vista, estadisticas.histograma); //dibuja la distribucion de puntajes junto al resumen numerico

    al_draw_textf(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 515.0f, ALLEGRO_ALIGN_CENTER, "Jugador %d de %d", posicion, totalJugadores); //indica el lugar de este jugador dentro de la navegacion de la consulta

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 545.0f, ALLEGRO_ALIGN_CENTER, "IZQUIERDA / DERECHA: cambiar jugador | ESC o ENTER: volver"); //recuerda como cambiar de jugador o regresar al menu

    al_flip_display(); //presenta el resumen y el histograma del jugador
}

//muestra el aviso de estadisticas sin datos y las teclas para volver al menu
static void dibujarEstadisticasVacias(const VistaAllegro& vista) {
    dibujarFondoInterfaz(vista); //prepara el fondo de la consulta sin resultados
    dibujarTitulo(vista, "ESTADISTICAS HISTORICAS"); //conserva el titulo de la pantalla de estadisticas

    al_draw_text(vista.fuente, al_map_rgb(220, 220, 220), static_cast<float>(vista.ancho) / 2.0f, 270.0f, ALLEGRO_ALIGN_CENTER, "No hay partidas registradas."); //informa que faltan partidas para calcular el resumen historico

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 330.0f, ALLEGRO_ALIGN_CENTER, "ENTER o ESC para volver"); //indica como volver al menu desde este aviso

    al_flip_display(); //presenta la pantalla de estadisticas sin datos
}

//deja la interfaz en el menu con las selecciones iniciales y sin mensajes pendientes
void inicializarEstadoInterfaz(EstadoInterfaz& estado) {
    estado.pantallaActual = PANTALLA_MENU; //marca el menu como primera pantalla
    estado.opcionSeleccionada = 0; //selecciona la primera opcion del menu
    estado.jugadorRegistroActual = 0; //prepara el registro desde el primer jugador
    estado.mensaje.clear(); //borra cualquier aviso anterior
}

//muestra el menu y devuelve la opcion elegida con enter o la salida solicitada con escape
//si se cierra la ventana tambien activa la solicitud de cerrar la aplicacion
OpcionMenuPrincipal ejecutarMenuPrincipal(VistaAllegro& vista, EstadoInterfaz& estado, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_MENU; //identifica la pantalla que se esta mostrando
    estado.opcionSeleccionada = 0; //vuelve a seleccionar jugar al entrar al menu

    al_flush_event_queue(vista.colaEventos); //descarta los eventos pendientes de la pantalla anterior
    dibujarMenuPrincipal(vista, estado); //presenta el menu antes de esperar una entrada

    while (!cerrarAplicacion) { //mantiene el menu abierto mientras no se solicite cerrar el programa
        ALLEGRO_EVENT evento; //recibira el siguiente evento de la ventana
        al_wait_for_event(vista.colaEventos, &evento); //espera una entrada sin repetir el dibujo continuamente

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta el cierre desde la ventana
            cerrarAplicacion = true; //informa al resto de la aplicacion que debe terminar
            return MENU_SALIR; //devuelve la opcion de salida junto con la solicitud de cierre
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) { //atiende las teclas usadas para navegar y confirmar
            if (evento.keyboard.keycode == ALLEGRO_KEY_UP) { //mueve la seleccion hacia la opcion anterior
                estado.opcionSeleccionada--; //retrocede una posicion en el menu

                if (estado.opcionSeleccionada < 0) { //detecta que paso por encima de la primera opcion
                    estado.opcionSeleccionada = 3; //continua desde la ultima opcion
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN) { //mueve la seleccion hacia la opcion siguiente
                estado.opcionSeleccionada++; //avanza una posicion en el menu

                if (estado.opcionSeleccionada > 3) { //detecta que paso por debajo de la ultima opcion
                    estado.opcionSeleccionada = 0; //vuelve a la primera opcion
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) { //acepta la opcion que esta resaltada
                return static_cast<OpcionMenuPrincipal>(estado.opcionSeleccionada); //convierte su posicion al valor que entiende la aplicacion
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //permite salir directamente desde el menu
                return MENU_SALIR; //entrega la opcion de salida sin cambiar la bandera de cierre de ventana
            }

            dibujarMenuPrincipal(vista, estado); //actualiza el resaltado despues de atender la tecla
        }
    }

    return MENU_SALIR; //mantiene la salida si la aplicacion ya tenia solicitado el cierre
}

//solicita de uno a seis jugadores y luego sus alias usando letras numeros y guion bajo hasta quince caracteres
//devuelve verdadero al confirmar todos los alias no vacios y falso si se cancela o se cierra la ventana
bool ejecutarRegistroJugadores(VistaAllegro& vista, EstadoInterfaz& estado, RegistroGrafico& registro, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_REGISTRO; //identifica la pantalla de ingreso de jugadores
    estado.jugadorRegistroActual = 0; //empieza por el primer participante
    estado.mensaje.clear(); //elimina los avisos de un registro anterior

    registro.cantidadJugadores = 1; //propone un jugador como cantidad inicial

    for (int i = 0; i < 6; i++) { //recorre todos los espacios disponibles para alias
        registro.aliasJugadores[i].clear(); //borra los nombres que pudieran quedar de otra partida
    }

    bool seleccionandoCantidad = true; //separa la eleccion de cantidad de la escritura de alias

    al_flush_event_queue(vista.colaEventos); //descarta entradas que pertenecian a la pantalla anterior
    dibujarRegistroCantidad(vista, registro.cantidadJugadores); //muestra primero cuantas personas o bots se van a registrar

    while (!cerrarAplicacion) { //continua el registro mientras la aplicacion siga abierta
        ALLEGRO_EVENT evento; //recibira una pulsacion o un evento de ventana
        al_wait_for_event(vista.colaEventos, &evento); //espera la siguiente entrada del usuario

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta que se cerro la ventana durante el registro
            cerrarAplicacion = true; //propaga la solicitud de terminar la aplicacion
            return false; //no confirma un registro interrumpido
        }

        if (seleccionandoCantidad) { //atiende primero la cantidad de participantes
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) { //usa pulsaciones de teclas para elegir y confirmar la cantidad
                if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) { //solicita un participante menos
                    if (registro.cantidadJugadores > 1) { //conserva al menos un jugador
                        registro.cantidadJugadores--; //reduce la cantidad dentro del limite permitido
                    }
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) { //solicita un participante mas
                    if (registro.cantidadJugadores < 6) { //respeta los seis espacios disponibles
                        registro.cantidadJugadores++; //aumenta la cantidad sin salir del arreglo
                    }
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) { //confirma cuantos alias se deben ingresar
                    seleccionandoCantidad = false; //pasa a la etapa de escritura de nombres
                    estado.jugadorRegistroActual = 0; //selecciona al primer jugador que necesita un alias
                    estado.mensaje.clear(); //deja la nueva pantalla sin avisos

                    dibujarRegistroAlias(vista, estado, registro); //muestra el campo del primer participante
                    continue; //espera otro evento antes de procesar la entrada de texto
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //cancela desde la seleccion de cantidad
                    return false; //devuelve el control al menu sin confirmar jugadores
                }

                dibujarRegistroCantidad(vista, registro.cantidadJugadores); //refleja la cantidad seleccionada despues de atender la tecla
            }

            continue; //evita tratar estos eventos como escritura de alias
        }

        if (evento.type == ALLEGRO_EVENT_KEY_CHAR) { //usa los eventos de texto para escribir borrar y confirmar un alias
            int indice = estado.jugadorRegistroActual; //ubica al participante que esta completando su nombre
            string& alias = registro.aliasJugadores[indice]; //modifica directamente el nombre guardado en el registro

            if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) { //atiende la solicitud de borrar el ultimo caracter
                if (!alias.empty()) { //comprueba que haya un caracter para quitar
                    alias.pop_back(); //borra el final del alias
                }

                estado.mensaje.clear(); //retira el aviso despues de editar el nombre
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) { //intenta confirmar el alias actual
                if (!aliasValido(alias)) { //comprueba si el participante sigue sin nombre
                    estado.mensaje = "El alias no puede estar vacio."; //explica por que no puede avanzar al siguiente jugador
                }
                else { //continua cuando el alias tiene al menos un caracter
                    estado.mensaje.clear(); //quita cualquier aviso anterior del campo

                    if (indice + 1 >= registro.cantidadJugadores) { //detecta que este era el ultimo alias pendiente
                        return true; //confirma que todos los participantes quedaron registrados
                    }

                    estado.jugadorRegistroActual++; //pasa al nombre del siguiente participante
                }
            }
            else { //revisa los caracteres de texto que no son borrar ni confirmar
                int caracter = evento.keyboard.unichar; //obtiene el caracter producido por la entrada de texto
                bool permitido = (caracter >= 'A' && caracter <= 'Z') || (caracter >= 'a' && caracter <= 'z') || (caracter >= '0' && caracter <= '9') || caracter == '_'; //acepta solo letras ascii numeros y guion bajo

                if (permitido && alias.size() < 15) { //comprueba el caracter y el limite de longitud del alias
                    alias += static_cast<char>(caracter); //agrega el caracter permitido al final del nombre
                    estado.mensaje.clear(); //retira el aviso al recibir texto valido
                }
            }

            dibujarRegistroAlias(vista, estado, registro); //actualiza el nombre el participante y el mensaje que correspondan
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //reconoce escape como una pulsacion para cancelar la escritura
            return false; //cancela el registro sin solicitar el cierre de la aplicacion
        }
    }

    return false; //no confirma el registro si la aplicacion ya tenia solicitado el cierre
}

//muestra el jugador y la oleada que siguen y espera enter para devolver el control al inicio del turno
//si se cierra la ventana marca la solicitud de cierre antes de salir de la pantalla
void mostrarAnuncioTurno(VistaAllegro& vista, EstadoInterfaz& estado, const Jugador& jugador, int numeroOleada, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_ANUNCIO_TURNO; //identifica la pantalla previa al turno
    al_flush_event_queue(vista.colaEventos); //descarta las entradas pendientes de otras pantallas
    dibujarAnuncioTurno(vista, jugador, numeroOleada); //presenta los datos del participante que va a jugar

    while (!cerrarAplicacion) { //mantiene el anuncio mientras no se solicite cerrar la aplicacion
        ALLEGRO_EVENT evento; //recibira la siguiente entrada de la ventana
        al_wait_for_event(vista.colaEventos, &evento); //espera una confirmacion o el cierre de la ventana

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta que se solicito cerrar la ventana
            cerrarAplicacion = true; //informa que la aplicacion debe terminar
            return; //sale del anuncio sin esperar la confirmacion del turno
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER) { //acepta enter como confirmacion para continuar
            return; //devuelve el control para que la aplicacion pueda iniciar el turno
        }
    }
}

//muestra un titulo y un mensaje y espera enter o escape para regresar a quien abrio la pantalla
//si el usuario cierra la ventana tambien deja solicitada la salida de la aplicacion
void mostrarPantallaInformativa(VistaAllegro& vista, EstadoInterfaz& estado, PantallaInterfaz pantalla, const char* titulo, const char* mensaje, bool& cerrarAplicacion) {
    estado.pantallaActual = pantalla; //conserva el tipo de pantalla indicado por quien la solicito
    al_flush_event_queue(vista.colaEventos); //descarta eventos que pudieran confirmar el mensaje antes de verlo

    dibujarFondoInterfaz(vista); //prepara el fondo comun de las pantallas
    dibujarTitulo(vista, titulo); //coloca el titulo recibido en su posicion habitual
    al_draw_text(vista.fuente, al_map_rgb(220, 220, 220), static_cast<float>(vista.ancho) / 2.0f, 260.0f, ALLEGRO_ALIGN_CENTER, mensaje); //muestra el mensaje centrado en la ventana
    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 380.0f, ALLEGRO_ALIGN_CENTER, "ENTER o ESC para volver"); //indica las teclas que permiten salir de esta pantalla
    al_flip_display(); //presenta el contenido que se acaba de dibujar

    while (!cerrarAplicacion) { //espera una respuesta mientras no se solicite el cierre
        ALLEGRO_EVENT evento; //recibira una entrada de teclado o de ventana
        al_wait_for_event(vista.colaEventos, &evento); //espera el siguiente evento sin volver a dibujar el mensaje

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //reconoce el cierre solicitado desde la ventana
            cerrarAplicacion = true; //propaga la solicitud al resto de la aplicacion
            return; //abandona la pantalla informativa
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) { //revisa las teclas que pueden cerrar el mensaje
            if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER || evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //acepta cualquiera de las dos teclas indicadas
                return; //regresa sin cambiar la solicitud de cierre de la aplicacion
            }
        }
    }
}

//muestra los puntos y la vida de todos los participantes al terminar la oleada e identifica a los eliminados
//espera enter para continuar o informa el cierre de la ventana
void mostrarResumenOleada(VistaAllegro& vista, EstadoInterfaz& estado, const Partida& partida, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_RESUMEN; //identifica la pantalla que se va a mostrar

    al_flush_event_queue(vista.colaEventos); //descarta entradas pendientes de la pantalla anterior
    dibujarFondoInterfaz(vista); //prepara el fondo y el marco del resumen
    al_draw_textf(vista.fuente, al_map_rgb(255, 210, 30), static_cast<float>(vista.ancho) / 2.0f, 80.0f, ALLEGRO_ALIGN_CENTER, "OLEADA %d COMPLETADA", partida.numeroRonda); //muestra el numero de la oleada que acaba de terminar

    float y = 150.0f; //situa la primera fila de participantes

    for (int i = 0; i < partida.cantidadJugadores; i++) { //incluye a todos los jugadores en el orden de registro
        const Jugador& jugador = partida.jugadores[i]; //consulta sus datos sin modificarlos
        ALLEGRO_COLOR color = jugador.eliminado ? al_map_rgb(170, 90, 90) : al_map_rgb(220, 220, 220); //distingue con otro color a los participantes eliminados

        al_draw_textf(vista.fuente, color, 150.0f, y, 0, "%s", jugador.alias.c_str()); //escribe el alias al inicio de la fila
        al_draw_textf(vista.fuente, color, 330.0f, y, 0, "Puntos: %d", jugador.puntaje); //muestra los puntos acumulados hasta esta oleada
        al_draw_textf(vista.fuente, color, 500.0f, y, 0, "Vida: %d", jugador.vidaNucleo); //muestra la vida restante del nucleo

        if (jugador.eliminado) { //comprueba si el participante ya quedo fuera de la partida
            al_draw_text(vista.fuente, al_map_rgb(255, 80, 80), 650.0f, y, 0, "ELIMINADO"); //agrega el aviso al final de su fila
        }

        y += 45.0f; //deja espacio para el siguiente participante
    }

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 520.0f, ALLEGRO_ALIGN_CENTER, "ENTER para continuar"); //indica como salir del resumen
    al_flip_display(); //presenta en la ventana todo lo dibujado

    while (!cerrarAplicacion) { //mantiene el resumen hasta recibir una accion de salida
        ALLEGRO_EVENT evento; //recibira el siguiente evento de la ventana
        al_wait_for_event(vista.colaEventos, &evento); //espera sin avanzar la simulacion

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta que el usuario cerro la ventana
            cerrarAplicacion = true; //comunica el cierre a la aplicacion
            return; //termina de mostrar el resumen
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER) { //reconoce la tecla para continuar
            return; //devuelve el control sin solicitar el cierre de la aplicacion
        }
    }
}

//presenta los puestos finales ya asignados y consulta el ganador usando las precisiones guardadas
//conserva los puestos compartidos y espera enter o el cierre de la ventana
void mostrarFinPartida(VistaAllegro& vista, EstadoInterfaz& estado, const Partida& partida, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_GAME_OVER; //identifica la pantalla de resultados finales

    al_flush_event_queue(vista.colaEventos); //descarta entradas pendientes del juego
    dibujarFondoInterfaz(vista); //prepara el fondo y el marco de resultados
    al_draw_text(vista.fuente, al_map_rgb(255, 210, 30), static_cast<float>(vista.ancho) / 2.0f, 70.0f, ALLEGRO_ALIGN_CENTER, "FIN DE PARTIDA"); //muestra el titulo centrado

    float y = 150.0f; //establece donde comienza la lista de puestos

    for (int posicion = 1; posicion <= partida.cantidadJugadores; posicion++) { //recorre los puestos posibles desde el primero
        for (int i = 0; i < partida.cantidadJugadores; i++) { //busca todos los jugadores que comparten ese puesto
            const Jugador& jugador = partida.jugadores[i]; //consulta al participante sin cambiar el arreglo

            if (jugador.metricas.posicionFinal == posicion) { //selecciona los jugadores que pertenecen al puesto actual
                al_draw_textf(vista.fuente, al_map_rgb(230, 230, 230), static_cast<float>(vista.ancho) / 2.0f, y, ALLEGRO_ALIGN_CENTER, "%d. %s   %d pts", posicion, jugador.alias.c_str(), jugador.puntaje); //muestra el puesto el alias y los puntos finales
                y += 40.0f; //reserva otra fila incluso cuando el puesto esta compartido
            }
        }
    }

    double precisiones[6]; //reune las precisiones que se usaran para desempatar al ganador

    for (int i = 0; i < partida.cantidadJugadores; i++) { //copia solo los valores de los participantes registrados
        precisiones[i] = partida.jugadores[i].metricas.precisionColor; //usa la precision que ya esta guardada en sus metricas
    }

    const Jugador* ganador = determinarGanadorPartida(partida, precisiones); //consulta el ganador sin modificar la partida

    if (ganador != nullptr) { //comprueba si los criterios identificaron un ganador unico
        al_draw_textf(vista.fuente, al_map_rgb(0, 220, 255), static_cast<float>(vista.ancho) / 2.0f, 430.0f, ALLEGRO_ALIGN_CENTER, "Ganador: %s", ganador->alias.c_str()); //muestra el nombre del ganador
    }
    else { //atiende el caso en que no se obtuvo un ganador unico
        al_draw_text(vista.fuente, al_map_rgb(0, 220, 255), static_cast<float>(vista.ancho) / 2.0f, 430.0f, ALLEGRO_ALIGN_CENTER, "Ganador: empate"); //muestra el mensaje de empate previsto por la pantalla
    }

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 500.0f, ALLEGRO_ALIGN_CENTER, "ENTER para volver al menu"); //indica como salir de los resultados
    al_flip_display(); //presenta el contenido completo de la pantalla

    while (!cerrarAplicacion) { //mantiene los resultados visibles hasta recibir una salida
        ALLEGRO_EVENT evento; //recibira el siguiente evento
        al_wait_for_event(vista.colaEventos, &evento); //espera una entrada del usuario o de la ventana

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //reconoce el cierre de la ventana
            cerrarAplicacion = true; //solicita terminar la aplicacion
            return; //abandona la pantalla de resultados
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER) { //reconoce la tecla para volver al menu
            return; //deja que la aplicacion continue con su menu
        }
    }
}

//permite consultar las estadisticas de cada jugador siguiendo el orden del ranking historico
//las flechas recorren los jugadores en ambos sentidos y enter o escape terminan la consulta
void mostrarEstadisticasHistoricas(VistaAllegro& vista, EstadoInterfaz& estado, const HistorialPartidas& historial, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_ESTADISTICAS; //identifica la consulta historica que se va a mostrar

    Ranking ranking; //reune una entrada por jugador para ordenar la navegacion
    inicializarRanking(ranking); //prepara la lista antes de construirla
    construirRankingDesdeHistorial(historial, ranking); //obtiene los jugadores y su orden a partir de las partidas guardadas

    al_flush_event_queue(vista.colaEventos); //descarta entradas pendientes de la pantalla anterior

    if (ranking.tam == 0) { //comprueba si el historial no contiene jugadores para consultar
        dibujarEstadisticasVacias(vista); //muestra el aviso de ausencia de partidas

        while (!cerrarAplicacion) { //espera a que el usuario salga de este aviso
            ALLEGRO_EVENT evento; //recibira el siguiente evento
            al_wait_for_event(vista.colaEventos, &evento); //espera una entrada sin recalcular estadisticas

            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta que se cerro la ventana
                cerrarAplicacion = true; //comunica que debe terminar la aplicacion
                break; //sale del ciclo para completar la limpieza
            }

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) { //revisa las teclas que pueden cerrar el aviso
                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER || evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //acepta cualquiera de las dos formas de volver
                    break; //termina la espera sin pedir el cierre de la aplicacion
                }
            }
        }

        destruirRanking(ranking); //deja liberada la lista temporal antes de salir
        return; //termina porque no hay estadisticas que recorrer
    }

    int posicion = 1; //empieza por el primer jugador del ranking

    while (!cerrarAplicacion) { //mantiene la consulta mientras no se solicite salir
        const RegistroRanking* registro = obtenerPosicionRanking(ranking, posicion); //busca al jugador seleccionado sin copiar su registro

        if (registro == nullptr) { //comprueba si la posicion no tiene un registro disponible
            break; //sale del recorrido y libera el ranking al final
        }

        EstadisticasJugador estadisticas; //recibira el resumen historico del jugador seleccionado

        if (!calcularEstadisticasJugador(historial, registro->alias, estadisticas)) { //calcula sus resultados a partir del historial
            break; //termina la consulta si no se pudieron obtener sus partidas
        }

        dibujarEstadisticasJugadorGrafico(vista, estadisticas, posicion, ranking.tam); //muestra las metricas el histograma y la posicion dentro de la consulta

        ALLEGRO_EVENT evento; //recibira la siguiente entrada de navegacion
        al_wait_for_event(vista.colaEventos, &evento); //espera antes de volver a dibujar o cambiar de jugador

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //reconoce el cierre de la ventana
            cerrarAplicacion = true; //solicita terminar la aplicacion
            break; //deja pendiente solo la liberacion del ranking
        }

        if (evento.type != ALLEGRO_EVENT_KEY_DOWN) { //descarta los eventos que no son una tecla presionada
            continue; //vuelve al inicio de la consulta con el mismo jugador
        }

        if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) { //solicita avanzar al siguiente jugador
            posicion++; //mueve la seleccion un puesto hacia adelante

            if (posicion > ranking.tam) { //detecta que se paso del ultimo jugador
                posicion = 1; //reinicia el recorrido por el primero
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) { //solicita consultar al jugador anterior
            posicion--; //mueve la seleccion un puesto hacia atras

            if (posicion < 1) { //detecta que se paso del primer jugador
                posicion = ranking.tam; //continua desde el ultimo
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER || evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //reconoce las teclas para volver al menu
            break; //termina la consulta antes de liberar la lista
        }
    }

    destruirRanking(ranking); //libera los nodos creados solo para navegar por las estadisticas
}

//muestra los primeros diez puestos del historial o un aviso si no hay resultados
//espera enter escape o el cierre de la ventana y libera el ranking antes de salir
void mostrarRankingGrafico(VistaAllegro& vista, EstadoInterfaz& estado, const HistorialPartidas& historial, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_RANKING; //identifica la pantalla de clasificacion historica

    Ranking ranking; //guardara los jugadores ordenados por sus resultados historicos
    inicializarRanking(ranking); //prepara la lista vacia antes de construirla
    construirRankingDesdeHistorial(historial, ranking); //reune el mejor puntaje y la precision promedio de cada alias

    al_flush_event_queue(vista.colaEventos); //descarta entradas pendientes del menu anterior

    if (ranking.tam == 0) { //comprueba si no hay participantes en el historial
        dibujarRankingVacio(vista); //muestra el aviso de ausencia de partidas
    }
    else { //atiende el historial que tiene jugadores registrados
        dibujarRankingGrafico(vista, ranking); //dibuja hasta diez jugadores respetando el orden de la lista
    }

    while (!cerrarAplicacion) { //mantiene la pantalla hasta recibir una accion de salida
        ALLEGRO_EVENT evento; //recibira la siguiente entrada
        al_wait_for_event(vista.colaEventos, &evento); //espera sin modificar la clasificacion mostrada

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta el cierre de la ventana
            cerrarAplicacion = true; //comunica que la aplicacion debe terminar
            break; //sale del ciclo para liberar la lista temporal
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) { //revisa las teclas usadas para abandonar la consulta
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE || evento.keyboard.keycode == ALLEGRO_KEY_ENTER) { //acepta escape o enter para volver al menu
                break; //termina la espera sin solicitar el cierre de la aplicacion
            }
        }
    }
    destruirRanking(ranking); //libera los nodos del ranking temporal
}
