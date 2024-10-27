#ifndef Log_h
#define Log_h

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