.equ UART1_BASE, 0x60010000
.equ UART1_FIFO, UART1_BASE + 0x0000
.equ UART1_STATUS_REG, UART1_BASE + 0x001C
.equ UART_RXFIFO_CNT_MASK, 0x000003FF

.equ GPIO_BASE, 0x60004000
.equ GPIO_ENABLE_W1TS_REG, GPIO_BASE + 0x0024
.equ GPIO_ENABLE_W1TC_REG, GPIO_BASE + 0x0028
.equ GPIO_OUT_W1TC_REG, GPIO_BASE + 0x000C
.equ GPIO_OUT_W1TS_REG, GPIO_BASE + 0x0008

.equ GPIO1_LED1, (1<<1)
.equ GPIO3_LED2, (1<<3)
.equ GPIO10_LED3, (1<<10)



.global configurar_leds
.global info_gps
.global apagar_leds
.global prender_led1
.global prender_led2
.global prender_led3
.global apagar_led1
.global apagar_led2
.text

configurar_leds:
    li t0, GPIO_ENABLE_W1TS_REG
    li t1, GPIO1_LED1 | GPIO3_LED2 | GPIO10_LED3
    sw t1, 0(t0)
    ret
apagar_leds:
    li t1, GPIO1_LED1 | GPIO3_LED2 | GPIO10_LED3
    li t2, GPIO_OUT_W1TS_REG
    sw t1, 0(t2)
    ret
apagar_led1:
    li t1, GPIO1_LED1
    li t2, GPIO_OUT_W1TS_REG
    sw t1, 0(t2)
    ret
apagar_led2:
    li t1, GPIO3_LED2
    li t2, GPIO_OUT_W1TS_REG
    sw t1, 0(t2)
    ret
prender_led1:
    li t1, GPIO1_LED1
    li t2, GPIO_OUT_W1TC_REG
    sw t1, 0(t2)
    ret
prender_led2:
    li t1, GPIO3_LED2
    li t2, GPIO_OUT_W1TC_REG
    sw t1, 0(t2)
    ret
prender_led3:
    li t1, GPIO10_LED3
    li t2, GPIO_OUT_W1TC_REG
    sw t1, 0(t2)
    ret
info_gps:
    li t0, UART1_STATUS_REG
    lw t2, 0(t0)

    andi t2, t2, UART_RXFIFO_CNT_MASK
    beqz t2, no_hay_datos

    li t3, UART1_FIFO

    lbu a0, 0(t3)
    ret

no_hay_datos:
    li a0, -1
    ret