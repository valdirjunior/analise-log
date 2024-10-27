#include <stdlib.h>
#include <stdio.h>
#include "Tarefa.h"
#include "OutputData.h"
#include "Log.h"

//Inicializa uma noca tarefa
Tarefa *criarTarefa()
{
    Tarefa *novaTarefa = (Tarefa *)malloc(sizeof(Tarefa));

    novaTarefa->head = NULL;
    novaTarefa->tail = NULL;
    novaTarefa->next = NULL;
    novaTarefa->outputData = criarOutputData();
    novaTarefa->size = 0;

    return novaTarefa;
}

//Limpa uma tarefa
void limparTarefa(Tarefa *tarefa)
{
    Log* log;
    while (tarefa->size > 0) {
        limparLog(removerLog(tarefa));
    }
    free(tarefa);
}

//Insere um log na tarefa
void inserirLog(Tarefa* tarefa, Log* log)
{
    if (tarefa->head == NULL) {
        tarefa->head = log;
        tarefa->tail = log;
    } else {
        tarefa->tail->next = log;
        log->prev = tarefa->tail;
        tarefa->tail = log;
    }
}

//Remove um log da tarefa
Log* removerLog(Tarefa* tarefa)
{
    Log* log = tarefa->head;
    tarefa->head = tarefa->head->next;
    if (tarefa->head == NULL) {
        tarefa->tail = NULL;
    } else {
        tarefa->head->prev = NULL;
    }
    return log;
}

//Faz a filtragem das informações, separando por dia, hora, e verificando o status
void processarTarefa(Tarefa *tarefa) {
    int dia, hora, status;
    Log *aux = tarefa->head;
    while (NULL != aux) {
        dia = aux->dia;
        hora = aux->hora;
        status = aux->status;

        aux = aux->next;

        switch (dia) {
            case 23:
                tarefa->outputData->cont[0][hora][0]++;
                if(status == 200)
                    tarefa->outputData->cont[0][hora][1]++;
                break;
            case 24:
                tarefa->outputData->cont[1][hora][0]++;
                if(status == 200)
                    tarefa->outputData->cont[1][hora][1]++;
                break;
            case 25:
                tarefa->outputData->cont[2][hora][0]++;
                if(status == 200)
                    tarefa->outputData->cont[2][hora][1]++;
                break;
            default:
                break;
        }
        limparLog(removerLog(tarefa));
    }
}