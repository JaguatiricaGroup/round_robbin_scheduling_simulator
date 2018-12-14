/*
*  rrprio.c - Implementacao do algoritmo Round Robin com Prioridades
*
*  Autores: SUPER_PROGRAMADORES_C
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*
*/

#include "rrprio.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char rrprioName[]="RRPr";

//=====Funcoes Auxiliares=====
// estrutura de dados
//int rrpReleaseParams(Process *p);


typedef struct rrpLista RRPLista;



struct rrpLista { // lista encadeada sem mexer na lista de processos
    RRPLista* next; 
    Process* processo;
};

typedef struct { // estrutura para otimizar e encapsular todo o roundRobin
    RRPLista* colPrio[8];
    RRPLista* colPrioFim[8];  
    Process* processIdle;
}RRB;

RRB* roundRobin; // estrutura GLobal de escalonamento

void printNossaEstrutura(){
    for( int prioridade = 7; prioridade>=0; prioridade--){
//        if(roundRobin->colPrio[prioridade] != NULL){
        printf("prioridade: %d -- ", prioridade);
        RRPLista* aux = roundRobin->colPrio[prioridade];
        Process* processoAux;
        while (aux!=NULL) { // tenta pegar o primeiro
            processoAux=aux->processo;
            printf("p(%d, %d) ,",processGetPid(processoAux), processGetStatus(processoAux));
            fflush(stdin);
            aux = aux->next;
        } // se nao for o primeiro tenta os proximos            
        printf("\n");
    }
}


void insereNoEscalonador(Process* p, int prioridade){
    //prepara novo slot na lista
    RRPLista* novaPosLista = malloc(sizeof(RRPLista));
    novaPosLista->processo = p;
    novaPosLista->next = NULL;
    
    // se é o primeiro elemento da lista marca como fim da lista
    if(roundRobin->colPrio[prioridade] == NULL){ 
//        roundRobin->colPrioFim[prioridade] = novaPosLista;
        roundRobin->colPrio[prioridade] = novaPosLista;
        roundRobin->colPrioFim[prioridade] =novaPosLista;
    }else{
    // coloca novo elemento no Fim da lista        
        roundRobin->colPrioFim[prioridade]->next = novaPosLista;
        roundRobin->colPrioFim[prioridade] = novaPosLista;
    }
    // lembra de deletar no release ;) 
    printNossaEstrutura();
}

