#include "Log.h"
#include <stdlib.h>

//Cria um novo log
Log *criarLog(int dia, int hora, int status)
{
    Log* newLog = (Log*)malloc(sizeof(Log));
    newLog->hora = hora;
    newLog->dia = dia;
    newLog->status = status;
    newLog->next = NULL;
    newLog->prev = NULL;
    return newLog;
}

//Limpa um log
void limparLog(Log* log)
{
    free(log);
}