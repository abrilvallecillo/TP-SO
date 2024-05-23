#ifndef PROCESOS_H_
#define PROCESOS_H_

#include<netdb.h>
#include <utils/conexiones.h>
#include <commons/collections/queue.h>
typedef struct {
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
} registros_generales;

typedef struct {
    uint32_t PID;
    uint32_t PC;
    uint32_t Quantum;
    registros_generales registros;
} pcb;

typedef struct {
    registros_generales generales;
    uint32_t PC;
    uint32_t SI;
    uint32_t DI;
}registros_CPU; //registros propios que va a usar la CPU

typedef enum {
    CONTEXTO_A_EJECUTAR, //Se envía un contexto desde el kernel al cpu,
    INT_FIN_CONSOLA,
    INT_FIN_QUANTUM,
    P_EXIT,
    P_ERROR,
    P_QUANTUM,
    P_IO_GEN_SLEEP,
    CREAR_PROCESO,
    PROCESO_CREADO,
    INFO_INTERFAZ
} codigos_operacion_proceso;

typedef struct {
    pcb * contexto;
    uint32_t longitud_nombre_interfaz;
    char * nombre_interfaz;
    uint32_t unidades_de_trabajo;
} pcb_IOGENSLEEP;

typedef struct {
    pcb * contexto;
    codigos_operacion_proceso operacion;
    t_queue * cola_parametros;
} pcb_cola_interfaz;

typedef struct {
    uint32_t pid;
    uint32_t longitud_direccion_codigo;
    char * direccion_codigo;
} inicializar_proceso;

t_buffer * serializarProceso(pcb * proceso_a_serializar);

pcb * deserializarProceso(t_buffer * buffer_paquete);

void enviarBufferProcesoConMotivo(t_buffer * buffer_proceso_con_parametros, codigos_operacion_proceso motivo, int un_socket); //Enviamos un proceso directamente con el motivo de desalojo

t_buffer * serializarProcesoIOGENSLEEP(pcb_IOGENSLEEP * proceso);

pcb_IOGENSLEEP * deserealizarProcesoIOGENSLEEP(t_buffer * buffer_paquete);

inicializar_proceso * crearInicializarProceso(char * ruta_codigo, int valor_pid);

t_buffer * serializarInicializarProceso(inicializar_proceso * nueva_peticion);

inicializar_proceso * deserializarInicializarProceso(t_buffer * buffer);
#endif