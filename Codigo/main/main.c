#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Declarar la función assembler que devuelve un entero
extern int assembly_sum(void);

void app_main(void)
{
    printf("Iniciando programa con assembler...\n");

    // Llamamos a la función escrita en assembler
    int resultado = assembly_sum();

    // Mostramos el resultado
    printf("Resultado calculado por assembler: %d\n", resultado);

    printf("Programa finalizado.\n");
}