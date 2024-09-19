#include "OutputData.h"
#include <stdlib.h>

OutputData *criarOutputData()
{
    OutputData* outputData;
    outputData = malloc(sizeof(OutputData));
    int i, j, k;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 24; j++) {
            for (k = 0; k < 2; k++) {
                outputData->cont[i][j][k] = 0;
            }
        }
    }
    return outputData;
}

void contabilizarDados(OutputData* outputData, OutputData* outputDataTarefa)
{
    int i, j, k;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 24; j++) {
            for (k = 0; k < 2; k++) {
                outputData->cont[i][j][k] = outputDataTarefa->cont[i][j][k];
            }
        }
    }
}