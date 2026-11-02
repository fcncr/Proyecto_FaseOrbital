CXX ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra
PKG_CONFIG ?= pkg-config

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

FUENTES := Aplicacion.cpp ArgumentosCLI.cpp BitacoraEventos.cpp Bot.cpp ColaSpawn.cpp ControladorAllegro.cpp EjecutorPruebas.cpp Estadisticas.cpp FaseOrbital.cpp Generador.cpp HistorialPartidas.cpp InterfazAllegro.cpp ListaBalas.cpp ListaEnemigos.cpp ListaPuntajes.cpp ModoLotes.cpp Motor.cpp Oleada.cpp Partida.cpp Persistencia.cpp Prueba08.cpp Prueba14.cpp Pruebas.cpp PruebasBot.cpp PruebasEstadisticas.cpp PruebasListaBalas.cpp PruebasListaEnemigos.cpp PruebasMotor.cpp PruebasOleada.cpp PruebasPersistencia.cpp Ranking.cpp ReportePartida.cpp RondaJugadores.cpp ScriptAcciones.cpp VistaAllegro.cpp
FUENTES_GRAFICAS := VistaAllegro.cpp InterfazAllegro.cpp ControladorAllegro.cpp
FUENTES_CONSOLA := $(filter-out $(FUENTES_GRAFICAS),$(FUENTES))
OBJETOS_CONSOLA := $(patsubst %.cpp,build/consola/%.o,$(FUENTES_CONSOLA))
OBJETOS_GRAFICO := $(patsubst %.cpp,build/grafico/%.o,$(FUENTES))
PAQUETES_ALLEGRO := allegro-5 allegro_primitives-5 allegro_font-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5

.PHONY: all grafico pruebas clean comprobar-allegro
all: faseorbital_lotes$(EXE)
grafico: faseorbital$(EXE)

faseorbital_lotes$(EXE): $(OBJETOS_CONSOLA)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJETOS_CONSOLA) $(LDLIBS)

faseorbital$(EXE): $(OBJETOS_GRAFICO) | comprobar-allegro
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJETOS_GRAFICO) $$($(PKG_CONFIG) --libs $(PAQUETES_ALLEGRO)) $(LDLIBS)

build/consola/%.o: %.cpp
	mkdir -p build/consola
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=c++17 -DFASE_ORBITAL_SIN_GRAFICOS -MMD -MP -c $< -o $@

build/grafico/%.o: %.cpp | comprobar-allegro
	mkdir -p build/grafico
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=c++17 $$($(PKG_CONFIG) --cflags $(PAQUETES_ALLEGRO)) -MMD -MP -c $< -o $@

comprobar-allegro:
	@$(PKG_CONFIG) --exists $(PAQUETES_ALLEGRO) || { printf '%s\n' 'No se encontro pkg-config o Allegro 5 con sus addons primitives, font, ttf, audio y acodec.'; exit 1; }

pruebas: faseorbital_lotes$(EXE)
	mkdir -p build/pruebas
	cp p08_script.txt build/pruebas/p08_script.txt
	cd build/pruebas && ../../faseorbital_lotes$(EXE) --pruebas

clean:
	rm -rf build/consola build/grafico
	rm -f faseorbital_lotes$(EXE) faseorbital$(EXE)

-include $(OBJETOS_CONSOLA:.o=.d) $(OBJETOS_GRAFICO:.o=.d)