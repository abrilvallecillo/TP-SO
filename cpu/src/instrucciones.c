#include "instrucciones.h"
#include <utils/procesos.h>
#include <stdio.h>
#include "cpu.h"

// Definición de los registros y otros elementos necesarios

// Función SET: Asigna al registro el valor pasado como parámetro.
void SET(int *registro, int valor) { *registro = valor; }

// Función MOV_IN: Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro Dirección y lo almacena en el Registro Datos.
void MOV_IN(int *registroDatos, int *registroDireccion) {}

// Función MOV_OUT: Lee el valor del Registro Datos y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
void MOV_OUT(int *registroDireccion, int *registroDatos) {}

// Función SUM: Suma al Registro Destino el Registro Origen y deja el resultado en el Registro Destino.
void SUM(int *registroDestino, int *registroOrigen) { *registroDestino += *registroOrigen; }

// Función SUB: Resta al Registro Destino el Registro Origen y deja el resultado en el Registro Destino.
void SUB(int *registroOrigen, int *registroDestino) { *registroDestino -= *registroOrigen; }

// Función JNZ: Si el valor del registro es distinto de cero, actualiza el program counter al número de instrucción pasada por parámetro.
void JNZ(int *registro, int nroInstruccion) {
        if (*registro) { registrosPropios.PC = nroInstruccion; }
}
// Función RESIZE: Solicitará a la Memoria ajustar el tamaño del proceso al tamaño pasado por parámetro. En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto de ejecución al Kernel informando de esta situación.
void RESIZE(int tamanio) {}

// Función COPY_STRING: Toma del string apuntado por el registro SI y copia la cantidad de bytes indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI. 
void COPY_STRING(int tamanio) {}

// Función WAIT (Recurso): Esta instrucción solicita al Kernel que se asigne una instancia del recurso indicado por parámetro.
void WAIT(char ** recurso) {}

// Función SIGNAL (Recurso): Esta instrucción solicita al Kernel que se libere una instancia del recurso indicado por parámetro.
void SIGNAL(char ** recurso) {}

// Función IO_GEN_SLEEP
void IO_GEN_SLEEP(char* interfaz, int unidadesDeTrabajo){
    pcb_IOGENSLEEP* pcbIO = malloc(sizeof(pcb_IOGENSLEEP));
    pcbIO->unidades_de_trabajo = unidadesDeTrabajo;
    pcbIO->nombre_interfaz = interfaz;
    pcbIO->longitud_nombre_interfaz = strlen(interfaz) + 1;
    //TODO: pcb_IO->contexto
    t_buffer* buffer = serializarProcesoIOGENSLEEP(pcbIO);
    enviarBufferProcesoConMotivo(buffer, P_IO_GEN_SLEEP, conexion_dispatch_kernel);
}

// Función IO_STDIN_READ: Esta instrucción solicita al Kernel que mediante la interfaz ingresada se lea desde el STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del Registro Tamaño y el mismo se guarde a partir de la Dirección Lógica almacenada en el Registro Dirección.
void IO_STDIN_READ(char* interfaz, int *registroDireccion, int *registroTamanio) {}

// Función IO_STDOUT_WRITE: Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde la posición de memoria indicada por la Dirección Lógica almacenada en el Registro Dirección, un tamaño indicado por el Registro Tamaño y se imprima por pantalla.
void IO_STDOUT_WRITE(char* interfaz, int *registroDireccion, int *registroTamanio) {}

// FunciónFunciónIO_FS_CREATE: Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se cree un archivo en el FS montado en dicha interfaz.
void IO_FS_CREATE(char* interfaz, char* nombreArchivo) {}

// Función IO_FS_DELETE: Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se elimine un archivo en el FS montado en dicha interfaz
void IO_FS_DELETE(char* interfaz, char* nombreArchivo) {}

// Función IO_FS_TRUNCATE: Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se modifique el tamaño del archivo en el FS montado en dicha interfaz, actualizando al valor que se encuentra en el registro indicado por Registro Tamaño.
void IO_FS_TRUNCATE(char* interfaz, char* nombreArchiv, int *registroTamanio) {}

// Función IO_FS_WRITE: Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde Memoria la cantidad de bytes indicadas por el Registro Tamaño a partir de la dirección lógica que se encuentra en el Registro Dirección y se escriban en el archivo a partir del valor del Registro Puntero Archivo.
void IO_FS_WRITE(char* interfaz, char* nombreArchiv,int *registroDireccion, int *registroTamanio) {}

// Función IO_FS_READ: Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde el archivo a partir del valor del Registro Puntero Archivo la cantidad de bytes indicada por Registro Tamaño y se escriban en la Memoria a partir de la dirección lógica indicada en el Registro Dirección.
void IO_FS_READ(char* interfaz, char* nombreArchiv, int *registroTamanio, int *registropunteroArchivo) {}

// Función EXIT: Esta instrucción representa la syscall de finalización del proceso. Se deberá devolver el Contexto de Ejecución actualizado al Kernel para su finalización.
void EXIIT()