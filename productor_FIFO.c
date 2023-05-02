#include <time.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h> 
#include <unistd.h>

//Tamaño del buffer
#define MAX_BUFFER 5
//Número de mensajes a enviar
#define DATOS_A_PRODUCIR 100
//Primer parámetros de los mq_open
#define ALMACEN1 "/ALMACEN1"
#define ALMACEN2 "/ALMACEN2"

/* cola de entrada de mensajes para el productor */
mqd_t almacen1;
/* cola de entrada de mensajes para el consumidor */
mqd_t almacen2;


int produce_item(int iter){
	int item;
	//Se generan las letras en función de la letra y el máximo de su posición
	item = 'a'+ (iter%MAX_BUFFER);
	return item;
}

void productor(){
	//Valor producido
	char item;
    //Valor a enviar
    char mensaje;
	int i;

    printf("Espera a que se llene el buffer\n");
	
	for(i=0;i<DATOS_A_PRODUCIR;i++){

	 	//sleep(1);
        //Se genera el elemento
		item=produce_item(i);
		mq_receive(almacen1,&mensaje, sizeof(char),NULL);
        printf("\n\033[33m[P] Se ha recibido: %c \033[0m\n", mensaje);
		mensaje = item;
		mq_send(almacen2,&mensaje, sizeof(char), 0);
        printf("\033[35m[P] Se ha enviado: %c \033[0m \n", mensaje);
		
	}
}

void main(void) {
    struct mq_attr attr; /* Atributos de la cola */
    attr.mq_maxmsg = MAX_BUFFER;
    attr.mq_msgsize = sizeof (char);

    /* Borrado de los buffers de entrada
    por si existían de una ejecuci´on previa*/
    mq_unlink(ALMACEN1);
    mq_unlink(ALMACEN2);

    /* Apertura de los buffers */
    almacen1 = mq_open(ALMACEN1, O_CREAT|O_RDONLY, 0777, &attr);
    almacen2 = mq_open(ALMACEN2, O_CREAT|O_WRONLY, 0777, &attr);

    if ((almacen1 == -1) || (almacen2 == -1)) {
        perror ("mq_open");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    printf("---------------Productor FIFO---------------------\n");
    productor();

    mq_close(almacen1);
    mq_close(almacen2);

    exit(EXIT_SUCCESS);
}
