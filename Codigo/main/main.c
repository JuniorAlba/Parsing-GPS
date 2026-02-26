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
    while (trama[i] != '\0') { 
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
    char buffer_linea[128]; // Nuestro "balde" de 100 letras
    int indice = 0;         // El casillero por el que vamos
    while(true) {
        int letra;

        while ((letra = info_gps()) != -1) {

            // Si la letra es el final de la línea ('\n')
            if (letra == '\n') {

                buffer_linea[indice] = '\0';

                //$GPGGA,hhmmss.ss,Latitude,N,Longitude,E,FS,NoSV,HDOP,msl,m,Altref,m,DiffAge,DiffStation*cs<CR><LF>
                if (strstr(buffer_linea, "$GPGGA") && opcion=='1') {
                    char Latitud[100],Longitud[100];
                    obtener_campo_trama(buffer_linea,2,Latitud);
                    obtener_campo_trama(buffer_linea,4,Longitud);
                    printf("Aca esta la Longitud Oeste: %.3s grados y %s minutos \n", Longitud, Longitud+3);
                    printf("Aca esta la Latitud Sur: %.2s grados y %s minutos", Latitud, Latitud+2);
                    return;
                }
                //$GPGGA,hhmmss.ss,Latitude,N,Longitude,E,FS,NoSV,HDOP,msl,m,Altref,m,DiffAge,DiffStation*cs<CR><LF>
                else if (strstr(buffer_linea, "$GPGGA") && opcion=='2') {
                    char Altitud[100];
                    obtener_campo_trama(buffer_linea,9,Altitud);
                    printf("Aca esta la altitud: %s\n", Altitud);
                    return;
                }
                //$GPVTG,cogt,T,cogm,M,sog,N,kph,K,mode*cs<CR><LF>
                else if (strstr(buffer_linea, "$GPVTG") && opcion=='3') {
                    char Velocidad[100];
                    obtener_campo_trama(buffer_linea,7,Velocidad);
                    printf("Aca esta la velocidad: %s\n", Velocidad);
                    return;
                }
                indice = 0;
            }
            else{
                buffer_linea[indice] = (char)letra;
                indice++;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void app_main(void)
{
    //CONFIGURACION PUERTO UART1
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