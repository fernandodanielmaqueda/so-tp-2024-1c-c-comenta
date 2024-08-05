# Trabajo Práctico de Sistemas Operativos - 1er Cuatrimestre 2024

## Grupo "Operativos"

| Apellido y Nombre(s)       | Usuario de GitHub |
|----------------------------|-------------|-----------------|
| MAQUEDA, Fernando Daniel   | [@fernandodanielmaqueda](https://github.com/fernandodanielmaqueda) |
| FINK, Brian                | [@Brai93](https://github.com/Brai93) |
| ARANZAMENDI, Iñaki Joaquín | [@inaaranza](https://github.com/inaaranza) |
| MORISINI, Pablo            | [@MorosiniP](https://github.com/MorosiniP) |
| BENCINA, Morena            | [@MoreBencina](https://github.com/MoreBencina) |

## Prerequisitos

Sistema operativo tipo Unix (probado en Ubuntu Server)

## Dependencias

Para poder compilar y ejecutar el proyecto, es necesario tener instalada la
biblioteca [so-commons-library] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-commons-library
cd so-commons-library
make debug
make install
```

## Compilación

Cada módulo del proyecto se compila de forma independiente a través de un único
archivo `GNUmakefile`. Para compilar todos los módulos, es necesario ejecutar el comando
`make`.

El ejecutable resultante se guardará en la carpeta `bin` correspondiente al módulo.

## Importar desde Visual Studio Code

Para importar el workspace, debemos abrir el archivo `tp.code-workspace` desde
la interfaz o ejecutando el siguiente comando desde la carpeta raíz del
repositorio:

```bash
code tp.code-workspace
```

[so-commons-library]: https://github.com/sisoputnfrba/so-commons-library
