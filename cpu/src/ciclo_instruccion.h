#ifndef CICLO_INSTRUCCION_H_
#define CICLO_INSTRUCCION_H_

void cicloDeInstruccion();
void * fetch();
void * decode();
void * execute();
char * pedirInstruccion(uint32_t pid, uint32_t pc);
void * traducirMemoria();

#endif