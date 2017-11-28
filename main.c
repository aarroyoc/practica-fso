#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

char** buffer1;

void* productor(void* z)
{

	pthread_exit(NULL);
}

void* consumidor(void* z)
{
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
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
	pthread_t prod;
	pthread_t consum;
	
	pthread_create(&prod,NULL,productor,NULL);
	pthread_create(&consum,NULL,consumidor,NULL);

	pthread_join(consum,NULL);
	pthread_join(prod,NULL);	

	return 0;
}