void mudaPrioridadeRRPLista(RRPLista* noComPrioReduzida, RRPLista* noAnteriorAoQueTemPrioReduzida, int prioridadeAntiga){
    int prioAtual = ((RRPrioParams*)processGetSchedParams(noComPrioReduzida->processo))->prio;
    printf("eu sou o %d, minha prioridade antiga era %d, e agr e %d", processGetPid(noComPrioReduzida->processo), prioridadeAntiga, prioAtual);
    if( noComPrioReduzida == roundRobin->colPrio[prioridadeAntiga] ){//era a cabeca da lista
        if(noComPrioReduzida->next == NULL){ // nao tem irmao
            //removeu da posicao antiga
            printf("vou ser removido");
            fflush(stdout);
            roundRobin->colPrio[prioridadeAntiga] = NULL; // nao tem  irmao
            roundRobin->colPrioFim[prioridadeAntiga] = NULL; // nao tem  irmao
            //inserir na nova posicao
            if (roundRobin->colPrio[prioAtual] == NULL){ // ta vazio o lugar novo
                roundRobin->colPrio[prioAtual] = noComPrioReduzida; 
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;
                noComPrioReduzida->next = NULL;
            }else{// tem irmao no lugar que ele vai ficar                
                roundRobin->colPrioFim[prioAtual]->next = noComPrioReduzida; // coloca o proximo do fim nele
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;// poe como fim
                noComPrioReduzida->next = NULL;
            }         
        }else{//quando nocomPrioridadeReduzida tem irmaos
            //removi do lugar antigo
            roundRobin->colPrio[prioridadeAntiga] = noComPrioReduzida->next;
            //insere na posicao nova
            if(roundRobin->colPrio[prioAtual] == NULL){
                roundRobin->colPrio[prioAtual] = noComPrioReduzida;
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;                
                noComPrioReduzida->next = NULL;//para esquecer os irmaos
            }else{// ja tinha gente
                roundRobin->colPrioFim[prioAtual]->next = noComPrioReduzida; // coloca o proximo do fim nele
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;// poe como fim
                noComPrioReduzida->next = NULL; // para esquecer os irmaos
            }
            
        }
    }else{// esta no meio da lista de prioridades dele ou no fim
        if(noComPrioReduzida->next == NULL){ //ele e o ultimo da lista antiga dele, n tem irmao
            //remove
            noAnteriorAoQueTemPrioReduzida->next = NULL;// esquece irmaos
            roundRobin->colPrioFim[prioridadeAntiga] = noAnteriorAoQueTemPrioReduzida;// set pro fim
            //inserir
            if(roundRobin->colPrio[prioAtual] == NULL){// casa para a q vai esta vazia                
                roundRobin->colPrio[prioAtual] = noComPrioReduzida;
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;                
                noComPrioReduzida->next = NULL;
            }else{// tem genta na casa
                roundRobin->colPrioFim[prioAtual]->next = noComPrioReduzida;
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;
                noComPrioReduzida->next = NULL;
            }
        }else{// ele está no meio da lista dele
            //remove
            noAnteriorAoQueTemPrioReduzida->next = noComPrioReduzida->next;
            //insere
            if(roundRobin->colPrio[prioAtual] == NULL){// casa para a q vai, esta vazia
                roundRobin->colPrio[prioAtual] = noComPrioReduzida;
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;
                noComPrioReduzida->next = NULL;
            }else{// ja tinha gente na nova casa                
                roundRobin->colPrioFim[prioAtual]->next = noComPrioReduzida;
                roundRobin->colPrioFim[prioAtual] = noComPrioReduzida;
                noComPrioReduzida->next = NULL;// esquece os irmaos
            }
            
        }
    }
}
//
//void insereQdoDiminuiPrioridadeRRPLista(RRPLista* noComPrioReduzida, RRPLista* noAnteriorAoQueTemPrioDiminuida, int prioridadeAntiga){ //chamar quando diminui prioridade
//    
//    int prioridadeReduzida=(((RRPrioParams*)processGetSchedParams(noComPrioReduzida->processo))->prio);
//    printf("prioridade do %d é %d e antes era %d",((RRPrioParams*)processGetPid(noComPrioReduzida->processo)), prioridadeReduzida, prioridadeAntiga );
//    fflush(stdout);
//    RRPLista* inicioLista = roundRobin->colPrio[prioridadeReduzida];
//    
//    if(inicioLista == NULL){//se na casinha q ele vai ocupar nao tinha nd        
//        roundRobin->colPrio[prioridadeReduzida] = noComPrioReduzida;
//        if(noComPrioReduzida == NULL){
//            exit(11);
//        }
////        if ( noComPrioReduzida != roundRobin->colPrio[prioridadeAntiga])
//        roundRobin->colPrio[prioridadeAntiga] = noComPrioReduzida->next;
//        roundRobin->colPrioFim[prioridadeAntiga] = noAnteriorAoQueTemPrioDiminuida;
//        
//    }else{//se na casinha que ele vai ocupar ja tem pelo menos um
////        printf("estou no insere\n");
////        fflush(stdout);
//        
//        roundRobin->colPrioFim[prioridadeReduzida]->next=noComPrioReduzida;// next do ultimo vira noAtiaç
//        if (prioridadeAntiga == 0){// lista circular simplesss
//            if(noComPrioReduzida == noAnteriorAoQueTemPrioDiminuida){// caso seja a cabeca
//                roundRobin->colPrio[prioridadeAntiga] = roundRobin->colPrio[prioridadeReduzida]->next;
//            }else{// nap era a cabeca
//                noAnteriorAoQueTemPrioDiminuida->next = noComPrioReduzida->next;
//            }
//            
//        }        
//        noComPrioReduzida->next = NULL;
//    }
//    roundRobin->colPrioFim[prioridadeReduzida]=noComPrioReduzida;// no atual vira ultimo
////    noComPrioReduzida->next = NULL;
//    printf("estou no insere e vou imprimir a esttrutura para me debugar\n");
//        fflush(stdout);
//        
//    printNossaEstrutura();
//
//}
//
//void removeDaRRPListaQdoDiminuiAPrioridade(RRPLista* noParaSerRemovido, RRPLista* noAnteriorAoQueTemPrioDiminuida, int prioridadeAntiga){
//    //trata os casos de remoção de uma lista encadeada
////    if(noAnteriorAoQueTemPrioDiminuida!=NULL && noParaSerRemovido->next==NULL)    
//    int prio = (((RRPrioParams*)processGetSchedParams(noAnteriorAoQueTemPrioDiminuida->processo))->prio);
//    if(prioridadeAntiga != 0){
//        if(noAnteriorAoQueTemPrioDiminuida == noParaSerRemovido ){// é o primeiro da lista
//            
//            if(  noParaSerRemovido->next == NULL ){// quando so tem um na lista
////                printf(" no removeeee");
////                fflush(stdout);
//        //       roundRobin->colPrio[prio] = noAnteriorAoQueTemPrioDiminuida->next;
//               roundRobin->colPrio[prioridadeAntiga] = NULL;
//               roundRobin->colPrioFim[prioridadeAntiga] = NULL;       
//            }else{
//                roundRobin->colPrio[prioridadeAntiga] = noParaSerRemovido->next;
//    //            roundRobin->colPrioFim[prioridadeAntiga] = NULL;       
//            }
//        }
//        
//        if(noAnteriorAoQueTemPrioDiminuida!=NULL && noParaSerRemovido->next==NULL){// e o ultimo da lista
//            noAnteriorAoQueTemPrioDiminuida->next = NULL;
//            roundRobin->colPrioFim[prio] = noAnteriorAoQueTemPrioDiminuida;
//        }
//
//        if(noAnteriorAoQueTemPrioDiminuida!=NULL && noParaSerRemovido->next!=NULL){// ta no meio
//            noAnteriorAoQueTemPrioDiminuida->next=noParaSerRemovido->next;
//        }
//    }
//}

