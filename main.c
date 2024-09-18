#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 4
#define NUM_LOGS_POR_TAREFA 1000

int cont_logs[3][24][2] = {0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t fila_mutex;
// pthread_mutex_t cont_mutex;
pthread_cond_t fila_aviso;


typedef struct sLog {
    char *line;
    struct sLog *next;
    struct sLog *prev;
}Log;

typedef struct sTarefa {
    struct sLog *head;
    struct sLog *tail;
    struct sTarefa *next;
    int size;
}Tarefa;

typedef struct sFilaTarefas{
    struct sTarefa *head;
    struct sTarefa *tail;
    int size;
    int fimArquivo;
}FilaTarefas;

typedef struct sContador{
    int cont[3][24][2];
}Contador;

void processaAccessLog(FILE*, FilaTarefas*);
void inicializarFila(FilaTarefas*);
void limpaFila(FilaTarefas*);
void limpaTarefa(Tarefa*);
Tarefa* criaTarefa();
Log* criarLog(char*, Log*, Log*);
void insereNaFila(FilaTarefas*, Tarefa*);
Tarefa *removeTarefa(FilaTarefas*);
void* thread_function(void*);
void contaLogs(Tarefa*, Contador*);
Contador* criaContador(int);
void insereLinhaTarefa(Tarefa*, char*);

int main() {
    FILE *file;
    file = fopen("log/access.log", "r");


    pthread_cond_init(&fila_aviso, NULL);
    
    FilaTarefas *filaTarefas = malloc(sizeof(FilaTarefas));
    inicializarFila(filaTarefas);

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i <NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void*)filaTarefas);
    }

    processaAccessLog(file, filaTarefas);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&fila_aviso);

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
        printf("Logs no dia %d/Jan/2024:\n\n", dia);
        for (int j = 0; j < 24; j++) {
            printf("%d Horas: %d || Status 200: %d \n", j, cont_logs[i][j][0], cont_logs[i][j][1]);
        }
        printf("\n\n");
    }

    limpaFila(filaTarefas);

    return 0;
}

void processaAccessLog(FILE* file, FilaTarefas* filaTarefas) {
    // char line[512];
    char data[12], hora[10];
    int status;

    if (file == NULL) {
        printf("Erro ao abrir o arquivo!\n");
    } else {
        Tarefa *novaTarefa = NULL;
        char *line = malloc(sizeof(char)*512);
        int i = 0;

        while (fgets(line, sizeof(line), file)) {
            if((i % 1000) == 0) {
                if(novaTarefa != NULL) {
                    insereNaFila(filaTarefas, novaTarefa);
                }
                novaTarefa = criaTarefa();
            }
            insereLinhaTarefa(novaTarefa, line);
            i++;
            line = malloc(sizeof(char)*512);
        }
        pthread_mutex_lock(&mutex);
        filaTarefas->fimArquivo = 1;
        pthread_cond_broadcast(&fila_aviso);
        pthread_mutex_unlock(&mutex);
    }
    fclose(file);
}

void inicializarFila(FilaTarefas* filaTarefas) {
    filaTarefas->head = NULL;
    filaTarefas->tail = NULL;
    filaTarefas->size = 0;
    filaTarefas->fimArquivo = 0;
}

void limpaFila(FilaTarefas* filaTarefas) {
    Tarefa* task = filaTarefas->head;
    Tarefa* taskTemp;
    while(task != NULL) {
        taskTemp = task;
        task = task->next;
        limpaTarefa(taskTemp);
    }
    free(filaTarefas);
}

void limpaTarefa(Tarefa* tarefa) {
    Log* log = tarefa->head;
    Log* logTemp;
    while(log != NULL) {
        logTemp = log;
        log = log->next;
        free(logTemp);
    }
    free(tarefa);
}


void insereNaFila(FilaTarefas* filaTarefas, Tarefa *tarefa) {    
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

Tarefa* criaTarefa() {
    Tarefa *novaTarefa = (Tarefa*)malloc(sizeof(Tarefa));
    novaTarefa->head = NULL;
    novaTarefa->tail = NULL;
    novaTarefa->next = NULL;
    novaTarefa->size = 0;
}

Log *criarLog(char *line, Log* next, Log* prev) {
    Log* newLog = (Log*)malloc(sizeof(Log));
    newLog->line = line;
    newLog->next = next;
    newLog->prev = prev;
}

Tarefa *removeTarefa(FilaTarefas *filaTarefas) {
    Tarefa *aux = filaTarefas->head;
    filaTarefas->head = filaTarefas->head->next;
    filaTarefas->size--;
    return aux;
}

void *thread_function(void *arg) {
    FilaTarefas *filaTarefas = (FilaTarefas*)arg;
    Contador *contThread = (Contador*)malloc(sizeof(Contador));
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 24; j++) {
            for(int k = 0; k < 0; k++){
                contThread->cont[i][j][k] = 0;
            }
        }
    }

    while (1) {        

        pthread_mutex_lock(&mutex);
        if(NULL != filaTarefas->head) {
            int tamanhoTarefa = filaTarefas->head->size;
            if((tamanhoTarefa % NUM_LOGS_POR_TAREFA) == 0) {
                Tarefa *task = removeTarefa(filaTarefas);
                pthread_mutex_unlock(&mutex);
                
                contaLogs(task, contThread);
                
                limpaTarefa(task);
            }
        } else {
            if (filaTarefas->fimArquivo) {
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
    }
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 24; j++) {
            for(int k = 0; k < 0; k++){
               cont_logs[i][j][k] += contThread->cont[i][j][k];
            }
        }
    }
    return NULL;
}
void contaLogs(Tarefa *tarefa, Contador *contador) {
    char data[12];
    char hour[10];
    int dia, hora, status;
    Log *aux = tarefa->head;
    while (NULL != aux) {
        sscanf(aux->line, "%*[^ ] - %*[^ ] [%[^:]:%[^ ] %*[^]]] \"%*s %*s %*[^\"]\" %d", data, hour, &status);
        sscanf(data, "%d/%*d/%*d", &dia);
        sscanf(hour, "%d:%*d:%*d", &hora);

        switch (dia){
        case 23:
            contador->cont[0][hora][0]++;
            if(status == 200)
                contador->cont[0][hora][1]++;
            break;
        case 24:
            contador->cont[1][hora][0]++;
            if(status == 200)
                contador->cont[1][hora][1]++;
            break;
        case 25:
            contador->cont[2][hora][0]++;
            if(status == 200)
                contador->cont[2][hora][1]++;
            break;
        default:
            break;
        }
        aux = aux->next;
    }
}

void insereLinhaTarefa(Tarefa *tarefa, char *line) {
    if(tarefa->head != NULL) {
        Log *newLog = criarLog(line, NULL, tarefa->tail);
        tarefa->tail->next = newLog;
        tarefa->tail = tarefa->tail->next;
        tarefa->size++;
    } else {
        Log *newLog = criarLog(line, NULL, NULL);
        tarefa->head = newLog;
        tarefa->tail = tarefa->head;
        tarefa->size++;
    }
}