#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CLIENTES (10)
#define CADEIRAS (5)

sem_t sem_cadeiras; //semaforo para cadeira ocupada
sem_t sem_barbeiro; //ocupado ou nao
sem_t sem_cadeira_barb; //semaforo para a cadeira do barbeiro
sem_t sem_clientes; //Existe clientes ou nao

void* barbeiroFunction(void *arg);
void* clienteFunction(void *arg);

int main()
{
  sem_init(&sem_cadeiras, 0, CADEIRAS);
  sem_init(&sem_barbeiro, 0, 0);
  sem_init(&sem_cadeira_barb, 0, 1);
  sem_init(&sem_clientes, 0, 0);

  pthread_t clientes[CLIENTES];
  pthread_t barbeiro;

  int i, ids[CLIENTES];
  for(i = 0; i < CLIENTES; i++)
  {
    ids[i] = i + 1;
    pthread_create(&clientes[i], NULL, clienteFunction, (void *)&ids[i]);
  }

  pthread_create(&barbeiro, NULL, barbeiroFunction, NULL);

  //Inicializacao das threads;
  for(i = 0; i < CLIENTES; i++)
  {
    pthread_join(clientes[i], NULL);
  }

  return 0;
}

void* barbeiroFunction(void *arg)
{
  while(1)
  {
    sem_wait(&sem_clientes); //o barbeiro dorme ou acorda.
    printf("O barbeiro cortou o cabelo de um cliente.\n");
    sem_post(&sem_barbeiro);
  }

  return NULL;
}

void* clienteFunction(void *arg)
{
  int id = *(int*)arg;
  sleep(id % 3);

  //sem_trywait
  if(sem_trywait(&sem_cadeiras) == 0) //entrou na barbearia
  {
    printf("Cliente %d entrou na barbearia.\n", id);
    
    sem_wait(&sem_cadeira_barb); //espera a cadeira do barbeiro ser liberada.
      printf("Cliente %d esta cortando o cabelo.\n", id);
      sem_post(&sem_clientes); //cliente sai da cadeira de espera.
      sem_post(&sem_cadeiras); //libera uma cadeira de espera.
      sem_wait(&sem_barbeiro); //cliente esta cortando o cabelo
    sem_post(&sem_cadeira_barb); // libera a cadeira do barbeiro.
    
    printf("O cliente %d deixou a barbearia.\n\n", id);
  }
  else //nao entrou na barbearia
  {
    printf("Cliente %d nao entrou na barbearia...\n", id);
    return NULL;
  }

  pthread_exit(NULL);
}