int diminuiPrio(Process* p){
    printf("diminuindo a prioridade\n");
    int prio = (((RRPrioParams*)processGetSchedParams(p))->prio);
    if( prio != 0){
        (((RRPrioParams*)processGetSchedParams(p))->prio)-=1;
    }
    return prio;
}

Process* retornaProximoProcessoListaComRBB(){
    // retira o processo com maior prioridade do inicio que esteja ready
    printf("estou vendo todas prioridades\n");
        fflush(stdout);
    for( int prioridade = 7; prioridade>=0 ; prioridade--){
        
        printf("prioridade %d",prioridade );
        fflush(stdout);

//        if(roundRobin->colPrio[prioridade] != NULL){
        RRPLista* atual = roundRobin->colPrio[prioridade];
        if (atual != NULL) {
            RRPLista* antesDoEscolhido = roundRobin->colPrio[prioridade];
            Process* processoAux;
            int prioridadeAntiga;                
            if ( processGetStatus(atual->processo) == PROC_READY ){
                prioridadeAntiga = diminuiPrio(atual->processo);
                processoAux = atual->processo;
//                insereQdoDiminuiPrioridadeRRPLista(atual,antesDoEscolhido, prioridadeAntiga);// insere no fim da prioridade nova
//                removeDaRRPListaQdoDiminuiAPrioridade(atual, antesDoEscolhido, prioridadeAntiga);// remove o de prioridade antiga
                mudaPrioridadeRRPLista(atual, antesDoEscolhido, prioridadeAntiga);
                return atual->processo;
            }          
            atual = atual->next;        
            while (atual != NULL ) {            
                if(processGetStatus(atual->processo) == PROC_READY){                
                    prioridadeAntiga = diminuiPrio(atual->processo);
//                    insereQdoDiminuiPrioridadeRRPLista(atual,antesDoEscolhido, prioridadeAntiga);
//                    removeDaRRPListaQdoDiminuiAPrioridade(atual, antesDoEscolhido, prioridadeAntiga);
                    mudaPrioridadeRRPLista(atual, antesDoEscolhido, prioridadeAntiga);
                    return atual->processo;
                }            
                antesDoEscolhido = atual;
                atual = atual->next;         
            }
        }
    }
        printf(" Eu não achei Ninguemmm :')))) to bugado \n");
        fflush(stdout);
        return NULL;
}

