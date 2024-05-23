#include "interfaces.h"



t_buffer * serializarInformacionInterfaz(info_interfaz * interfaz){
    t_buffer * buffer = crearBufferGeneral(
        sizeof(uint32_t) //longitud_nombre
        + interfaz->longitud_nombre
        + sizeof(uint8_t) //tipo_interfaz
    );

    agregarABufferString(buffer, interfaz->nombre, interfaz->longitud_nombre);
    agregarABufferUint8(buffer, interfaz->tipo_interfaz);

    return buffer;
}

t_buffer * serializarOperacionIOGENSLEEP(pcb_cola_interfaz * pcb_a_trabajar) {
    t_buffer *buffer = crearBufferGeneral(
        sizeof(uint32_t) //unidades_de_trabajo 
    );

    uint32_t * unidades_de_trabajo = (uint32_t *) queue_pop(pcb_a_trabajar->cola_parametros);
    agregarABufferUint32(buffer, *unidades_de_trabajo);
    
    return buffer;
}

uint32_t deserializarOperacionIOGENSLEEP(t_buffer * buffer_operacion) {
    return leerDeBufferUint32(buffer_operacion);
}