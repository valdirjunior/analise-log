#ifndef Log_h
#define Log_h

typedef struct sLog {
    char *line;
    struct sLog *next;
    struct sLog *prev;
} Log;

Log* criarLog(char*);
void limparLog(Log*);

#endif