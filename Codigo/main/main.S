.equ UART0_BASE, 0x60000000
.equ UART1_BASE, 0x60010000
.equ UART0_FIFO, UART0_BASE + 0x0000
.equ UART1_FIFO, UART1_BASE + 0x0000
.equ UART0_STATUS_REG, UART0_BASE + 0x001C
.equ UART1_STATUS_REG, UART1_BASE + 0x001C
#Con estos registros tenemos todo listo para operar el puerto UART
.equ GPIO_BASE, 0x60004000      #Espacio de memoria de los GPIO
.equ GPIO_ENABLE_REG, GPIO_BASE + 0x0020        #Registro que nos dice que GPIO estan configurados como salida
.equ GPIO_ENABLE_W1TS_REG, GPIO_BASE + 0x0024   #Registro que permite configurar un GPIO para salida de datos
.equ GPIO_ENABLE_W1TC_REG, GPIO_BASE + 0x0028   #Registro que permite deshabilitar un GPIO para salida de datos (ponerlo como entrada)

.equ GPIO_OUT_REG, GPIO_BASE + 0x0004           #Registro que nos dice que datos de salida hay en cada GPIO
.equ GPIO_OUT_W1TS_REG, GPIO_BASE + 0x0008      #Registro que nos permite setear un 1 en la salida de un GPIO
.equ GPIO_OUT_W1TC_REG, GPIO_BASE + 0x000C      #Registro que nos permite setear un 0 en la salida de un GPIO

.equ GPIO2_TXGPS, (1<<2)
.equ GPIO3_RXGPS, (1<<3)
.equ GPIO4_LED1, (1<<4)
.equ GPIO5_LED2, (1<<5)
.equ GPIO6_LED3, (1<<6)


.equ UART_RXFIFO_CNT_MASK, 0x000003FF
.equ UART_TXFIFO_CNT_MASK, 0x03FF0000

.section .data
assembly_sum:
    # Cargar los dos valores a sumar
    li a0, 25        # primer número
    li a1, 17        # segundo número

    # Sumar los valores
    add a0, a0, a1   # a0 = a0 + a1

    # Devolver resultado (por convención de RISC-V, el retorno va en a0)
    ret