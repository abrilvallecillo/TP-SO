#include "planificacion.h"
#include <utils/conexiones.h>
#include "kernel.h"
#include <commons/temporal.h>
#include <utils/logger_concurrente.h>
#include <utils/procesos.h>
#include <utils/hilos.h>
#include <semaphore.h>
#include <commons/string.h>
#include "configuracion.h"

pcb * obtenerProcesoPorFIFO(void);
pcb * obtenerProcesoPorRR(void);
pcb * obtenerProcesoPorVRR(void);
pcb * seleccionarProcesoPorAlgoritmo(void);
void enviarContexto(pcb * proceso_a_enviar, int conexion_cpu_dispatch);
void operarSegunMotivoDesalojo(t_paquete * proceso_desalojado);
void * pasarProcesosEstadoAExit(void * sin_parametro);
void * pasarProcesosNewAReady(void * sin_parametro);
void * contabilizarTiempoEjecucion(void * quantum_proceso);
void hacerIOGENSLEEP(pcb_IOGENSLEEP * pcb);
void agregarAColaBloqInterfazIOGENSLEEP(pcb_IOGENSLEEP * pcb);
void eliminarProceso(pcb * pcb_a_eliminar);

pthread_t pasar_procesos_new_ready;
pthread_t pasar_procesos_estado_exit;
pthread_t hacer_quantum;

sem_t * hay_procesos_cola_exit;
sem_t * hay_procesos_cola_new;
sem_t * grado_multiprogramacion_habilita;
sem_t * hay_procesos_cola_ready;

pthread_t hilo_timer;

void *planificarProcesos(void * sin_parametro){
    int conexion_cpu_dispatch = crearConexionCliente(configuracion.IP_CPU, configuracion.PUERTO_CPU_DISPATCH);
    int handshake_cpu_dispatch = enviarHandshake(conexion_cpu_dispatch, KERNEL);
    if(handshake_cpu_dispatch == -1) {
        fprintf(stderr, "Hubo problema al hacer el handshake con la memoria");
        abort();
    }

    //Iniciamos los semaforos
    sem_init(hay_procesos_cola_exit, 0, 0);
    sem_init(hay_procesos_cola_new, 0, 0);
    sem_init(grado_multiprogramacion_habilita, 0, grado_multiprogramacion);
    sem_init(hay_procesos_cola_ready, 0, 0);
    
    //Crear hilo control new ready y hilo de estado a exit
    pasar_procesos_estado_exit = crearHilo(pasarProcesosEstadoAExit, NULL);
    pasar_procesos_new_ready = crearHilo(pasarProcesosNewAReady, NULL);
    
    pthread_detach(pasar_procesos_estado_exit);
    pthread_detach(pasar_procesos_new_ready);
    //Seleccionar algoritmo de planificación
    while(1) {
        sem_wait(hay_procesos_cola_ready);
        pcb * proximo_proceso_a_ejecutar = seleccionarProcesoPorAlgoritmo();
        avisarCambioEstado(proximo_proceso_a_ejecutar->PID, "READY", "EXEC");
        enviarContexto(proximo_proceso_a_ejecutar, conexion_cpu_dispatch);
        t_paquete * paquete_pcb_desalojado = recibirPaqueteGeneral(conexion_cpu_dispatch);
        pthread_cancel(hilo_timer);
        operarSegunMotivoDesalojo(paquete_pcb_desalojado);
    }
    
    sem_destroy(hay_procesos_cola_exit);
    sem_destroy(hay_procesos_cola_new);
    sem_destroy(hay_procesos_cola_ready);
    sem_destroy(grado_multiprogramacion_habilita);
    return NULL;
}

void * pasarProcesosEstadoAExit(void * sin_parametro) {
    
    while(1) {
        sem_wait(hay_procesos_cola_exit);
        pcb * pcb_a_eliminar = sacarDeCola(cola_exit);
        eliminarProceso(pcb_a_eliminar);
        sem_post(grado_multiprogramacion_habilita);
    }

    return NULL;
}

void * pasarProcesosNewAReady(void * sin_parametro) {
    while(1) {
        sem_wait(hay_procesos_cola_new);
        sem_wait(grado_multiprogramacion_habilita);
        pcb * pcb_siguiente = (pcb *) sacarDeCola(cola_new);
        avisarCambioEstado(pcb_siguiente->PID, "NEW", "READY");
        agregarACola(cola_ready, (void *) pcb_siguiente);
        sem_post(hay_procesos_cola_ready);
    }

    return NULL;
}


