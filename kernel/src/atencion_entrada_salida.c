#include "atencion_entrada_salida.h"
#include <utils/conexiones.h>
#include <semaphore.h>
#include <utils/procesos.h>
#include <utils/logger_concurrente.h>
#include "kernel.h"
#include <utils/interfaces.h>

t_interfaz * recibirTipoInterfaz(int conexion_cliente_io);
t_interfaz * deserializarInformacionInterfaz(t_buffer * buffer);
void enviarOperacionAInterfaz(pcb_cola_interfaz * pcb_a_trabajar, int conexion_cliente_io);
void recibirOperacionAInterfaz(int conexion_cliente_io);

void * atenderIO(void * socket_cliente_io) {
    int* conexion_cliente_io = (int *) socket_cliente_io;
    int resultado = recibirHandshake(*conexion_cliente_io);
    if(resultado == -1) {
        fprintf(stderr, "Hubo problema al hacer el handshake con la interfaz");
        abort();
    }

    t_interfaz * interfaz_actual = recibirTipoInterfaz(*conexion_cliente_io); //Inicializara ademas el semaforo de hay_procesos_en_interfaz
    agregarInterfazALista(interfaz_actual);
    while(1) {
        sem_wait(interfaz_actual->hay_procesos_en_interfaz);
        pcb_cola_interfaz * pcb_a_trabajar = sacarDeCola(interfaz_actual->cola_bloqueados);
        enviarOperacionAInterfaz(pcb_a_trabajar, *conexion_cliente_io);
        recibirOperacionAInterfaz(*conexion_cliente_io);
        agregarACola(cola_ready, pcb_a_trabajar->contexto);
        
        queue_destroy(pcb_a_trabajar->cola_parametros);
        free(pcb_a_trabajar);
    }
    return NULL;
}

t_interfaz * recibirTipoInterfaz(int conexion_cliente_io) {
    t_paquete * paquete_informacion_interfaz = recibirPaqueteGeneral(conexion_cliente_io);
    t_interfaz * interfaz_obtenida = deserializarInformacionInterfaz(paquete_informacion_interfaz->buffer);
    eliminar_paquete(paquete_informacion_interfaz);
    return interfaz_obtenida;
}

t_interfaz * deserializarInformacionInterfaz(t_buffer * buffer) {
    t_interfaz * interfaz = malloc(sizeof(t_interfaz));
    
    uint32_t longitud_nombre = leerDeBufferUint32(buffer);
    interfaz->nombre_interfaz = leerDeBufferString(buffer, longitud_nombre);
    interfaz->tipo_de_interfaz = leerDeBufferUint8(buffer);
    iniciarCola(interfaz->cola_bloqueados);
    sem_init(interfaz->hay_procesos_en_interfaz, 0, 0);

    return interfaz;
}

void enviarOperacionAInterfaz(pcb_cola_interfaz * pcb_a_trabajar, int conexion_cliente_io) { //Aca ya suponemos que se ha elegido correctamente el tipo de operacion que corresponde a la interfaz
    t_buffer * buffer;
    switch(pcb_a_trabajar->operacion) {
        case P_IO_GEN_SLEEP:
            buffer = serializarOperacionIOGENSLEEP(pcb_a_trabajar);
            break;
        default:
            fprintf(stderr, "Error: se ha enviado un codigo de operacion no reconocido");
            return;
    }

    enviarBufferProcesoConMotivo(buffer, pcb_a_trabajar->operacion, conexion_cliente_io);
}

void recibirOperacionAInterfaz(int conexion_cliente_io) {
    t_paquete * paq_salio_bien_operacion= recibirPaqueteGeneral(conexion_cliente_io);
    if(paq_salio_bien_operacion->codigoOperacion != P_IO_GEN_SLEEP) {
        fprintf(stderr, "Hubo un error al realizar la operacion IO_GEN_SLEEP");
    }
    eliminar_paquete(paq_salio_bien_operacion);
}