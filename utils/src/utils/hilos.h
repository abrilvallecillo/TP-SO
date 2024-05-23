#ifndef UTILS_HILOS_H_
#define UTILS_HILOS_H_

#include <pthread.h>

pthread_t crearHilo(void *(*funcionAUsar)(void *), void * parametroDeFuncion);

void crearMutex(pthread_mutex_t mutex);
#endif