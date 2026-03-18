# Embedded GPS Data Acquisition System
### ESP32-C3 · RISC-V Assembly · C · NMEA Protocol · UART

A bare-metal hybrid embedded system that acquires and parses real-time geolocation data from a u-blox NEO-6M GPS module, built on an ESP32-C3 microcontroller using a two-layer software architecture: low-level RISC-V Assembly for direct hardware register manipulation, and C for NMEA protocol parsing and application logic.

---

## Architecture Overview

The system is intentionally split into two layers to demonstrate control at both ends of the hardware abstraction stack:

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

**Two independent UART ports** keep GPS data traffic separated from the user interface — UART1 receives raw NMEA frames from the GPS module while UART0 handles terminal I/O, preventing data contamination on the user-facing output.

---

## Hardware

| Component | Description |
|---|---|
| **MCU** | Espressif ESP32-C3 (RISC-V single-core, 160 MHz) |
| **GPS Module** | u-blox NEO-6M (NMEA 0183, 9600 baud) |
| **Interface** | UART1 @ 9600 baud, 8N1, no flow control |
| **Visual feedback** | 3-LED bar (active-low, GPIO 1 / 3 / 10) |
| **Power** | GPS powered from ESP32-C3 5V rail |

**Wiring:**

```
GPS TX  ──→  ESP32-C3 GPIO0 (UART1 RX)
GPS VCC ──→  ESP32-C3 5V
GPS GND ──→  ESP32-C3 GND

LED1 cathode ──→ GPIO1   (request received)
LED2 cathode ──→ GPIO3   (parsing in progress)
LED3 cathode ──→ GPIO10  (data ready)
All LED anodes ──→ 3.3V  (active-low logic)
```

---

## Low-Level Assembly Layer

Direct register manipulation for two peripherals, bypassing ESP-IDF abstractions entirely:

**GPIO control** — registers accessed by memory-mapped address:
```
GPIO_ENABLE_W1TS_REG  →  enable pins as output
GPIO_OUT_W1TS_REG     →  set pin HIGH (LED off, active-low)
GPIO_OUT_W1TC_REG     →  set pin LOW  (LED on)
```

**UART1 FIFO polling** — character-by-character read loop:
```
UART1_STATUS_REG  →  check RXFIFO_CNT (bits [7:0])
UART1_FIFO_REG    →  read one byte from hardware FIFO
```
Interrupts are explicitly disabled — the system uses polling, which is appropriate for the 9600 baud data rate of the NEO-6M. A polled design was chosen deliberately to expose the FIFO interaction at the register level.

---

## NMEA Parser

The C layer parses two sentence types from the GPS stream:

| Sentence | Fields extracted |
|---|---|
| `$GPGGA` | Latitude, Longitude, Cardinal direction, Altitude (MSL) |
| `$GPVTG` | Speed over ground (km/h) |

The parser iterates byte-by-byte over each received line, counting comma delimiters to index into the field at position `n`. Degrees are separated arithmetically from the raw DDDMM.MMMMM format using `strtol`, avoiding string-formatting artifacts like leading zeros.

```c
// Comma-indexed field extraction
void obtener_campo_trama(const char *frame, int field_index, char *result);

// Output example:
// Longitud W: 60 grados y 35.3569 minutos
// Latitud  S: 31 grados y 45.4159 minutos
```

Empty fields are caught before display — if the GPS hasn't acquired a satellite fix yet, the parser discards the frame and keeps polling.

---

## User Flow

```
System boot
    │
    ▼
Menu displayed on UART0
  [1] Coordinates  [2] Altitude  [3] Speed
    │
    ▼ (key press)
LED1 ON  →  request acknowledged
    │
    ▼
LED2 ON  →  polling UART1 FIFO for matching NMEA sentence
    │
    ▼
LED2 OFF, LED3 ON  →  data found, printed to terminal
    │
    ▼
Return to menu
```

---

## Build & Flash

**Requirements:** ESP-IDF v5.x, CMake, a serial terminal (e.g. `idf.py monitor`)

```bash
git clone https://github.com/JuniorAlba/Parsing-GPS.git
cd Parsing-GPS/Codigo

# Set up ESP-IDF environment (adjust path to your installation)
. $IDF_PATH/export.sh

idf.py set-target esp32c3
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## Design Decisions & Trade-offs

**Polling vs. interrupts** — At 9600 baud the MCU has ~1ms between bytes, making polling safe and predictable. An interrupt-driven design would be necessary if baud rate increased, multiple peripherals competed for CPU time, or parallel tasks were introduced via FreeRTOS.

**Dual UART** — Separating GPS traffic (UART1) from user I/O (UART0) eliminates the need to filter raw NMEA output from the terminal display. The user sees only the extracted, formatted value.

**Active-low LED wiring** — All GPIO pins available on the ESP32-C3 without JTAG reassignment are used. The active-low configuration avoids reconfiguring the debug interface pin (GPIO4).

---

## Repository Structure

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

---

## Authors

**Hugo J. Albarenque** · [LinkedIn](https://www.linkedin.com/in/Junior-Hugo-Albarenque) · [GitHub](https://github.com/JuniorAlba)  
**Julián A. Barbero**

*Computer Organization — Facultad de Ingeniería y Ciencias Hídricas, UNL*

---

## References

- [ESP32-C3 Datasheet](https://documentation.espressif.com/esp32-c3_datasheet_en.pdf)
- [ESP32-C3 Technical Reference Manual](https://documentation.espressif.com/esp32-c3_technical_reference_manual_en.pdf)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/index.html)
- [u-blox NEO-6M Protocol Specification](https://content.u-blox.com/sites/default/files/products/documents/u-blox6_ReceiverDescrProtSpec_%28GPS.G6-SW-10018%29_Public.pdf)
