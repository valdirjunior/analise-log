#ifndef Log_h
#define Log_h

typedef struct sLog {
    char *hora;
    char *dia;
    int status;
    struct sLog *next;
    struct sLog *prev;
} Log;

Log* criarLog(char*, char*, int);
void limparLog(Log*);

#endif