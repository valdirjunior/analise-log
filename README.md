# Análise de logs # 

Disciplina: Sistemas Operacionais.

Tema: Processamento Multi-threads.

Aluno: Valdir Rugiski Junior.

Para o aprendizado sobre processamento multi-thread foi desenvolvido uma aplicação em `C` para a leitura e extração de dados de acessos a um servidor web. O arquivo conta com 5 milhões de registros. O objetivo é realizar a contagem de acessos com status 200. É aí que as threads entram em ação. Foi criada uma fila de tarefas, onde cada tarefa inserida contém 1000 registros*, que serão contabilizados dentro da thread.

***A excessão é a última tarefa, que deve ter 999, visto que o último registro do arquivo está incompleto, e não é contabilizado.**

**A aplicação foi desenvolvida para sistemas LINUX. Não funcionará em outros sistemas operacionais.**


# Instruções #

**IMPORTANTE: Lembre-se de adicionar o arquivo de logs. Crie uma pasta nomeada "log" no caminho da main.c, e coloque o arquivo com nome "access.log" dentro dela.**
Como a aplicação está dividida em mais de um arquivo, é necessário compilar todos juntos. Siga os comandos:

    gcc -g -pthread main.c src/FilaTarefa.c src/Log.c src/OutputData.c src/Tarefa.c -o main

E para execurar:

    ./main    


# Considerações #

A primeira versão com sscanf não era acertiva na contagem dos acessos, sendo necessário a refatoração da função que processa o arquivo.

Para a solução, passei a utilizar a biblioteca `regex.h`. O grande desafio foi encontrar uma expressão regular funcional, que atendesse a todos os registros. Algumas linhas tem tamanho superior a 8k de caracteres, sendo necessário utilizar linhas de 32k para processar.
A expressão que atendeu ao objetivo foi:

    ^[^ ]+ - [^ ]+ \\[([0-9]{2})/[^/]+/[0-9]{4}:([0-9]{2}):[0-9]{2}:[0-9]{2} [^]]+\\] \".*?\" ([0-9]{3})

Extraí somente os 3 grupos necessários:

`([0-9]{2})`: salva os 2 digitos do dia;

`([0-9]{2})`: salva os 2 dígitos da hora;

`([0-9]{3})`: salva os 3 dígitos do status.

# Resultado Final #

## Dia 23/Jan/2019 #

| Hora | Acessos Totais | Acessos Success |
|:----:|:--------------:|:---------------:|
| 0 | 0 | 0 | 
| 1 | 0 | 0 |
| 2 | 0 | 0 |
| 3 | 0 | 0 | 
| 4 | 0 | 0 |
| 5 | 0 | 0 |
| 6 | 0 | 0 | 
| 7 | 0 | 0 |
| 8 | 0 | 0 |
| 9 | 0 | 0 | 
| 10 | 0 | 0 |
| 11 | 0 | 0 |
| 12 | 81768 | 76852 |
| 13 | 154560 | 143647 |
| 14 | 151429 | 139325 |
| 15 | 138612 | 127257 |
| 16 | 125123 | 113827 |
| 17 | 127902 | 116598 |
| 18 | 111533 | 102199 |
| 19 | 103714 | 95757 |
| 20 | 109736 | 101269 |
| 21 | 109053 | 100354 |
| 22 | 117247 | 106478 |
| 23 | 107478 | 99683 |

## Dia 24/Jan/2019 #

| Hora | Acessos Totais | Acessos Success |
|:----:|:--------------:|:---------------:|
| 0 | 76877 | 68010 |
| 1 | 52966 | 45737 |
| 2 | 30582 | 25427 |
| 3 | 18575 | 15554 |
| 4 | 13587 | 11832 |
| 5 | 11865 | 10416 |
| 6 | 14587 | 12778 |
| 7 | 30460 | 27838 |
| 8 | 71606 | 67181 |
| 9 | 94659 | 87564 |
| 10 | 119912 | 111701 |
| 11 | 136331 | 127127 |
| 12 | 124874 | 116762 |
| 13 | 117220 | 106999 |
| 14 | 113949 | 106500 |
| 15 | 104640 | 96551 |
| 16 | 83741 | 75973 |
| 17 | 86333 | 78350 |
| 18 | 93861 | 87469 |
| 19 | 82589 | 72938 |
| 20 | 83098 | 74724 |
| 21 | 88700 | 81553 |
| 22 | 108831 | 100895 |
| 23 | 98014 | 90706 |

## Dia 25/Jan/2019 #

| Hora | Acessos Totais | Acessos Success |
|:----:|:--------------:|:---------------:|
| 0 | 96468 | 87137 |
| 1 | 62715 | 55558 |
| 2 | 37703 | 33615 |
| 3 | 21799 | 19865 |
| 4 | 14993 | 13982 |
| 5 | 12913 | 12133 |
| 6 | 15593 | 14669 |
| 7 | 25976 | 24207 |
| 8 | 40897 | 36571 |
| 9 | 60566 | 55593 |
| 10 | 81948 | 75811 |
| 11 | 91797 | 84604 |
| 12 | 102056 | 93678 |
| 13 | 104636 | 97165 |
| 14 | 101468 | 92403 |
| 15 | 101378 | 93360 |
| 16 | 94829 | 87214 |
| 17 | 96582 | 88411 |
| 18 | 100643 | 91203 |
| 19 | 115621 | 104932 |
| 20 | 116375 | 107164 |
| 21 | 115503 | 106332 |
| 22 | 91528 | 87107 |
| 23 | 0 | 0 |

## Total ##

| Acessos Totais | Acessos Success |
|:--------------:|:---------------:|
| 4999999 | 4586545 |
