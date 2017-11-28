#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>



char** buffer1;

sem_t sem_prod;
sem_t sem_con;

int random_n(int max){
	int r = rand() % (max+1);
	return r;
}

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

void* productor(void* z)
{
	int* x = (int*) z;
	int tamBuffer = (int) *x;
	for(int i=0;i<75;i++){
		int n = random_n(999);
		sem_wait(&sem_prod);
		sprintf(buffer1[i%tamBuffer],"%d",n);
		sem_post(&sem_con);
	}
	pthread_exit(NULL);
}

void* consumidor(void* z)
{
	int* x = (int*) z;
	int tamBuffer = *x;
	for(int i=0;i<75;i++){
		sem_wait(&sem_con);
		if(palindromo(buffer1[i%tamBuffer]) == 1){
			printf("%s es palindromo\n",buffer1[i%tamBuffer]);
		}
		printf("%s\n",buffer1[i%tamBuffer]);
		sem_post(&sem_prod);
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	printf("PRACTICA FSO\n");
	if(argc != 3){
		fprintf(stderr,"Numero de argumentos incorrectos\n");
		return -1;
	}
	int tamBuffer = atoi(argv[1]);
	if(tamBuffer == 0){
		fprintf(stderr,"Especifica un numero de tamano de buffer correcto\n");
		return -2;
	}
	char* ficheroSalida = argv[2];
	buffer1 = (char**)malloc(tamBuffer*sizeof(char*));
	for(int i=0;i<tamBuffer;i++){
		buffer1[i] = (char*) malloc(4*sizeof(char));
	}
	pthread_t prod;
	pthread_t consum;

	sem_init(&sem_prod,0,tamBuffer);
	sem_init(&sem_con,0,0);
	
	pthread_create(&prod,NULL,productor,(void*)&tamBuffer);
	pthread_create(&consum,NULL,consumidor,(void*)&tamBuffer);

	pthread_join(consum,NULL);
	pthread_join(prod,NULL);	

	for(int i=0;i<tamBuffer;i++){
		free(buffer1[i]);
	}
	free(buffer1);

	return 0;
}
