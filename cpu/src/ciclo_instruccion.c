#include "ciclo_instruccion.h"
#include "instrucciones.c"
#include <utils/procesos.h>
#include <utils/conexiones.h>

void cicloDeInstruccion(pcb* contextoDeEjecucion){
    (contextoDeEjecucion->PC)++;
};

// Buscar próxima instrucción a ejecutar
// Cada instrucción deberá ser pedida al módulo Memoria utilizando el Program Counter 
// Al finalizar el ciclo, este último deberá ser actualizado (sumarle 1) si corresponde.
void * fetch(uint32_t pid, uint32_t pc){
    char* proximaInstruccion = pedirInstruccion(pid, pc);
};

// Interpretar qué instrucción es la que se va a ejecutar
// Si la misma requiere de una traducción de dirección lógica a dirección física.

void * decode(){

};

// Ejecutar lo correspondiente a cada instrucción -- Archivo instrucciones 
void * execute(){
    
};

char * pedirInstruccion(uint32_t pid, uint32_t pc){ 
    
    t_buffer * buffer = crearBufferGeneral(
        sizeof(uint32_t)   //PID
        + sizeof(uint32_t) //PC
    );

    agregarABufferUint32(buffer, pid);
    agregarABufferUint32(buffer, pc);

    enviarBufferPorPaquete(buffer, PAQUETE, conexion_memoria);
    t_paquete * paqueteConInstruccion = recibirPaqueteGeneral(conexion_memoria);

    return leerDeBufferString(paqueteConInstruccion->buffer, 128);

};

void * traducirMemoria(){}