## PROYECTO ESTRUCTURAS DE DATOS - GRUPO 04

## Estudiantes:
Genesis Villegas Umana - Carne: 2026018895
Amanda Torres Venegas - Carne: 2026018764
Fabian Cambronero Nunez - Carne: 2026079420


## Fase Orbital:

Desarrollamos este proyecto en C++ con Allegro 5

El programa permite jugar desde una interfaz grafica y ejecutar partidas por consola con el mismo motor

Este archivo explica como preparar las dependencias, compilar el proyecto y ejecutar los programas generados


## Requisitos para Windows:
Usamos Visual Studio Community 2026 con la carga de trabajo Desarrollo para el escritorio con C++

La instalacion debe incluir las herramientas MSVC v145 y Windows SDK

El proyecto de Visual Studio utiliza C++20 y tiene configurados los addons de Allegro para Debug y x64

Las dependencias estan declaradas en packages.config

Allegro version 5.2.11.3
AllegroDeps version 1.16.0

Para descargar estos paquetes por primera vez se necesita conexion a Internet


## Preparar el proyecto:
Extraer el archivo comprimido completo o clonar el repositorio, conservando la organizacion de sus carpetas

La carpeta del proyecto debe contener FaseOrbital.vcxproj, packages.config, los archivos .cpp y .h, el Makefile, p08_script.txt y la carpeta recursos

Si se incluye una solucion de Visual Studio, abrirla directamente

Si solo se recibio la carpeta del proyecto, crear una solucion en blanco en la carpeta superior y agregar FaseOrbital.vcxproj desde Agregar, Proyecto existente

El proyecto busca sus dependencias en ..\packages, tomando como referencia la carpeta de FaseOrbital.vcxproj

La carpeta packages debe quedar en el directorio superior, junto a la solucion


## Compilar en Visual Studio:
1 Abrir la solucion que contiene FaseOrbital

2 Desde el Explorador de soluciones, hacer clic derecho sobre la solucion y seleccionar Restaurar paquetes NuGet

3 Comprobar que se hayan restaurado Allegro 5.2.11.3 y AllegroDeps 1.16.0 en la carpeta packages

4 Seleccionar Debug como configuracion y x64 como plataforma

5 Hacer clic derecho sobre FaseOrbital y establecerlo como proyecto de inicio

6 Abrir Propiedades del proyecto, Depuracion y colocar $(ProjectDir) en Directorio de trabajo

7 Dejar vacios los argumentos del comando para iniciar la interfaz grafica

8 Seleccionar Compilar, Compilar solucion

9 Revisar la ventana Salida y comprobar que la compilacion haya terminado sin errores

10 Ejecutar con Ctrl + F5

La ventana Salida indica la ruta donde se genero FaseOrbital.exe

Esa ruta puede cambiar segun la ubicacion de la solucion, por lo que se debe utilizar la que muestre la compilacion

## Recursos necesarios:

La interfaz grafica utiliza los siguientes archivos

recursos/fuente.ttf
recursos/sonido_baja.wav
recursos/sonido_error.wav
recursos/sonido_dano.wav
recursos/sonido_pulso.wav

El programa busca estos archivos desde su directorio de trabajo

Al ejecutar desde Visual Studio con $(ProjectDir), se utiliza la carpeta recursos del proyecto


## Ejecutar fuera de Visual Studio:
Localizar FaseOrbital.exe en la ruta indicada al compilar

Copiar la carpeta completa recursos junto al ejecutable, conservando tambien la carpeta original del proyecto

Abrir PowerShell en la carpeta que contiene FaseOrbital.exe y ejecutar

.\FaseOrbital.exe;

Si Windows informa que falta una DLL, tambien debe estar disponible esa dependencia de la compilacion

Copiar recursos permite encontrar la fuente y los sonidos, pero no sustituye las bibliotecas que necesite el ejecutable


## Compilar con Makefile:
El Makefile permite generar una version sin graficos y otra con Allegro

Para utilizarlo se necesita GNU Make, g++ compatible con C++17 y una terminal con las herramientas mkdir, cp y rm

Los comandos de esta seccion se ejecutan desde la carpeta que contiene el Makefile y los archivos .cpp

Esta es otra forma de compilar el proyecto

Para trabajar desde Visual Studio no hace falta ejecutar Make

## Version sin graficos:
Ejecutar

make

Este comando genera faseorbital_lotes, o faseorbital_lotes.exe en Windows

La compilacion define FASE_ORBITAL_SIN_GRAFICOS y excluye los modulos graficos, por lo que no necesita Allegro

El ejecutable permite ejecutar partidas por lotes, consultar estadisticas y ranking y correr las pruebas

Ejemplo de ejecucion desde una terminal de Linux

./faseorbital_lotes --lotes --jugadores "BOT1,BOT2,BOT3,BOT4" --semilla 42 --oleadas 10


## Version con graficos:
Para compilar esta version se necesitan Allegro 5, sus archivos de desarrollo y pkg-config

Deben estar disponibles los siguientes modulos

allegro-5
allegro_primitives-5
allegro_font-5
allegro_ttf-5
allegro_audio-5
allegro_acodec-5

Compilar con

make grafico

Ejecutar desde la misma carpeta con

./faseorbital

En Windows, el ejecutable generado se llama faseorbital.exe

La carpeta recursos debe estar disponible en el directorio de trabajo desde el que se inicia el programa


## Comprobar la compilacion:
Para comprobar el ejecutable de Windows sin abrir la interfaz, ejecutar desde PowerShell en la carpeta de FaseOrbital.exe

.\FaseOrbital.exe --lotes --jugadores "BOT1,BOT2,BOT3,BOT4" --semilla 42 --oleadas 10;

Al terminar, el programa imprime el reporte y guarda historial.csv e historial_detalle.csv en el directorio de trabajo

Para ejecutar las pruebas con el Makefile, usar

make pruebas

Este comando compila la version sin graficos, prepara build/pruebas y copia alli p08_script.txt antes de ejecutar los casos

Desde Windows tambien se pueden ejecutar las pruebas con

.\FaseOrbital.exe --pruebas;

En ese caso, p08_script.txt debe encontrarse en la carpeta desde la que se ejecuta el comando

Las pruebas muestran PASS o FAIL y generan sus archivos en el directorio de trabajo

El resultado P14 PASS corresponde a comprobaciones logicas

La verificacion de fugas y errores de memoria requiere ademas ejecutar el programa con Valgrind o una compilacion instrumentada con AddressSanitizer


## Limpiar la compilacion del Makefile:
Para eliminar los objetos y ejecutables generados por el Makefile, ejecutar

make clean

Este comando conserva los historiales y los resultados de las pruebas