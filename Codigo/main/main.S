# .equ UART0_BASE, 0x60000000
# .equ UART1_BASE, 0x60010000
# .equ UART0_FIFO, UART0_BASE + 0x0000
# .equ UART1_FIFO, UART1_BASE + 0x0000
# .equ UART0_STATUS_REG, UART0_BASE + 0x001C
# .equ UART1_STATUS_REG, UART1_BASE + 0x001C
# #Con estos registros tenemos todo listo para operar el puerto UART
# .equ GPIO_BASE, 0x60004000      #Espacio de memoria de los GPIO
# .equ GPIO_ENABLE_REG, GPIO_BASE + 0x0020        #Registro que nos dice que GPIO estan configurados como salida
# .equ GPIO_ENABLE_W1TS_REG, GPIO_BASE + 0x0024   #Registro que permite configurar un GPIO para salida de datos
# .equ GPIO_ENABLE_W1TC_REG, GPIO_BASE + 0x0028   #Registro que permite deshabilitar un GPIO para salida de datos (ponerlo como entrada)

# .equ GPIO_OUT_REG, GPIO_BASE + 0x0004           #Registro que nos dice que datos de salida hay en cada GPIO
# .equ GPIO_OUT_W1TS_REG, GPIO_BASE + 0x0008      #Registro que nos permite setear un 1 en la salida de un GPIO
# .equ GPIO_OUT_W1TC_REG, GPIO_BASE + 0x000C      #Registro que nos permite setear un 0 en la salida de un GPIO

# .equ GPIO0_TXGPS, (1)
# .equ GPIO1_RXGPS, (1<<1)
# .equ GPIO3_LED1, (1<<3)
# .equ GPIO10_LED2, (1<<10)
# .equ GPIO4_LED3, (1<<4)


# .equ UART_RXFIFO_CNT_MASK, 0x000003FF
# .equ UART_TXFIFO_CNT_MASK, 0x03FF0000

# .global prueba_gps    # Exportamos la etiqueta para que C la encuentre
# .global prueba_leds
# .text               # Indicamos que acá arranca el código ejecutable
# prueba_leds:
#     li t0, GPIO_ENABLE_W1TS_REG
#     li t1, GPIO3_LED1 | GPIO4_LED3 | GPIO10_LED2
#     sw t1,0(t0)
#     li t2,GPIO_OUT_W1TC_REG
#     sw t1, 0(t2)
#     ret
# prueba_gps:
#     li t0, UART1_STATUS_REG
#     lw t2, 0(t0)

#     andi t2, t2, UART_RXFIFO_CNT_MASK
#     beqz t2, bucle_fin

#     li t1, UART0_FIFO
#     li t3, UART1_FIFO

# bucle:
#     lbu t4, 0(t3)           # Leemos un byte del GPS
#     sb t4, 0(t1)            # Lo mandamos a la consola
#     addi t2, t2, -1
#     bnez t2, bucle          # Usamos bnez (Branch if Not Equal Zero)
# bucle_fin:
#     ret


# --- DIRECCIONES DE MEMORIA ACTUALIZADAS ---
.equ UART1_BASE, 0x60010000
.equ UART1_FIFO, UART1_BASE + 0x0000
.equ UART1_STATUS_REG, UART1_BASE + 0x001C
.equ UART_RXFIFO_CNT_MASK, 0x000003FF

.equ GPIO_BASE, 0x60004000
.equ GPIO_ENABLE_W1TS_REG, GPIO_BASE + 0x0024
.equ GPIO_ENABLE_W1TC_REG, GPIO_BASE + 0x0028
.equ GPIO_OUT_W1TC_REG, GPIO_BASE + 0x000C

.equ GPIO3_LED1, (1<<3)
.equ GPIO4_LED3, (1<<4)
.equ GPIO10_LED2, (1<<10)

.global prueba_gps
.global prueba_leds
.text

prueba_leds:
    li t0, GPIO_ENABLE_W1TS_REG
    li t1, GPIO3_LED1 | GPIO4_LED3 | GPIO10_LED2
    sw t1, 0(t0)
    li t2, GPIO_OUT_W1TC_REG
    sw t1, 0(t2)
    ret

prueba_gps:
    li t0, UART1_STATUS_REG
    lw t2, 0(t0)

    andi t2, t2, UART_RXFIFO_CNT_MASK
    beqz t2, no_hay_datos

    li t3, UART1_FIFO

    lb a0, 0(t3)
    ret

no_hay_datos:
    li a0, -1
    ret