#include "Log.h"
#include <stdlib.h>

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

void limparLog(Log* log)
{
    free(log);
}