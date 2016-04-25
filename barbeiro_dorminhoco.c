#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*
    Definição do problema:
    - Se não há clientes, o barbeiro adormece;
    - Se a cadeira do barbeiro estiver livre, um cliente pode ser atendido imediatamente;
    - O cliente espera pelo barbeiro se houver uma cadeira de espera vazia;
    - Se não tiver onde sentar, o cliente vai embora.
*/

#define N_CLIENTES (10)
#define N_CADEIRAS (5)

sem_t semf_cadeiras; //semaforo para as cadeiras de espera
sem_t semf_cadeira_barbeiro; //semaforo para a cadeira do barbeiro
sem_t semf_cliente; //semaforo para se existe ou nao cliente
sem_t semf_barbeiro; //semaforo para o barbeiro (dormindo ou ocupado);

//Procedimento que sera executado pela thread 'barbeiro'
void *barbeiro(void *arg)
{
    while (1)
    {
        sem_wait(&semf_cliente);
        sleep(2); //2 segundos para fazer o servico
        printf("O barbeiro cortou o cabelo do cliente.\n");
        sem_post(&semf_barbeiro); //Libera o barbeiro.
    }
}

// ....... sera executado pela thread 'clientes[i]'
void *cliente(void *arg)
{
    int id = *(int*)arg;
    sleep(id % 3);

    if(sem_trywait(&semf_cadeiras) == 0)
    {
        printf("O cliente %d entrou na barbearia.\n", id);
        sem_wait(&semf_cadeira_barbeiro);
        sem_post(&semf_cliente); //Acorda o barbeiro
        sem_post(&semf_cadeiras); //libera uma cadeira de espera
        printf("O cliente %d esta cortando o cabelo...\n", id);
        sem_wait(&semf_barbeiro); //Espera ate que o barbeiro esteja disponivel
        sem_post(&semf_cadeira_barbeiro); //Libera a cadeira o barbeiro
        printf("Cliente %d saiu satisfeito.\n\n", id);
    }
    else
    {
        printf("O cliente %d nao entrou na barbearia porque estava lotada.\n", id);
    }

    pthread_exit(NULL);
}

int main()
{
    sem_init(&semf_cadeiras, 0, N_CADEIRAS);
    sem_init(&semf_cadeira_barbeiro, 0, 1);
    sem_init(&semf_cliente, 0, 0);
    sem_init(&semf_barbeiro, 0, 0);

    pthread_t tbarbeiro;
    pthread_t tclientes[N_CLIENTES];

    int i;
    int ids[N_CLIENTES];

    //Cria as threads dos clientes
    for(i = 0; i < N_CLIENTES; i++)
    {
        ids[i] = i + 1;
        pthread_create(&tclientes[i], NULL, cliente, (void*)&ids[i]);
    }

    //cria a thread do barbeiro
    pthread_create(&tbarbeiro, NULL, barbeiro, NULL);

    //Coloca os clientes em concorrencia.
    for(i = 0; i < N_CLIENTES; i++)
    {
        pthread_join(tclientes[i], NULL);
    }

    return 0;
}
