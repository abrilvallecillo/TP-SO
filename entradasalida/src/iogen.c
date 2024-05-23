#include "iogen.h"
#include <utils/interfaces.h>
#include <commons/string.h>

info_interfaz* crearTipoInterfaz(char* nombre, char* t_interfaz){
    info_interfaz* tipo_interfaz = malloc(sizeof(info_interfaz));
    tipo_interfaz->nombre = string_duplicate(nombre);
    tipo_interfaz->longitud_nombre = string_length(nombre) + 1;
    tipo_interfaz->tipo = casteoStringInterfaz(t_interfaz);
    return tipo_interfaz;
}

int casteoStringInterfaz(char* tipo_interfaz){
    if(strcmp("GENERICA", tipo_interfaz)){
        return GENERICA;
    }
}