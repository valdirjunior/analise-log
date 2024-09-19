#ifndef Tarefa_h
#define Tarefa_h

#include "Log.h"
#include "OutputData.h"

typedef struct sTarefa {
    Log *head;
    Log *tail;
    OutputData* outputData;
    struct sTarefa *next;
    int size;
} Tarefa;

Tarefa* criarTarefa();
void limparTarefa(Tarefa*);

void inserirLog(Tarefa*, Log*);
Log* removerLog(Tarefa*);

void processarTarefa(Tarefa*);

#endif