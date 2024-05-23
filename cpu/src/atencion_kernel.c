#include "atencion_kernel.h"
#include <utils/hello.h>

void * atenderInterrupciones(void * puerto_interrupt){
    char * puerto_escucha_interrupt = (char *) puerto_interrupt;
        printf("El string recibido es: \"%s\"\n", puerto_escucha_interrupt);

    int puerto_num = atoi(puerto_escucha_interrupt);
        printf("El puerto es: \"%d\"\n", puerto_num);
    return 0;
};