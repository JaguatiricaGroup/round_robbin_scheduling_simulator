
/*
*  rrprio.c - Implementacao do algoritmo Round Robin com Prioridades
*
*  Autores: Leticia Florentino Pires, Gabriel Dias de Abreu with Marcelo Moreno Orientation
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*/

#ifndef RRPRIO_H
#define RRPRIO_H

#include "sched.h"

#define NUMERO_PRIORIDADES 8

typedef struct rrprio_params {
    int prio; // prioridade    
    //Muito estranha essa estrutura
   
    // um vetor onde cada posicao é uma prioridade com uma lista 
    // de ponteiros para processos
//    Process* colunaPrioridades[NUMERO_PRIORIDADES];
    
    
//    int pidUltimoProcessoInserido;
    //ponteira para os primeiros elementos da lista, garante inserçao O(1)
//    Process* PonteiroFimListaRRP[NUMERO_PRIORIDADES];  
    
} RRPrioParams;

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Round Robin com Prioridades
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void rrpInitSchedInfo(void);

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de RRPrio
void rrpInitSchedParams(Process *p, void *params);

//Retorna o proximo processo a obter a CPU, conforme o algortimo RRPrio
Process* rrpSchedule(Process *plist);

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de RRPrio
int rrpReleaseParams(Process *p);

//Modifica a prioridade de um processo para um novo valor
//Retorna o valor da prioridade anteriormente atribuida ao processo
int rrpSetPrio(Process *p, int prio);

#endif
