/* Arroyo Calle, Adrian - 71178253S */
/* Martinez Alonso, Eloy Antonio - 71173798E */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

/* BUFFERS */

char** buffer1;
char** buffer2;

/* SEMAFOROS */

sem_t sem_hayhuecob1;
sem_t sem_haydatob1;
sem_t sem_hayhuecob2;
sem_t sem_haydatob2;
sem_t mutex_procesados;
sem_t mutex_impresos;

int procesados;
int impresos;

/* Genera un numero aleatorio entre 0 y max */

int random_n(int max){
	int r = rand() % (max+1);
	return r;
}

/* Comprueba si un numero es palindromo (1) o no (0) */

int palindromo (char *palabra) {
	int esPalindromo = 1;
	int i,j;

	j = strlen (palabra)-1;
	for (i=0; i<strlen(palabra)/2 && esPalindromo; i++, j--){
		if (*(palabra+i)!=*(palabra+j)){
			esPalindromo = 0;
		}
	}
	return esPalindromo;
}

/* Hilo productor inicial */

void* productor(void* z)
{
	int* x = (int*) z;
	int tamBuffer = (int) *x;
	for(int i=0;i<75;i++){
		int n = random_n(999);
		sem_wait(&sem_hayhuecob1);
		sprintf(buffer1[i%tamBuffer],"%d",n);
		sem_post(&sem_haydatob1);
	}
	pthread_exit(NULL);
}

/* Hilo(s) consumidores intermedios */

void* consumidor(void* z)
{
	int* x = (int*) z;
	int tamBuffer = *x;
	while(1){
		/* Leer dato del buffer 1 y procesar */
		sem_wait(&mutex_procesados);
		if(procesados >= 75){
			sem_post(&mutex_procesados);
			pthread_exit(NULL);
		}
		sem_wait(&sem_haydatob1);
		int i = procesados;
		procesados++;
		sem_post(&mutex_procesados);

		char tmp[120];
		if(palindromo(buffer1[i%tamBuffer]) == 1){
			sprintf(tmp,"El numero %s es palindromo\n",buffer1[i%tamBuffer]);
		}else{
			sprintf(tmp,"El numero %s NO es palindromo\n",buffer1[i%tamBuffer]);
		}
		sem_post(&sem_hayhuecob1);

		/* Escribir en buffer 2 */
		sem_wait(&mutex_impresos);
		sem_wait(&sem_hayhuecob2);
		strcpy(buffer2[impresos%5],tmp);
		impresos++;
		sem_post(&sem_haydatob2);
		sem_post(&mutex_impresos);

	}
	pthread_exit(NULL);
}

/* Hilo consumidor final. z es un puntero al archivo donde se guardan los datos */

void* consumidor_final(void* z)
{	
	FILE* file = (FILE*)z;
	for (int i=0; i<75; i++){
		sem_wait(&sem_haydatob2);
		fprintf(file,"%s",buffer2[i%5]);
		sem_post(&sem_hayhuecob2);
	}
	fclose(file);
	
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	/* Iniciar variables */
	procesados = 0;
	impresos = 0;
	srand(time(NULL));

	/* Comprobacion de los argumentos */

	if(argc != 4){
		fprintf(stderr,"Numero de argumentos incorrectos\n");
		return -1;
	}
	int tamBuffer = atoi(argv[1]);
	if(tamBuffer <= 0){
		fprintf(stderr,"Especifica un numero de tamano de buffer correcto\n");
		return -2;
	}
	char* ficheroSalida = argv[2];

	int num_P = atoi(argv[3]);
	if(num_P <= 0){
		fprintf(stderr,"Numero de hilos incorrecto\n");
		return -3;
	}
	FILE* file = fopen(ficheroSalida,"w");
	if(file == NULL){
		fprintf(stderr,"Imposible abrir fichero en modo escritura\n");
		return -4;
	}

	/* reservar memoria para los bufferes */

	buffer1 = (char**)malloc(tamBuffer*sizeof(char*));
	if(buffer1 == NULL){
		fprintf(stderr,"Error en la reserva de memoria\n");
		return -5;
	}
	for(int i=0;i<tamBuffer;i++){
		buffer1[i] = (char*) malloc(4*sizeof(char));
		if(buffer1[i] == NULL){
			fprintf(stderr,"Error en la reserva de memoria\n");
			return -5;
		}
	}
	buffer2 = (char**)malloc(5*sizeof(char*));
	if(buffer2 == NULL){
		fprintf(stderr,"Error en la reserva de memoria\n");
		return -5;
	}
	for (int i=0;i<5;i++){
		buffer2[i] = (char*) malloc(120*sizeof(char*));
		if(buffer2[i] == NULL){
			fprintf(stderr,"Error en la reserva de memoria\n");
			return -5;
		}
	}
	pthread_t prod;
	pthread_t* consum;
	pthread_t consum_final;

	/* Inicializar semaforos */
	sem_init(&sem_hayhuecob1,0,tamBuffer);
	sem_init(&sem_haydatob1,0,0);
	sem_init(&sem_haydatob2,0,0);
	sem_init(&sem_hayhuecob2,0,5);
	sem_init(&mutex_procesados,0,1);
	sem_init(&mutex_impresos,0,1);

	consum = (pthread_t*)malloc(num_P*sizeof(pthread_t));
	if(consum == NULL){
		fprintf(stderr,"Error en la reserva de memoria\n");
		return -5;
	}

	/* Lanzar hilos */

	pthread_create(&prod,NULL,productor,(void*)&tamBuffer);
	for(int i=0;i<num_P;i++){
		pthread_create(&consum[i],NULL,consumidor,(void*)&tamBuffer);
	}
	pthread_create(&consum_final,NULL,consumidor_final,(void*)file);

	pthread_join(consum_final,NULL);
	for(int i=0;i<num_P;i++){
		pthread_join(consum[i],NULL);
	}
	pthread_join(prod,NULL);	

	/* Liberar recursos */

	free(consum);

	for(int i=0;i<tamBuffer;i++){
		free(buffer1[i]);
	}
	free(buffer1);

	for(int i=0;i<5;i++){
		free(buffer2[i]);
	}
	free(buffer2);

	return 0;
}