pcb * seleccionarProcesoPorAlgoritmo(void) { //Devuelve el proceso seleccionado por el algoritmo 
    switch (algoritmo_elegido)
    {
    case FIFO:
        return obtenerProcesoPorFIFO();        
    case RR:
        return obtenerProcesoPorRR();
    case VRR:
        return obtenerProcesoPorVRR();
    default:
        fprintf(stderr, "Se selecciono un algoritmo invalido");
        abort();
    }
}


pcb * obtenerProcesoPorFIFO(void) {
    pcb * proceso_elegido = (pcb *) sacarDeCola(cola_ready);
    return proceso_elegido;
}

pcb * obtenerProcesoPorRR(void) {
    pcb * proceso_elegido = (pcb *) sacarDeCola(cola_ready);
    return proceso_elegido;
}

pcb * obtenerProcesoPorVRR(void) {
    //TODO
    return NULL;
}

void enviarContexto(pcb * proceso_a_enviar, int conexion_cpu_dispatch) {
    t_buffer * buffer_de_pcb = serializarProceso(proceso_a_enviar);
    enviarBufferPorPaquete(buffer_de_pcb, CONTEXTO_A_EJECUTAR, conexion_cpu_dispatch);
    if(algoritmo_elegido == RR || algoritmo_elegido == VRR) {
        hilo_timer = crearHilo(contabilizarTiempoEjecucion, &proceso_a_enviar->Quantum);
        pthread_detach(hilo_timer);    
    }
}


void operarSegunMotivoDesalojo(t_paquete * proceso_desalojado_paq) {
    pcb * proceso_recibido; 
    codigos_operacion_proceso motivo = proceso_desalojado_paq->codigoOperacion;
    switch (motivo)
    {
    case P_EXIT:
    case P_ERROR:
        proceso_recibido = deserializarProceso(proceso_desalojado_paq->buffer);
        agregarACola(cola_exit, (void *) proceso_recibido);
        break;
    case P_QUANTUM:
        proceso_recibido = deserializarProceso(proceso_desalojado_paq->buffer);
        agregarACola(cola_ready, (void *) proceso_recibido);
        avisarCambioEstado(proceso_recibido->PID, "EXEC", "READY");
        break;
    case P_IO_GEN_SLEEP:
        pcb_IOGENSLEEP * pcb_genSleep = deserealizarProcesoIOGENSLEEP(proceso_desalojado_paq->buffer);
        hacerIOGENSLEEP(pcb_genSleep);
        break;
    default:
        fprintf(stderr, "Error: Motivo de contexto devuelto desconocido");
        break;
    }

    eliminar_paquete(proceso_desalojado_paq);
}

void hacerIOGENSLEEP(pcb_IOGENSLEEP * pcb) {
    nombre_interfaz_a_comparar = string_duplicate(pcb->nombre_interfaz);
    if(!existeInterfazEnSistema(pcb->nombre_interfaz) || !interfazPuedeRealizarOperacion(P_IO_GEN_SLEEP)) {
        agregarACola(cola_exit, (void *) pcb->contexto);
    } else {
        agregarAColaBloqInterfazIOGENSLEEP(pcb);
    }
    free(pcb->nombre_interfaz);
    free(pcb);
}

void * contabilizarTiempoEjecucion(void * quantum_proceso) {
    int * tiempo_a_esperar = (int *) quantum_proceso;
    sleep(*tiempo_a_esperar);
    sem_post(no_surgio_otra_interrupcion);
    return NULL;
}

void agregarAColaBloqInterfazIOGENSLEEP(pcb_IOGENSLEEP * pcb) {
    pcb_cola_interfaz * proceso_iogensleep = malloc(sizeof(pcb_cola_interfaz));
    proceso_iogensleep->cola_parametros = queue_create();

    proceso_iogensleep->contexto = pcb->contexto;
    proceso_iogensleep->operacion = P_IO_GEN_SLEEP;
    queue_push(proceso_iogensleep->cola_parametros, &pcb->unidades_de_trabajo);
    t_interfaz * interfaz_elegida = encontrarInterfazEnLista();
    agregarACola(interfaz_elegida->cola_bloqueados, proceso_iogensleep);
}

void eliminarProceso(pcb * pcb_a_eliminar) {
    free(pcb_a_eliminar);
}