#include "manejo_memoria.h"
#include <utils/conexiones.h>
#include <utils/procesos.h>
#include <commons/collections/queue.h>
#include "kernel.h"
#include <commons/log.h>
#include "configuracion.h"

pcb * recibirRespuestaACrearProceso(int conexion_memoria);

void *manejarMemoria(void * sin_parametro) {
    int conexion_memoria = crearConexionCliente(configuracion.IP_MEMORIA, configuracion.PUERTO_MEMORIA);
    int handshake_memoria = enviarHandshake(conexion_memoria, KERNEL);
    if(handshake_memoria == -1) {
        fprintf(stderr, "Hubo problema al hacer el handshake con la memoria");
        abort();
    }
    
    while(1){
        inicializar_proceso * nueva_peticion = sacarDeCola(cola_memoria);
        t_buffer * buffer = serializarInicializarProceso(nueva_peticion);
        enviarBufferProcesoConMotivo(buffer, CREAR_PROCESO, conexion_memoria);
        pcb * pcb_obtenido = recibirRespuestaACrearProceso(conexion_memoria);
        agregarACola(cola_new, pcb_obtenido);
        free(nueva_peticion->direccion_codigo);
        free(nueva_peticion);
    }
    return NULL;
}

pcb * recibirRespuestaACrearProceso(int conexion_memoria) {
    t_paquete * paquete_proceso = recibirPaqueteGeneral(conexion_memoria);
    pcb * pcb_nuevo = deserializarProceso(paquete_proceso->buffer);
    eliminar_paquete(paquete_proceso);
    return pcb_nuevo;
}