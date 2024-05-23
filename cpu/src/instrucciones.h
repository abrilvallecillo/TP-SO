#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

void SET(int *registro, int valor);
void MOV_IN(int *registroDatos, int *registroDireccion);
void MOV_OUT(int *registroDireccion, int *registroDatos);
void SUM(int *registroDestino, int *registroOrigen);
void SUB(int *registroOrigen, int *registroDestino);
void JNZ(int *registro, int nroInstruccion);
void RESIZE(int tamanio);
void COPY_STRING(int tamanio);
// void WAIT (Recurso)
// void SIGNAL (Recurso)
void IO_GEN_SLEEP(char *interfaz, int unidadesDeT rabajo);
void IO_STDIN_READ(char* interfaz, int *registroDireccion, int *registroTamanio);
void IO_STDOUT_WRITE(char* interfaz, int *registroDireccion, int *registroTamanio);
void IO_FS_CREATE(char* interfaz, char* nombreArchivo);
void IO_FS_DELETE(char* interfaz, char* nombreArchivo);
void IO_FS_TRUNCATE(char* interfaz, char* nombreArchiv, int *registroTamanio);
void IO_FS_WRITE(char* interfaz, char* nombreArchiv,int *registroDireccion, int *registroTamanio);
void IO_FS_READ(char* interfaz, char* nombreArchiv, int *registroTamanio, int *registropunteroArchivo);
void EXIIT();
#endif