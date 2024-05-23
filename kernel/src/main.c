#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/hilos.h>
#include <utils/conexiones.h>
#include <commons/log.h>
#include <commons/config.h> 
#include <pthread.h>
#include "atencion_entrada_salida.h"
#include "consola.h"
#include "control_interrupciones.h"
#include "manejo_memoria.h"
#include "planificacion.h"
#include "kernel.h"
#include <utils/logger_concurrente.h>
#include "configuracion.h"

int main(int argc, char* argv[]) {
    t_config* config;

    
    //Inicializamos las colas, el logger y el config
    iniciarCola(cola_memoria);
    iniciarCola(cola_new);
    iniciarCola(cola_ready);
    iniciarCola(cola_ready_vrr);
    inicializarLogger("Log del Kernel", "kernel.log");
    sem_init(solicitaron_finalizar_proceso, 0, 0);
    sem_init(no_surgio_otra_interrupcion, 0, 0);
    sem_inti(hay_peticion_en_cola, 0, 0);
    pid_nuevo_proceso = 0;
    iniciarEnteroSincronizado(&grado_multiprogramacion);
    lista_interfaces = list_create();

    obtener_config(argv[1]);
    algoritmo_elegido = algoritmoPorString(configuracion.ALGORITMO_PLANIFICACION);
    config_destroy(config);
    grado_multiprogramacion.entero = configuracion.GRADO_MULTIPROGRAMACION;

    //Creamos los hilos para las conexiones del kernel como cliente y la consola
    pthread_t planificador_de_procesos_h = crearHilo(planificarProcesos, NULL);
    pthread_t control_de_interrupciones_h = crearHilo(interrupcionDeProcesos, NULL);
    pthread_t manejo_de_memoria_h = crearHilo(manejarMemoria, NULL);
    pthread_t consola_h = crearHilo(consola, NULL);

    pthread_detach(consola_h);
    pthread_detach(planificador_de_procesos_h);
    pthread_detach(control_de_interrupciones_h);
    pthread_detach(manejo_de_memoria_h);
    
    //Creamos la conexión para que recibamos las conexiones de Memoria
    int conexion_servidor_memoria = crearConexionServidor(configuracion.PUERTO_MEMORIA);
    log_info(logger, "Servidor listo para recibir al cliente");

    //Creamos la conexión para que recibamos las conexiones de I/O
    int conexion_servidor_io = crearConexionServidor(configuracion.PUERTO_ESCUCHA);
    log_info(logger, "Servidor listo para recibir al cliente");

    //Hacemos el loop para ir recibiendo conexiones de I/O
    while (1){
        int * socket_cliente_io = malloc(sizeof(int));
        *socket_cliente_io = esperarCliente(conexion_servidor_io);
        pthread_t hiloAtencionIO = crearHilo(atenderIO, socket_cliente_io);
        pthread_detach(hiloAtencionIO);
    }
   
    log_destroy(logger);
    
    return 0;
}


