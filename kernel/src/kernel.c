#include "kernel.h"
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <utils/hilos.h>

cola_sincronizada * cola_new;
cola_sincronizada * cola_ready;
cola_sincronizada * cola_ready_vrr;
cola_sincronizada * cola_exit;
cola_sincronizada * cola_memoria;
algoritmo algoritmo_elegido;
t_list * lista_interfaces;
sem_t * solicitaron_finalizar_proceso;
sem_t * no_surgio_otra_interrupcion;
sem_t * hay_peticion_en_cola;
pthread_t hilo_timer;
pthread_mutex_t mutex_lista_interfaces;
char * nombre_interfaz_a_comparar;
entero_sincronizado grado_multiprogramacion;
int pid_nuevo_proceso;

void iniciarEnteroSincronizado(entero_sincronizado * entero_sincro) {
    entero_sincro.entero = 0;
    crearMutex(entero_sincro->mutex_entero);
}

void incrementarEnteroSincronizado(entero_sincronizado * entero_sincro) {
    pthread_mutex_lock(entero_sincro->mutex_entero);
    entero_sincro->entero++;
    pthread_mutex_unlock(entero_sincro->mutex_entero);
}

void iniciarPID() {
    pid_nuevo_proceso = 0;
    crearMutex(mutex_pid_nuevo_proceso);
}

algoritmo algoritmoPorString(char* algoritmo) {
    if(strcmp(algoritmo, "FIFO") == 0) 
        return FIFO;
    else if(strcmp(algoritmo, "RR") == 0)
        return RR;
    else
        return VRR;
}

void iniciarCola(cola_sincronizada * cola_elegida) {
    cola_elegida = malloc(sizeof(cola_sincronizada));
    cola_elegida->cola = queue_create();
    if(!cola_elegida->cola) {
        fprintf(stderr, "Error al crear la cola");
        abort();
    }
    crearMutex(cola_elegida->mutex_cola);
}

void agregarACola(cola_sincronizada * cola_elegida, void * valor) {
    pthread_mutex_lock(cola_elegida->mutex_cola);
    queue_push(cola_elegida->cola, valor);
    pthread_mutex_unlock(cola_elegida->mutex_cola);
}
void * sacarDeCola(cola_sincronizada * cola_elegida) {
    pthread_mutex_lock(cola_elegida->mutex_cola);
    void * valor = queue_pop(cola_elegida->cola);
    pthread_mutex_unlock(cola_elegida->mutex_cola);
    return valor;
}

void eliminarColaSincronizada(cola_sincronizada * cola_elegida) {
    queue_destroy(cola_elegida->cola);
    pthread_mutex_destroy(cola_elegida->mutex_cola);
    free(cola_elegida);
}

void avisarCambioEstado(uint32_t pid, const char * estado_anterior, const char * estado_actual) {
    char * aviso_cambio_estado = string_from_format("PID: %d - Estado Anterior: %s - Estado Actual: %s ", pid, estado_anterior, estado_actual);
    logInfoSincronizado(aviso_cambio_estado);
}

bool existeInterfazEnSistema(char * nombre_interfaz) {
    pthread_mutex_lock(mutex_lista_interfaces);
    bool resultado = list_any_satisfy(lista_interfaces, interfazTieneNombre);
    pthread_mutex_unlock(mutex_lista_interfaces);
    return resultado;
}

bool interfazTieneNombre(void * interfaz) {
    t_interfaz * interfaz_elegida = (t_interfaz *) interfaz;
    return !strcmp(nombre_interfaz_a_comparar, interfaz_elegida->nombre_interfaz);
}
bool interfazPuedeRealizarOperacion(codigos_operacion_proceso codigo) {
    t_interfaz * interfaz_encontrada = t_interfaz * encontrarInterfazEnLista();
    
    switch(codigo){
        case P_IO_GEN_SLEEP:
            return interfaz_encontrada->tipo_de_interfaz == codigo;
        default:
            fprintf(stderr, "Error: operacion desconocida!");
            return false;
    }
}

t_interfaz * encontrarInterfazEnLista() {
    pthread_mutex_lock(mutex_lista_interfaces);
    t_interfaz * interfaz_encontrada = (t_interfaz *) list_find(lista_interfaces, interfazTieneNombre);
    pthread_mutex_unlock(mutex_lista_interfaces);
    return interfaz_encontrada;
}

void agregarInterfazALista(t_interfaz * interfaz_actual) {
    pthread_mutex_lock(mutex_lista_interfaces);
    list_add(lista_interfaces, interfaz_actual);
    pthread_mutex_unlock(mutex_lista_interfaces);
}


