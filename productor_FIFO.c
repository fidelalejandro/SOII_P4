#include <time.h> //Para la semilla que nos permitirá a partir del tiempo obtener valores aleatorios
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h> //Para el correcto envío de mensajes con las distintas funciones POSIX
#include <unistd.h> //Para las llamadas a la función sleep

//Constantes: Tamaño del array, número de mensajes a enviar, primer parámetros de los mq_open(const char*) y colores
#define MAX_BUFFER 5
#define DATOS_A_PRODUCIR 100
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
	//Declaración de variables
	char item;
	int i;
	char mensaje;
	
	for(i=0;i<DATOS_A_PRODUCIR;i++){
	 	//sleep(1);
		item=produce_item(i);
		mq_receive(almacen1,&mensaje, sizeof(char),NULL);
		printf("\n(P): Se ha recibido--%c--\n", mensaje);
		mensaje = item;
		mq_send(almacen2,&mensaje, sizeof(char), 0);
		printf("(P):Se ha enviado--%c--\n", mensaje);
		
	}
}

void main(void) {
    struct mq_attr attr; /* Atributos de la cola */
    attr.mq_maxmsg = MAX_BUFFER;
    attr.mq_msgsize = sizeof (char);

    /* Borrado de los buffers de entrada
    por si exist´ıan de una ejecuci´on previa*/
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

    productor();

    mq_close(almacen1);
    mq_close(almacen2);

    exit(EXIT_SUCCESS);
}
