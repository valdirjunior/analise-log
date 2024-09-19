#include <stdlib.h>
#include "FilaTarefa.h"

FilaTarefas *criarFilaTarefas()
{
    FilaTarefas *filaTarefas = NULL;
    filaTarefas = malloc(sizeof(FilaTarefas));

    filaTarefas->head = NULL;
    filaTarefas->tail = NULL;
    filaTarefas->size = 0;
    filaTarefas->fimArquivo = 0;

    return filaTarefas;
}

void limparFilaTarefas(FilaTarefas* filaTarefas)
{
    Tarefa* tarefa;
    while (filaTarefas->size > 0) {
        tarefa = removerTarefa(filaTarefas);
        limparTarefa(tarefa);
    }
}

Tarefa *removerTarefa(FilaTarefas *filaTarefas)
{
    Tarefa *aux = filaTarefas->head;
    filaTarefas->head = filaTarefas->head->next;
    filaTarefas->size--;
    return aux;
}

void inserirTarefa(FilaTarefas *filaTarefas, Tarefa *tarefa)
{
    if(filaTarefas->head != NULL) {
        filaTarefas->tail->next = tarefa;
        filaTarefas->tail = filaTarefas->tail->next;
        filaTarefas->size++;
    } else {
        filaTarefas->head = tarefa;
        filaTarefas->tail = filaTarefas->head;
        filaTarefas->size++;
    }
}