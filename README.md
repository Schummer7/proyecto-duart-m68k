# Ensamblador-duart-m68k
# Controlador E/S por Interrupciones — DUART MC68681 (Motorola 68000)

Proyecto académico de la asignatura Arquitectura de Computadores (curso 2025–2026).

Implementación de un controlador de Entrada/Salida no bloqueante gestionado mediante interrupciones para el periférico **DUART MC68681** montado sobre un procesador **Motorola 68000** en el simulador BSVC.

## Subrutinas Implementadas

* **`INIT`**: Configura la DUART a 38400 baudios, 8 bits de datos, modo Full Duplex, vector de interrupción `$40` e inicializa los buffers circulares internos de 2000 bytes.
* **`SCAN`**: Lectura no bloqueante desde los buffers internos de recepción hacia un buffer de usuario.
* **`PRINT`**: Escritura no bloqueante desde un buffer de usuario hacia los buffers internos de transmisión, habilitando las interrupciones de TX cuando hay datos listos.
* **`RTI`**: Rutina de Tratamiento de Interrupción encargada del arbitraje y vaciado/llenado concurrente de las colas RX y TX de las líneas A y B.

## Estructura del Proyecto

* `src/es_int.s`: Código fuente principal en ensamblador Motorola 68000.
* `bib_aux.s`: Biblioteca de subrutinas auxiliares (`LEECAR`, `ESCCAR`, `INI_BUFS`).

## Requisitos de Ejecución

* Simulador BSVC configurado para MC68000.
