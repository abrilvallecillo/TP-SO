#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/hilos.h>
#include <utils/conexiones.h>
#include <commons/log.h>
#include <commons/config.h> 

int main(int argc, char* argv[]) {
    t_log* logger;
    t_config* config;
    char * puerto_escucha;

    logger = log_create("memoria.log", "Log de la Memoria", true, LOG_LEVEL_INFO);
    config = config_create(argv[1]);

    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    int conexion_servidor = crearConexionServidor(puerto_escucha);
    log_info(logger, "Servidor listo para recibir al cliente");
    
    while (1) {
        int * socket_cliente = malloc(sizeof(int));
        *socket_cliente = esperarCliente(conexion_servidor);
        pthread_t hiloAtencion = crearHilo(atenderPeticiones, socket_cliente);
        pthread_detach(hiloAtencion);
    }

    log_destroy(logger);
    config_destroy(config);
    return 0;
}

