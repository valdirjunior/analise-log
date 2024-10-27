#ifndef Data_h
#define Data_h

//Matriz do contador
typedef struct  {
    int cont[3][24][2];
} OutputData;

OutputData* criarOutputData();
void contabilizarDados(OutputData*, OutputData*);

#endif