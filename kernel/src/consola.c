#include "consola.h"
#include <readline/readline.h>
#include <commons/string.h>
#include "kernel.h"
#include <stdlib.h>


void * consola(void * parametro_nulo) {
    char * leer_cadena = readline("> ");

    while(1) {
        leer_linea(leer_cadena);
        free(leer_cadena);
        leer_cadena = readline("> ");
    }
    free(leer_cadena);
    return NULL;
}

void ejecutarComando(char *comando, char *parametro) {
    if (strcmp(comando, "EJECUTAR_SCRIPT")){
        FILE *script_comandos = fopen(parametro, "r");
        char *lectura_linea;
        if (script_comandos == NULL){fprintf(stderr, "El archivo esta vacio"); abort();}
        while(fgets(lectura_linea, 256, script_comandos)){
            leer_linea(lectura_linea);
        }
        fclose(script_comandos);
    }
    if (strcmp(comando, "INICIAR_PROCESO")){
        inicializar_proceso * peticion = crearInicializarProceso(parametro, pid_nuevo_proceso);
        agregarACola(cola_memoria, peticion);
        pid_nuevo_proceso++;
    }
    if (strcmp(comando, "MULTIPROGRAMACION")){
        grado_multiprogramacion = (int) strtol(parametro, NULL, 10);
    }
}

void leer_linea(char* leer_cadena){
    char** array_cadena = string_split(leer_cadena, " ");
    ejecutarComando(array_cadena[0], array_cadena[1]);
     string_array_destroy(array_cadena);
}