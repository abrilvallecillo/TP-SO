#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/hilos.h>
#include <utils/conexiones.h>
#include <commons/log.h>
#include <commons/config.h> 
#include <utils/procesos.h>
#include <utils/interfaces.h>
#include "configuracion.h"

int main(int argc, char* argv[]) {
    t_log* logger;
    t_config* config;

    logger = log_create("entradasalida.log", "Log de IO", true, LOG_LEVEL_INFO);

    // Enviando un handshake para establecer la comunicación con él.
    int conexion_kernel = crearConexionCliente(configuracion.IP_KERNEL, configuracion.puerto_kernel);
    int handshake_kernel = enviarHandshake(conexion_kernel, ENTRADASALIDA);

    // Enviando un handshake para establecer la comunicación con él.
    int conexion_memoria = crearConexionCliente(configuracion.IP_MEMORIA, configuracion.PUERTO_MEMORIA);
    int handshake_memoria = enviarHandshake(conexion_memoria, ENTRADASALIDA);

    info_interfaz* tipo_interfaz = crearTipoInterfaz(argv[1], configuracion.TIPO_INTERFAZ);
    t_buffer* buffer_interfaz = serializarInformacionInterfaz(tipo_interfaz);
    enviarBufferProcesoConMotivo(buffer_interfaz, INFO_INTERFAZ, conexion_kernel);

    while(1){
        t_paquete* paquete_kernel = recibirPaqueteGeneral(conexion_kernel);
        switch(paquete_kernel->codigoOperacion){
            case P_IO_GEN_SLEEP:
                int unidades_de_trabajo = deserializarOperacionIOGENSLEEP(paquete_kernel->buffer);
                sleep(unidades_de_trabajo * configuracion.TIEMPO_UNIDAD_TRABAJO);
                t_buffer* buffer = crearBufferGeneral(0);
                enviarBufferProcesoConMotivo(buffer, P_IO_GEN_SLEEP, conexion_kernel);
                break;

            default: 
                fprintf(stderr, "No se encontro la operacion");            
        }
    }
    
    log_destroy(logger);
    config_destroy(config);
    return 0;
}
