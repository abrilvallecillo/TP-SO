#include "atencion_modulos.h"
#include <commons/collections/list.h>

t_list * codigos_programas;

typedef struct {
    int pid;
    char ** array_instrucciones;
} codigo_proceso; 

void * atenderPeticiones(void * conexion_cliente) {
    int * socket_cliente = (int *) conexion_cliente;
    int resultado = recibirHandshake(*conexion_cliente);
    if(resultado == -1) {
        fprintf(stderr, "Hubo problema al hacer el handshake con el cliente");
        abort();
    }

    codigos_programas = list_create();

    while(1) {
        t_paquete * paquete_peticion = recibirPaqueteGeneral(*socket_cliente);
        switch(paquete_peticion->codigo_de_operacion) {
            case CREAR_PROCESO:
                inicializar_proceso * nueva_peticion = deserializarInicializarProceso(paquete_peticion->buffer);
                
                break;
            default:
                fprintf(stderr, "Error: codigo de operacion desconocido");
                break;
        }
        eliminar_paquete(paquete_peticion);
    }

}

pcb * CrearProceso(inicializar_proceso * peticion) {
    pcb * pcb_nuevo = malloc(sizeof(pcb));
    codigo_proceso * codigo_nuevo = malloc(sizeof(codigo_proceso));

    pcb_nuevo->PID = peticion->pid;
    pcb_nuevo->PC = 0;
    pcb_nuevo->registros = {0, 0, 0, 0, 0, 0, 0, 0};
    pcb_nuevo->Quantum = peticion->quantum;
    
    codigo_nuevo->pid = peticion->pid;
    FILE *codigo_proceso_archivo = fopen(peticion->direccion_codigo, "r");
    char *lectura_linea;
    if (codigo_proceso_archivo == NULL){fprintf(stderr, "El archivo esta vacio"); abort();}
    int i = 0;
    while(fgets(lectura_linea, 256, codigo_proceso_archivo)){
        (codigo_nuevo->array_instrucciones + i) = string_duplicate(lectura_linea);
        i++;
    }
    fclose(codigo_proceso_archivo);
    list_add(codigos_programas, codigo_nuevo);

    return pcb_nuevo;   
}