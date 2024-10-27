#include <stdlib.h>
#include <stdio.h>
#include "Tarefa.h"
#include "OutputData.h"
#include "Log.h"

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

void limparTarefa(Tarefa *tarefa)
{
    Log* log;
    while (tarefa->size > 0) {
        limparLog(removerLog(tarefa));
    }
    free(tarefa);
}

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

void processarTarefa(Tarefa *tarefa) {
    int dia, hora, status;
    // char status_char[3];
    Log *aux = tarefa->head;
    while (NULL != aux) {
        // sscanf(aux->dia, "%d/%*d/%*d", &dia);
        // sscanf(aux->hora, "%d:%*d:%*d", &hora);
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