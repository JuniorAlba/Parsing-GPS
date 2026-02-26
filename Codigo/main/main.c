// #include <stdio.h>
// #include <inttypes.h>
// #include "sdkconfig.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/gpio.h" // Librería obligatoria para manejar el multiplexor
// #include "driver/uart.h"
// #include "esp_private/periph_ctrl.h" // Para habilitar el reloj manualmente
// // Declaramos tu subrutina escrita en Assembler
// extern void prueba_leds(void);
// extern void prueba_gps(void);
// void app_main(void)
// {
//     // ========================================================
//     uart_config_t uart1_config = {
//         .baud_rate = 9600,                      // Baudrate exacto del GPS
//         .data_bits = UART_DATA_8_BITS,          // 8 bits de datos
//         .parity    = UART_PARITY_DISABLE,       // Sin paridad
//         .stop_bits = UART_STOP_BITS_1,          // 1 bit de parada
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  // Sin control de flujo por hardware
//         .source_clk = UART_SCLK_APB,            // Reloj base estándar
//     };
//     // Aplicamos los parámetros al puerto UART1
//     uart_param_config(UART_NUM_1, &uart1_config);
//         // 1. Forzamos la desconexión del JTAG y los pasamos a modo GPIO normal
//     uart_set_pin(UART_NUM_1, 1, 0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
//     // TRUCO MÁGICO: Instalamos el driver pero con BUFFER 0 para que no nos robe los datos
//     uart_driver_install(UART_NUM_1, 0, 0, 0, NULL, 0);
//     gpio_reset_pin(4);
//     printf("Configurando hardware desde C...\n");

//     prueba_leds();

//     while(1) {
//         prueba_gps();                   // Tu ensamblador vacía la FIFO si hay datos
//         vTaskDelay(pdMS_TO_TICKS(10));  // Pausa de 10ms para no asfixiar a la placa
//     }

//     printf("Programa finalizado.\n");
// }
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"


extern void prueba_leds(void);
extern int prueba_gps(void); // Ahora sabemos que devuelve un int seguro

void app_main(void)
{
    uart_config_t uart1_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };


    uart_param_config(UART_NUM_1, &uart1_config);
    uart_set_pin(UART_NUM_1, 1, 0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    gpio_reset_pin(4);
    prueba_leds();

    printf("Iniciando lectura APB de 32 bits (TX GPS -> GPIO 0)...\n");

    while(1) {
        int letra;

        // Llamamos al Ensamblador. Ahora sí la FIFO se va a ir vaciando.
        while ((letra = prueba_gps()) != -1) {
            printf("%c",letra);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Pausa segura de 10ms
    }
}