int teste(Process* lista){
    printf("funcionei");
    fflush(stdout);
    return 200000;
    
}

//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Round Robin com Prioridades
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void rrpInitSchedInfo() {
    //cria e registra o escalonador
    SchedInfo* escalonadorAux = malloc(sizeof(SchedInfo));
    for (int i = 0; i<MAX_NAME_LEN+1; i++){
        escalonadorAux->name[i] = rrprioName[i];        
    }
    escalonadorAux->initParamsFn = rrpInitSchedParams;    
    escalonadorAux->scheduleFn = rrpSchedule;
    escalonadorAux->releaseParamsFn = rrpReleaseParams;
//    printf("ponteiro esc %u ponteiro func %u", escalonadorAux->releaseParamsFn, rrpReleaseParams);
//    Process* nada = NULL;    
    printf("vou chorar");
    fflush(stdout);
    
//    escalonadorAux->scheduleFn(NULL);
//    printf("%d", escalonadorAux->releaseParamsFn(NULL));
    
    printf("chora ");
    fflush(stdout);
    schedRegisterScheduler(escalonadorAux);
    
    // inicia estrutura global do escalonador
    roundRobin = malloc(sizeof(RRB));
//    roundRobin->colPrio = malloc(sizeof(RRPLista*)*8);
//    roundRobin->colPrioFim = malloc(sizeof(RRPLista*)*8);
    for(int i =0; i< 8; i++){
        roundRobin->colPrio[i] = NULL;
        roundRobin->colPrioFim[i] = NULL;
    }
    
    printf("%s criado e colocado no Sched...\n", escalonadorAux->name);
}

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de RRPrio
void rrpInitSchedParams(Process *p, void *params) {
    printf("Vou inserir um processo\n" );
    
//    if(processGetPid(p)==1){// guarda o primeiro para retornar quando n tem ngm
//        RRB->processIdle = p;
//    }else{
    
        // ao ser construido um novo processo vou colocar na estrutura
        processSetSchedParams(p, params);
        int prio = ((RRPrioParams*)params)->prio;
        insereNoEscalonador(p, prio);
        printf("\nInseridoo %d \n", processGetPid(p) );
        fflush(stdout);
//    }
        schedSetScheduler(p, params, 0);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo RRPrio
Process* rrpSchedule(Process *plist) {        
    printf("\nvou escalonar e imprimir a nossa estrutura \n");
    printNossaEstrutura();
    fflush(stdout);
    Process* processoEscolhido = retornaProximoProcessoListaComRBB();
    printf("escolhi o processo %d", processGetPid(processoEscolhido));
    printf("ja esalonei olha como ficou\n");
    printNossaEstrutura();
    fflush(stdout);
    return processoEscolhido;
}

//Libera os parametros de escalonamento de um processo p, chamada 
//normalmente quando o processo e' desassociado do slot de RRPrio
int rrpReleaseParams(Process *p) {
//    if( ((int)p) == 0)
//        return 4000;
    printf("\ncomecei %d\n");
    printf("\n=======================================atualz PID %d\n", processGetPid(p));
    fflush(stdout);
    RRPLista* atual= roundRobin->colPrio[((RRPrioParams*)processGetSchedParams(p))->prio];
    
    fflush(stdout);
    RRPLista* anterior = atual;
//    RRPLista* fimLista = lista;
    // nao tem como a lista ser nula
    if (atual==NULL) return 0;
    if(processGetPid(atual->processo) == processGetPid(p)){//processo que queriamos estava na cabeca
//        lista = NULL;
        if(atual->next == NULL){
            roundRobin->colPrio[((RRPrioParams*)processGetSchedParams(p))->prio] = NULL;
            roundRobin->colPrioFim[((RRPrioParams*)processGetSchedParams(p))->prio] = NULL;
        }else{
            roundRobin->colPrio[((RRPrioParams*)processGetSchedParams(p))->prio] = atual->next;
        }
        
        atual->next = NULL;
        atual->processo = NULL;
        free(atual);
        return 1;
    }else{ // p nao era primeiro elem
        anterior = atual;
        atual = atual->next;
        while( atual!=NULL ) {// percorre
            if( atual->processo == p){ // achou
                anterior->next = atual->next;                
                if ( atual == roundRobin->colPrioFim[((RRPrioParams*)processGetSchedParams(p))->prio] ){ // se for o ultimo elemento
                    roundRobin->colPrioFim[((RRPrioParams*)processGetSchedParams(p))->prio] = anterior;
                }                    
                atual->next = NULL;
                atual->processo = NULL;
                free(atual);
                return 1;
            }
        anterior = atual;
        atual = atual->next;
            
        }
    }
            
    return 0;
}

//Modifica a prioridade de um processo para um novo valor
//Retorna o valor da prioridade anteriormente atribuida ao processo
int rrpSetPrio(Process *p, int prio) {
    int prioAntiga = ((RRPrioParams*)(processGetSchedParams(p)))->prio;
    ((RRPrioParams*)(processGetSchedParams(p)))->prio = prio;
    
    RRPLista* noDoProcesso = roundRobin->colPrio[prioAntiga];
    RRPLista* noAnteriorAoDoProcesso = roundRobin->colPrio[prioAntiga];
    
    if( noDoProcesso != NULL){
        if(noDoProcesso->processo == p){
            mudaPrioridadeRRPLista(noDoProcesso, noAnteriorAoDoProcesso, prioAntiga);
            return prioAntiga;
        }
//            //insere
//            if (roundRobin->colPrio[prio] == NULL){
//                roundRobin->colPrio[prio] = noDoProcesso;
////                roundRobin->colPrio[prioAntiga] = noDoProcesso->next;
////                roundRobin->colPrioFim[prio] = noDoProcesso;
////                noDoProcesso->next = NULL;
//            }else{
//                roundRobin->colPrioFim[prio]->next = noDoProcesso;
////                roundRobin->colPrioFim[prio] = noDoProcesso;
////                noDoProcesso->next = NULL;
//            }
//            roundRobin->colPrioFim[prio] = noDoProcesso;// arrumou o q aponta pro fim        
//            roundRobin->colPrio[prioAntiga] = noDoProcesso->next;// garantiu os irmaos
//            noDoProcesso->next = NULL;// ele tirou os irmaos antigos
//            //remove            
//            if(roundRobin->colPrioFim[prioAntiga] == noDoProcesso){
//                roundRobin->colPrioFim[prioAntiga] = NULL;
//            }
//        }
        noDoProcesso = noDoProcesso->next;
    }
    
    while (noDoProcesso != NULL) {
        if( noDoProcesso->processo == p ){
            mudaPrioridadeRRPLista(noDoProcesso, noAnteriorAoDoProcesso, prioAntiga);
            return prioAntiga;
            //insere
//            if (roundRobin->colPrio[prio] == NULL){// ta vazio onde quero por
//                roundRobin->colPrio[prio] = noDoProcesso;
//                roundRobin->colPrioFim[prio] = noDoProcesso;
//                noDoProcesso->next = NULL;
//            }else{
//                roundRobin->colPrioFim[prio]->next = noDoProcesso;
//                roundRobin->colPrioFim[prio] = noDoProcesso;
//                noDoProcesso->next = NULL;
//            }
//            //remove
//            
//            noAnteriorAoDoProcesso->next = noDoProcesso->next;
//            if(noDoProcesso->next == NULL){
//                roundRobin->colPrioFim[prioAntiga] = noAnteriorAoDoProcesso;
//            }            
        }
        noAnteriorAoDoProcesso = noDoProcesso;
        noDoProcesso = noDoProcesso->next;        
    }
//    return prioAntiga;
}
