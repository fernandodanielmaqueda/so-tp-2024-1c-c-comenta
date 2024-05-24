#include "scheduler.h"

	sem_t sem_detener_execute;
	sem_t sem_detener_new_ready;
	sem_t sem_detener_block_ready;
	sem_t sem_detener_block;
	sem_t sem_detener_planificacion;

void stop_planificacion()
	{
		sem_wait(&sem_detener_planificacion);
		sem_wait(&sem_detener_new_ready);
		sem_wait(&sem_detener_block_ready);
		sem_wait(&sem_detener_block);
		sem_wait(&sem_detener_execute);
		sem_post(&sem_detener_planificacion);
}



void init_planificacion()
	{
		sem_wait(&sem_detener_planificacion);
		sem_post(&sem_detener_new_ready);
		sem_post(&sem_detener_block_ready);
		sem_post(&sem_detener_block);
		sem_post(&sem_detener_execute);
		sem_post(&sem_detener_planificacion);
	} 

    
void free_strv(char** array)
{
    if (array != NULL) {
        for (int i = 0; array[i] != NULL; i++) {
            free(array[i]);
        }
        free(array);
    }
}