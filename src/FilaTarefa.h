#ifndef FilaTarefa_h
#define FilaTarefa_h
#include "Tarefa.h"

//Estrutura base da fila de Tarefas
typedef struct sFilaTarefas {
    struct sTarefa *head;
    struct sTarefa *tail;
    int size;
    int fimArquivo;
} FilaTarefas;

FilaTarefas* criarFilaTarefas();
void limparFilaTarefas(FilaTarefas*);

void inserirTarefa(FilaTarefas*, Tarefa*);
Tarefa *removerTarefa(FilaTarefas*);


#endif