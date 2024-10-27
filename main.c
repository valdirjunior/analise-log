#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <regex.h>

#include "src/Log.h"
#include "src/FilaTarefa.h"

#define NUM_THREADS 4               //Número de threads. Pode ser alterado.
#define NUM_LOGS_POR_TAREFA 1000    //Número máximo de logs de cada tarefa.

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_saida = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fila_aviso = PTHREAD_COND_INITIALIZER;

typedef struct {
    FilaTarefas* filaTarefas;
    OutputData* outputData;
} ThreadArgs;

ThreadArgs* criarThreadArgs(FilaTarefas*, OutputData*);
void processarArquivo(FILE*, FilaTarefas*, OutputData*);
void processarArquivoREGEX(FILE*, FilaTarefas*, OutputData*);
void* thread_function(void*);
void imprimirLinhaInvalida(const char*);

int main() {
    FILE *file;
    file = fopen("log/access.log", "r");  //Abre o arquivo de log, certifique-se de colocar o arquivo no caminho certo
    if (file == NULL) {
        printf("Não foi possível abrir o arquivo.");
        return 1;
    }

    FilaTarefas *filaTarefas = criarFilaTarefas();
    OutputData *outputData = criarOutputData();

    pthread_t threads[NUM_THREADS];
    //Inicializa as threads
    for (int i = 0; i <NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void*)criarThreadArgs(filaTarefas, outputData));
    }
    //Faz a triagem do arquivo
    processarArquivoREGEX(file, filaTarefas, outputData);

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

//Função antiga usando sscanf, não utilizada na versão atual
void processarArquivo(FILE* file, FilaTarefas* filaTarefas, OutputData* outputData) {

    Tarefa *novaTarefa = NULL;
    char line[32768];
    // char date[12];
    // char hour[10];
    int status, date, hour;
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        if((i % NUM_LOGS_POR_TAREFA) == 0) {
            if(novaTarefa != NULL) {
                pthread_mutex_lock(&mutex);
                inserirTarefa(filaTarefas, novaTarefa);
                pthread_mutex_unlock(&mutex);
                pthread_cond_signal(&fila_aviso);
            }
            novaTarefa = criarTarefa();
        }

        // sscanf(line, "%*[^ ] - %*[^ ] [%[^:]:%[^ ] %*[^]]] \"%*s %*s %*[^\"]\" %d", date, hour, &status);
        sscanf(line, "%*[^-]- %*[^ ] [%d/%*[^:]:%d%*[^]]] \"%*[^\"]\" %d ", &date, &hour, &status);
        // printf("Status: %s\n", status);
        inserirLog(novaTarefa, criarLog(date, hour, status));
        i++;
    }
    pthread_mutex_lock(&mutex);
    filaTarefas->fimArquivo = 1;

    pthread_mutex_unlock(&mutex);
}
//Função para processar o arquivo utilizando regex 
void processarArquivoREGEX(FILE* file, FilaTarefas* filaTarefas, OutputData* outputData) {
    Tarefa* novaTarefa = NULL;
    char line[32768];
    regex_t regex;
    regmatch_t matches[4];  //4 grupos, o primeiro para a linha, os demais para as informações de interesse
    int status, date, hour;
    int i = 0;

    // Compila a expressao regular
    if (regcomp(&regex, "^[^ ]+ - [^ ]+ \\[([0-9]{2})/[^/]+/[0-9]{4}:([0-9]{2}):[0-9]{2}:[0-9]{2} [^]]+\\] \".*?\" ([0-9]{3})", REG_EXTENDED)) {
        fprintf(stderr, "Erro na compilacao da expressao regular.\n");
        exit(1);
    }
    //Enquanto não chegar o fim do arquivo executa:
    while (fgets(line, sizeof(line), file)) {
        // Se atingir o valor predefinido de registros, cria uma nova tarefa e insere na fila.
        if((i % NUM_LOGS_POR_TAREFA) == 0) {
            if(novaTarefa != NULL) {
                pthread_mutex_lock(&mutex);
                inserirTarefa(filaTarefas, novaTarefa);
                pthread_mutex_unlock(&mutex);
                pthread_cond_signal(&fila_aviso);
            }
            novaTarefa = criarTarefa();
        }
        // Verifica se a linha corresponde à expressão regular. Se sim, extrai as informações relevantes e cria um novo log.
        if (regexec(&regex, line, 4, matches, 0) == 0) {
            line[matches[1].rm_eo] = '\0';
            date = atoi(line + matches[1].rm_so);
            line[matches[2].rm_eo] = '\0';
            hour = atoi(line + matches[2].rm_so);
            line[matches[3].rm_eo] = '\0';
            status = atoi(line + matches[3].rm_so);

            inserirLog(novaTarefa, criarLog(date, hour, status));
        } else { //Senão imprime a linha problemática
            imprimirLinhaInvalida(line);
        }
        i++;
    }

    //Caso alguma tarefa não atinja o tamanho estipulado, é inserida na fila após o final do arquivo
    if (novaTarefa != NULL) {
        pthread_mutex_lock(&mutex);
        inserirTarefa(filaTarefas, novaTarefa);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&fila_aviso);
    }
    pthread_mutex_lock(&mutex);
    filaTarefas->fimArquivo = 1; //Sinaliza que a leitura do arquivo terminou
    pthread_mutex_unlock(&mutex);

    //Libera memória do regex
    regfree(&regex);
}


//Função executada por cada thread
void *thread_function(void *arg) {

    FilaTarefas *filaTarefas = ((ThreadArgs*)arg)->filaTarefas;
    OutputData *outputData = ((ThreadArgs*)arg)->outputData;
    Tarefa* tarefa;

    while (1) {
        pthread_mutex_lock(&mutex);
        //Enquanto a fila estiver vazia e o arquivo não foi lido, aguarda
        while ((filaTarefas->size == 0) && (!filaTarefas->fimArquivo)) {
            pthread_cond_wait(&fila_aviso, &mutex);
        }
        //Se a fila estiver vazia e o arquivo foi lido, sai do loop
        if ((filaTarefas->fimArquivo == 1) && filaTarefas->size == 0) {
            break;
        }
        //Retira a tarefa da fila 
        tarefa = removerTarefa(filaTarefas);

        pthread_mutex_unlock(&mutex);
        processarTarefa(tarefa); //Processa os dados da tarefa para o outputData da task
        pthread_mutex_lock(&mutex_saida);
        contabilizarDados(outputData, tarefa->outputData); //Faz a soma dos dados para o outputData principal
        pthread_mutex_unlock(&mutex_saida);
        limparTarefa(tarefa);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

//Inicializa os argumentos da thread
ThreadArgs* criarThreadArgs(FilaTarefas* filaTarefas, OutputData* outputData)
{
    ThreadArgs* threadArgs;
    threadArgs = malloc(sizeof(ThreadArgs));
    threadArgs->filaTarefas = filaTarefas;
    threadArgs->outputData = outputData;
    return threadArgs;
}

//Imprime a linha problemática
void imprimirLinhaInvalida(const char* linha) {
    fprintf(stderr, "Linha inválida: %s\n", linha);
}