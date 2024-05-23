#ifndef KERNEL_H_
#define KERNEL_H_

#include <commons/collections/queue.h>
#include <utils/conexiones.h>
#include <utils/conexiones.h>
#include <utils/procesos.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
    t_queue * cola;
    pthread_mutex_t mutex_cola;
} cola_sincronizada;

typedef enum {
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
} tipo_interfaz;

typedef struct {
    char * nombre_interfaz;
    tipo_interfaz tipo_de_interfaz;
    sem_t * hay_procesos_en_interfaz;
    cola_sincronizada * cola_bloqueados;
} t_interfaz;

typedef struct {
    int entero;
    pthread_mutex_t mutex_entero;
} entero_sincronizado;

void iniciarCola(cola_sincronizada * cola_elegida);
void agregarACola(cola_sincronizada * cola_elegida, void * valor);
void * sacarDeCola(cola_sincronizada * cola_elegida);
void eliminarColaSincronizada(cola_sincronizada * cola_elegida);

extern cola_sincronizada * cola_new;
extern cola_sincronizada * cola_ready;
extern cola_sincronizada * cola_ready_vrr;
extern cola_sincronizada * cola_exit;
extern cola_sincronizada * cola_memoria;
extern t_list * lista_interfaces;
extern sem_t * solicitaron_finalizar_proceso;
extern sem_t * no_surgio_otra_interrupcion;
extern sem_t * hay_peticion_en_cola;
extern pthread_t hilo_timer;
extern pthread_mutex_t mutex_lista_interfaces;
extern char * nombre_interfaz_a_comparar;
extern entero_sincronizado grado_multiprogramacion;
extern int pid_nuevo_proceso;

typedef struct {
    datos_conexion_h conexion;
    char * tipo_de_algoritmo;
} datos_planificacion;


typedef enum {
    FIFO,
    RR,
    VRR
} algoritmo;

extern algoritmo algoritmo_elegido;

algoritmo algoritmoPorString(char* algoritmo);

void avisarCambioEstado(uint32_t pid, const char * estado_anterior, const char * estado_actual);

bool existeInterfazEnSistema(char * nombre_interfaz);
bool interfazPuedeRealizarOperacion(codigos_operacion_proceso codigo);
t_interfaz * encontrarInterfazEnLista();

void agregarInterfazALista(t_interfaz * interfaz_actual);
void iniciarEnteroSincronizado(entero_sincronizado * entero_sincro);
void incrementarEnteroSincronizado(entero_sincronizado * entero_sincro);

#endif