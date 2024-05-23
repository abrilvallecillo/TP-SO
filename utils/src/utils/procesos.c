#include "procesos.h"
#include <commons/collections/list.h>
#include <commons/string.h>
t_list * archivo_codigo;



void agregarProcesoABuffer(t_buffer * buffer, pcb * proceso_a_serializar);

t_buffer * serializarProceso(pcb * proceso_a_serializar) {
    t_buffer * buffer = crearBufferGeneral(
        sizeof(uint32_t)   //PID
        + sizeof(uint32_t) //PC
        + 4 * sizeof(uint8_t) //registros AX, BX, CX y DX
        + 4 * sizeof(uint32_t) //registros EAX, EBX, ECX, EDX
    );

    agregarProcesoABuffer(buffer, proceso_a_serializar);

    return buffer;
}


pcb * deserializarProceso(t_buffer * buffer_paquete) {
    pcb * proceso = malloc(sizeof(pcb));

    proceso->PID = leerDeBufferUint32(buffer_paquete);
    proceso->PC = leerDeBufferUint32(buffer_paquete);
    proceso->registros.AX = leerDeBufferUint8(buffer_paquete);
    proceso->registros.BX = leerDeBufferUint8(buffer_paquete);
    proceso->registros.CX = leerDeBufferUint8(buffer_paquete);
    proceso->registros.DX = leerDeBufferUint8(buffer_paquete);
    proceso->registros.EAX = leerDeBufferUint32(buffer_paquete);
    proceso->registros.EBX = leerDeBufferUint32(buffer_paquete);
    proceso->registros.ECX = leerDeBufferUint32(buffer_paquete);
    proceso->registros.EDX = leerDeBufferUint32(buffer_paquete);

    return proceso;
}



void enviarBufferProcesoConMotivo(t_buffer * buffer_proceso_con_parametros, codigos_operacion_proceso motivo, int un_socket) {
    enviarBufferPorPaquete(buffer_proceso_con_parametros, (uint8_t) motivo, un_socket);
}

t_buffer * serializarProcesoIOGENSLEEP(pcb_IOGENSLEEP * proceso) {
    t_buffer * buffer = crearBufferGeneral(
        sizeof(uint32_t)   //PID
        + sizeof(uint32_t) //PC
        + 4 * sizeof(uint8_t) //registros AX, BX, CX y DX
        + 4 * sizeof(uint32_t) //registros EAX, EBX, ECX, EDX
        + sizeof(uint32_t)  //longitud nombre de interfaz
        + proceso->longitud_nombre_interfaz
        + sizeof(uint32_t)  //unidades de trabajo
    );

    agregarProcesoABuffer(buffer, proceso->contexto);
    agregarABufferString(buffer, proceso->nombre_interfaz, proceso->longitud_nombre_interfaz);
    agregarABufferUint32(buffer, proceso->unidades_de_trabajo);
    return buffer;
}

pcb_IOGENSLEEP * deserealizarProcesoIOGENSLEEP(t_buffer * buffer_paquete) {
    pcb_IOGENSLEEP * pcb_iogensleep = malloc(sizeof(pcb_IOGENSLEEP));

    pcb_iogensleep->contexto = deserializarProceso(buffer_paquete);    
    pcb_iogensleep->longitud_nombre_interfaz = leerDeBufferUint32(buffer_paquete);
    pcb_iogensleep->nombre_interfaz = leerDeBufferString(buffer_paquete, pcb_iogensleep->longitud_nombre_interfaz);
    pcb_iogensleep->unidades_de_trabajo = leerDeBufferUint32(buffer_paquete);

    return pcb_iogensleep;
}

void agregarProcesoABuffer(t_buffer * buffer, pcb * proceso_a_serializar) {
    agregarABufferUint32(buffer, proceso_a_serializar->PID);
    agregarABufferUint32(buffer, proceso_a_serializar->PC);
    agregarABufferUint8(buffer, proceso_a_serializar->registros.AX);
    agregarABufferUint8(buffer, proceso_a_serializar->registros.BX);
    agregarABufferUint8(buffer, proceso_a_serializar->registros.CX);
    agregarABufferUint8(buffer, proceso_a_serializar->registros.DX);
    agregarABufferUint32(buffer, proceso_a_serializar->registros.EAX);
    agregarABufferUint32(buffer, proceso_a_serializar->registros.EBX);
    agregarABufferUint32(buffer, proceso_a_serializar->registros.ECX);
    agregarABufferUint32(buffer, proceso_a_serializar->registros.EDX);
}

 //Es una estructura lo que crea, que tiene los datos necesarios para enviarle a memoria que cree el proceso
inicializar_proceso * crearInicializarProceso(char * ruta_codigo, int valor_pid) {
    inicializar_proceso * nueva_peticion = malloc(inicializar_proceso);
    nueva_peticion->pid = valor_pid;
    nueva_peticion->direccion_codigo = string_duplicate(ruta_codigo);
    nueva_peticion->longitud_direccion_codigo = strlen(ruta_codigo) + 1;

    return nueva_peticion;
} 

t_buffer * serializarInicializarProceso(inicializar_proceso * nueva_peticion) {
     t_buffer * buffer = crearBufferGeneral(
        sizeof(uint32_t)   //PID
        + sizeof(uint32_t) //longitud_direccion_codigo
        + nueva_peticion->longitud_direccion_codigo
    );

    agregarABufferUint32(buffer, nueva_peticion->pid);
    agregarABufferString(buffer, nueva_peticion->direccion_codigo, nueva_peticion->longitud_direccion_codigo);

    return buffer;
}

inicializar_proceso * deserializarInicializarProceso(t_buffer * buffer) {
    inicializar_proceso * nueva_peticion = malloc(sizeof(inicializar_proceso));

    nueva_peticion->pid = leerDeBufferUint32(buffer);
    nueva_peticion->longitud_direccion_codigo = leerDeBufferUint32(buffer);
    nueva_peticion->direccion_codigo = leerDeBufferString(buffer, nueva_peticion->longitud_direccion_codigo);

    return nueva_peticion;
}