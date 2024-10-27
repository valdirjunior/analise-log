#ifndef Log_h
#define Log_h

//Estrutura base de cada registro
typedef struct sLog {
    int hora;
    int dia;
    int status;
    struct sLog *next;
    struct sLog *prev;
} Log;

Log* criarLog(int, int, int);
void limparLog(Log*);

#endif