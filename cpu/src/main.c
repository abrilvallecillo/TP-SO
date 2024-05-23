#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/hilos.h>
#include <utils/conexiones.h>
#include <commons/log.h>
#include <commons/config.h> 
#include <pthread.h>
#include <utils/procesos.h>
#include "atencion_kernel.h"
#include "cpu.h"
#include "ciclo_instruccion.h"

int main(int argc, char* argv[]) {
    //Declaracion de loggers y config
    t_log* logger;
    t_log* logger_debug;
    t_config* config;

    //Creacion de los loggers
    logger = log_create("cpu.log", "Log del CPU", true, LOG_LEVEL_INFO);
    logger_debug = log_create("cpu_debug.log", "Log para puebas", true, LOG_LEVEL_INFO);
    
    //Creacion del socket de la conexion a memoria
    log_info(logger_debug, "Conectando a memoria...");
    int conexion_memoria = crearConexionCliente(configuracion.IP_MEMORIA , configuracion.PUERTO_MEMORIA);
    //Creacion de socket de la conexion dispatch con kernel
    int conexion_dispatch = crearConexionServidor(configuracion.puerto_escucha_dispatch);

    //Creo hilo detachable para gestionar las interrupciones
    pthread_t escucha_interrupt = crearHilo(atenderInterrupciones, configuracion.puerto_escucha_interrupt);

    pthread_detach(escucha_interrupt);
    int puerto_num = atoi(configuracion.puerto_escucha_interrupt);
    printf("%d", puerto_num);
    log_info(logger_debug, "Servidor listo para recibir al cliente");
  
    log_info(logger_debug, "Conectando a memoria...");
    int handshake_memoria = enviarHandshake(conexion_memoria, CPU);

    if(handshake_memoria == 0) {
        log_info(logger_debug, "Handshake exitoso");
    } else {
        log_error(logger_debug, "No se pudo realizar el handshake");
    };

    //Conexion dispatch
    conexion_dispatch_kernel = esperarCliente(conexion_dispatch);
    int resultado_dispatch = recibirHandshake(conexion_dispatch_kernel);
    if(resultado_dispatch == 0) {
            log_info(logger_debug, "Handshake exitoso");
        } else {
            log_error(logger_debug, "No se pudo realizar el handshake");
    };

    while(1){
        t_paquete* buffer_proceso = recibirPaqueteGeneral(conexion_dispatch_kernel);
        pcb* proceso_elegido = deserializarProceso(buffer_proceso->buffer);
    }

    log_destroy(logger);
    log_destroy(logger_debug);
    config_destroy(config);

    return 0;
}
