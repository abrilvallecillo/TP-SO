#include "hilos.h"
#include <stdio.h>
#include <stdlib.h>

pthread_t crearHilo(void * (*funcionDeHilo)(void *), void * parametroDeFuncion) {
    pthread_t hilo;
    int seCreoBien = pthread_create(&hilo, NULL, funcionDeHilo, parametroDeFuncion);
    if(seCreoBien) {
        fprintf(stderr, "Hubo un error al crear el Hilo");
        abort();
    }
    return hilo;
}

void crearMutex(pthread_mutex_t mutex) {
    int  mutex_mal_creado = pthread_mutex_init(mutex, NULL);
    if(mutex_mal_creado) {
        fprintf(stderr, "Error al crear el mutex");
        abort();
    }
}