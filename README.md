# Sistema Embebido de Adquisición GPS — ESP32-C3

Sistema embebido que lee datos de geolocalización en tiempo real desde un módulo GPS
(u-blox NEO-6M) conectado a un microcontrolador ESP32-C3. Lo bueno de este proyecto es su
arquitectura de dos capas: la comunicación con el hardware está escrita directamente en
Assembly RISC-V, mientras que el procesamiento de datos está en C.

## ¿Qué hace?

Desde un menú por terminal, el usuario elige qué dato consultar:

1. **Coordenadas** — Latitud y longitud en grados y minutos
2. **Altitud** — Metros sobre el nivel del mar
3. **Velocidad** — Velocidad actual en km/h

El sistema lee las tramas NMEA que envía el GPS, extrae el campo solicitado, y lo muestra
formateado. Tres LEDs indican el estado del proceso: solicitud recibida → buscando datos
→ dato listo.

## ¿Por qué es interesante?

- **Assembly real en RISC-V**: El control de GPIO (LEDs) y la lectura del FIFO de UART se
  hacen manipulando registros de hardware directamente, sin usar funciones del framework.
- **Dos puertos UART independientes**: UART0 para la interfaz de usuario y UART1 para los
  datos GPS, evitando contaminación entre ambos flujos.
- **Parsing de protocolo NMEA**: Implementación propia de un parser que recorre las tramas
  carácter por carácter, indexando campos por comas.

## Tecnologías

| Componente         | Detalle                              |
| --------------------| --------------------------------------|
| Microcontrolador   | ESP32-C3 (RISC-V, 160 MHz)           |
| Módulo GPS         | u-blox NEO-6M (NMEA 0183, 9600 baud) |
| Capa de hardware   | RISC-V Assembly (GPIO, UART FIFO)    |
| Capa de aplicación | C (parser NMEA, menú, lógica)        |
| Framework          | ESP-IDF v5.x                         |

## Arquitectura

```
┌─────────────────────────────────────────────┐
│              Application Layer (C)           │
│  NMEA frame parser · User menu · LED logic   │
├─────────────────────────────────────────────┤
│          Hardware Abstraction Layer          │
│              (RISC-V Assembly)               │
│  UART1 FIFO polling · GPIO register control  │
├──────────────┬──────────────────────────────┤
│   ESP32-C3   │  UART0 (user I/O)             │
│  Hardware    │  UART1 (GPS data)             │
│              │  GPIO 1, 3, 10 (LEDs)         │
│              │  GPIO 0 (GPS RX)              │
└──────────────┴──────────────────────────────┘
         │                    │
   [u-blox NEO-6M]        [Terminal]
```

## Estructura del proyecto

```
Parsing-GPS/
├── Codigo/
│   ├── main/
│   │   ├── main.c          # Application layer: parser, menu, control flow
│   │   └── main.S          # Assembly layer: UART FIFO polling, GPIO control
│   ├── CMakeLists.txt
│   └── sdkconfig
├── Documentacion/
│   └── Informe_trabajo_final_coloquio.pdf
├── Trabajo Práctico.pdf    # Original assignment specification
└── *.png                   # Register map reference diagrams
```

## Build & Flash

**Requisitos:** VS Code con la extensión ESP-IDF, ESP-IDF v5.x

1. Crear un proyecto nuevo desde la extensión ESP-IDF en VS Code
   - Seleccionar `esp32c3` como target
   - Seleccionar `esp32c3 -JTAG` como OpenOCD
   - Elegir el puerto USB al que se conectará la placa
   - Elegir el template más básico
2. Dentro de la carpeta `main/` del proyecto creado:
   - Reemplazar el contenido del archivo `.c` con `main.c`
   - Crear el archivo `main.S` y pegar el código
3. Conectar el ESP32-C3 por USB al puerto indicado en el paso 1
4. **Build Project** desde la barra de ESP-IDF
5. **Flash Project** (UART) para cargar el firmware al ESP32-C3
6. **Monitor Device** para abrir la terminal serie y ver el menú GPS

## Autores

Hugo J. Albarenque · Julián A. Barbero

*Ingeniería en Informática - FICH, UNL*
