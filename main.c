#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "src/Log.h"
#include "src/FilaTarefa.h"

#define NUM_THREADS 4
#define NUM_LOGS_POR_TAREFA 1000


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_saida = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fila_aviso = PTHREAD_COND_INITIALIZER;

typedef struct {
    FilaTarefas* filaTarefas;
    OutputData* outputData;
} ThreadArgs;

ThreadArgs* criarThreadArgs(FilaTarefas*, OutputData*);
void processarArquivo(FILE*, FilaTarefas*, OutputData*);
void* thread_function(void*);

int main() {
    FILE *file;
    file = fopen("log/access.log", "r");
    if (file == NULL) {
        printf("Não foi possível abrir o arquivo.");
        return 1;
    }

    FilaTarefas *filaTarefas = criarFilaTarefas();
    OutputData *outputData = criarOutputData();

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i <NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void*)criarThreadArgs(filaTarefas, outputData));
    }

    processarArquivo(file, filaTarefas, outputData);

    fclose(file);
    while (filaTarefas->size > 0) {
        sleep(1);
        printf("Fila de tarefas: %d\n", filaTarefas->size);
    }

    filaTarefas->fimArquivo = 1;
    pthread_cond_broadcast(&fila_aviso);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&fila_aviso);

    int totalLogs, totalLogs200;
    // Escrever os resultados:
    for(int i = 0; i < 3; i++) {
        int dia;
        switch (i) {
        case 0:
            dia = 23;
            break;
        case 1:
            dia = 24;
            break;
        case 2:
            dia = 25;
            break;
        default:
            break;
        }
        printf("\nLogs no dia %d/Jan/2019:\n\n", dia);
        for (int j = 0; j < 24; j++) {
            printf("%d Horas: %d || Status 200: %d \n", j, outputData->cont[i][j][0], outputData->cont[i][j][1]);
            totalLogs += outputData->cont[i][j][0];
            totalLogs200 += outputData->cont[i][j][1];
        }
        printf("\n\n");
    }
    printf("Total de acessos: %d || Total de acessos com sucesso: %d\n\n", totalLogs, totalLogs200);

    limparFilaTarefas(filaTarefas);

    return 0;
}

void processarArquivo(FILE* file, FilaTarefas* filaTarefas, OutputData* outputData) {

    Tarefa *novaTarefa = NULL;
    char line[4096];
    char date[12];
    char hour[10];
    int status;
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        if((i % 1000) == 0) {
            if(novaTarefa != NULL) {
                pthread_mutex_lock(&mutex);
                inserirTarefa(filaTarefas, novaTarefa);
                pthread_mutex_unlock(&mutex);
                pthread_cond_signal(&fila_aviso);
            }
            novaTarefa = criarTarefa();
        }

        sscanf(line, "%*[^ ] - %*[^ ] [%[^:]:%[^ ] %*[^]]] \"%*s %*s %*[^\"]\" %d", date, hour, &status);
        inserirLog(novaTarefa, criarLog(date, hour, status));
        i++;
    }
    pthread_mutex_lock(&mutex);
    filaTarefas->fimArquivo = 1;

    pthread_mutex_unlock(&mutex);
}



void *thread_function(void *arg) {

    FilaTarefas *filaTarefas = ((ThreadArgs*)arg)->filaTarefas;
    OutputData *outputData = ((ThreadArgs*)arg)->outputData;
    Tarefa* tarefa;

    while (1) {
        pthread_mutex_lock(&mutex);
        while ((filaTarefas->size == 0) && (!filaTarefas->fimArquivo)) {
            pthread_cond_wait(&fila_aviso, &mutex);
        }
        if (filaTarefas->fimArquivo == 1) {
            break;
        }

        tarefa = removerTarefa(filaTarefas);

        pthread_mutex_unlock(&mutex);
        processarTarefa(tarefa);
        pthread_mutex_lock(&mutex_saida);
        contabilizarDados(outputData, tarefa->outputData);
        pthread_mutex_unlock(&mutex_saida);
        limparTarefa(tarefa);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

ThreadArgs* criarThreadArgs(FilaTarefas* filaTarefas, OutputData* outputData)
{
    ThreadArgs* threadArgs;
    threadArgs = malloc(sizeof(ThreadArgs));
    threadArgs->filaTarefas = filaTarefas;
    threadArgs->outputData = outputData;
    return threadArgs;
}