#include "Log.h"
#include <stdlib.h>

Log *criarLog(char *line)
{
    Log* newLog = (Log*)malloc(sizeof(Log));
    newLog->line = line;
    newLog->next = NULL;
    newLog->prev = NULL;
    return newLog;
}

void limparLog(Log* log)
{
    free(log->line);
    free(log);
}