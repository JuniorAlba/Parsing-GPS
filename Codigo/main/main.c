#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include <string.h>

extern void configurar_leds(void);
extern void apagar_leds(void);
extern void prender_led1(void);
extern void prender_led2(void);
extern void prender_led3(void);
extern void apagar_led1(void);
extern void apagar_led2(void);
extern int info_gps(void);

void obtener_campo_trama(const char *trama, int numero_coma, char *resultado) {
    int comas_actuales = 0;
    int i = 0, j = 0;
    //El dato que buscamos se encuentra antes del fin de la cadena y antes del checksum
    //(el checksum esta delimitado por el caracter *)
    while (trama[i] != '\0' && trama[i] != '*') {
        if (trama[i] == ',') {
            comas_actuales++;
        } else if (comas_actuales == numero_coma) {
            resultado[j] = trama[i];
            j++;
        } else if (comas_actuales > numero_coma) {
            break;
        }
        i++;
    }
    resultado[j]= '\0';
}
char mostrar_menu_y_esperar(void) {
    char opcion = '0';
    printf("\n=================================================\n");
    printf("   SISTEMA DE ADQUISICION DE DATOS GPS (UART1)   \n");
    printf("=================================================\n");
    printf("Seleccione la informacion que desea consultar:\n");
    printf(" [1] Coordenadas (Latitud y Longitud)\n");
    printf(" [2] Altitud\n");
    printf(" [3] Velocidad\n");
    printf("=================================================\n");

    // Bucle infinito esperando una tecla válida
    while (1) {
        opcion=getchar();

        // Filtramos para que solo acepte 1, 2 o 3
        if (opcion == '1' || opcion == '2' || opcion == '3') {
            printf("\n\n>>> Opcion [%c] recibida correctamente.\n", opcion);
            return opcion;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


void mostrar_opcion(char opcion){
    char buffer_linea[128];
    int indice = 0;
    while(true) {
        int letra;

        while ((letra = info_gps()) != -1) {

            // Si la letra es el final de la línea ('\n')
            if (letra == '\n') {

                buffer_linea[indice] = '\0';

                //$GPGGA,hhmmss.ss,Latitude,N,Longitude,E,FS,NoSV,HDOP,msl,m,Altref,m,DiffAge,DiffStation*cs<CR><LF>
                if (strstr(buffer_linea, "$GPGGA") && opcion=='1') {
                    char Latitud[20] = "",Longitud[20] = "", dir_Latitud[2] = "", dir_Longitud[2] = "";
                    obtener_campo_trama(buffer_linea,2,Latitud);
                    obtener_campo_trama(buffer_linea,3,dir_Latitud);
                    obtener_campo_trama(buffer_linea,4,Longitud);
                    obtener_campo_trama(buffer_linea,5,dir_Longitud);
                    printf("Aca esta la Longitud %s: %.3s grados y %s minutos \n",dir_Longitud, Longitud, Longitud+3);
                    printf("Aca esta la Latitud %s: %.2s grados y %s minutos",dir_Latitud, Latitud, Latitud+2);
                    return;
                }
                //$GPGGA,hhmmss.ss,Latitude,N,Longitude,E,FS,NoSV,HDOP,msl,m,Altref,m,DiffAge,DiffStation*cs<CR><LF>
                else if (strstr(buffer_linea, "$GPGGA") && opcion=='2') {
                    char Altitud[20] = "";
                    obtener_campo_trama(buffer_linea,9,Altitud);
                    printf("Aca esta la altitud: %s metros\n", Altitud);
                    return;
                }
                //$GPVTG,cogt,T,cogm,M,sog,N,kph,K,mode*cs<CR><LF>
                else if (strstr(buffer_linea, "$GPVTG") && opcion=='3') {
                    char Velocidad[20] = "";
                    obtener_campo_trama(buffer_linea,7,Velocidad);
                    printf("Aca esta la velocidad: %s km/h\n", Velocidad);
                    return;
                }
                indice = 0;
            }

            //Si el caracter leido no es \n entonces la guardamos en la cadena de la linea
            //solo si la cadena no llego a su maxima capacidad, las lineas que suelta el gps
            //deberian ser de menos de 128 caracteres, pero si se da el caso de que una linea sea
            //mayor, en ese caso, evitamos el desbordamiento
            //ademas dejo espacio para el caracter de fin de cadena
            else if(indice < 127){
                buffer_linea[indice] = (char)letra;
                indice++;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void app_main(void)
{
    // CONFIGURACION DEL PUERTO SERIE (UART1) PARA EL MODULO GPS

    // 1. Instalación del driver del sistema operativo (ESP-IDF)
    // Parámetros: Puerto 1
    //Buffer de Recepción RAM: 256 bytes
    //Buffer de Transmisión: 0 (solo recibimos datos)
    // Event Queue: 0 (desactivada) | Puntero a Queue: NULL | Flags de Interrupción: 0 (por defecto) -> parametros destinados a interrupciones
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);

    // 2. Estructura de parametros necesaria para configurar UART1
    uart_config_t uart1_config = {
        .baud_rate = 9600,                     // Velocidad de transmisión del GPS
        .data_bits = UART_DATA_8_BITS,         // 8 bits por carácter (ASCII)
        .parity    = UART_PARITY_DISABLE,      // Sin bit de comprobación de errores (Protocolo 8N1)
        .stop_bits = UART_STOP_BITS_1,         // 1 bit de parada para marcar el final de cada carácter
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Sin control de flujo por hardware (el gps solo tiene pines RX y TX)
        .source_clk = UART_SCLK_APB,           // Utilizamos el reloj destinado a manejar perifericos
    };

    // 3. Aplicamos la configuracion al puerto
    uart_param_config(UART_NUM_1, &uart1_config);

    // 4. Conectamos internamente la UART1 a los pines de la placa ESP32-C3:
    // Puerto | Pin TX: GPIO 1 | Pin RX: GPIO 0 | RTS: No cambiar | CTS: No cambiar
    uart_set_pin(UART_NUM_1, 1, 0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Debido a que necesitamos 5 pines GPIO y, la placa solo proporciona 4 pines libres
    //de manera nativa, le solicitamos que libere un pin destinado a JTAG, destinado a depuracion por hardware,
    //para fines de este trabajo practico no es necesario depurar a ese nivel.
    gpio_reset_pin(4);


    while(true){
        configurar_leds();
        char opcion = mostrar_menu_y_esperar();
        vTaskDelay(pdMS_TO_TICKS(1000));
        apagar_leds();
        prender_led1();
        vTaskDelay(pdMS_TO_TICKS(1000));
        apagar_led1();
        prender_led2();
        vTaskDelay(pdMS_TO_TICKS(1000));
        mostrar_opcion(opcion);
        apagar_led2();
        prender_led3();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}