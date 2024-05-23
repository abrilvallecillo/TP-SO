#ifndef INTERFACES_H_
#define INTERFACES_H_

#include<netdb.h>
#include <utils/conexiones.h>
#include <commons/collections/queue.h>
#include <utils/procesos.h>

typedef struct {
    uint32_t longitud_nombre;
    char * nombre;
    uint8_t tipo;
} info_interfaz;

typedef enum{
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS    
}t_interfaz; 

t_buffer * serializarInformacionInterfaz(info_interfaz * interfaz);

t_buffer * serializarOperacionIOGENSLEEP(pcb_cola_interfaz * pcb_a_trabajar);

uint32_t deserializarOperacionIOGENSLEEP(t_buffer * buffer_operacion); 
#endif