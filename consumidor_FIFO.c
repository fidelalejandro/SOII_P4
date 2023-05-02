#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>

//Constantes: Tamaño del array, número de mensajes a enviar, primer parámetros de los mq_open(const char*)
#define MAX_BUFFER 5
#define DATOS_A_CONSUMIR 100
#define ALMACEN1 "/ALMACEN1"
#define ALMACEN2 "/ALMACEN2"

/* cola de entrada de mensajes para el productor */
mqd_t almacen1;
/* cola de entrada de mensajes para el consumidor */
mqd_t almacen2;

/**
 * Función que consume los datos producidos por el productor manejando las colas de datos correspondientes.
 */
 void consumidor(void){
    char mensaje;
    int i;
    //Llenado del buffer almacen1 con mensajes vaciós
    for(i=0;i<MAX_BUFFER;i++) {
        mensaje='-';
        mq_send(almacen1,&mensaje,sizeof(char),1);
        printf("Se han enviado\n");
    }

    //Bucle en el que se consumen los datos realizando las iteraciones que se definan en DATOS_A_CONSUMIR
    for(i=0;i<DATOS_A_CONSUMIR;i++){
        mq_receive(almacen2,&mensaje,sizeof(char),NULL);
        printf("\n\033[33m[C] Se ha enviado: %c \033[0m\n", mensaje);
        mq_send(almacen1,&mensaje,sizeof(char), 0);
        printf("\033[35m[C] Se ha consumido: %c \033[0m \n", mensaje);

    }

    //Bucle que consume los elementos que faltaban por consumir del almacen1
    for(i=0;i<MAX_BUFFER;i++){
        mq_receive(almacen1,&mensaje,sizeof(char),NULL);
    }

}

void main(void) {
    struct mq_attr attr; /* Atributos de la cola */
    attr.mq_maxmsg = MAX_BUFFER;
    attr.mq_msgsize = sizeof (char);

    /* Apertura de los buffers */
    almacen1 = mq_open(ALMACEN1, O_RDWR, 0777, &attr);
    almacen2 = mq_open(ALMACEN2, O_RDONLY, 0777, &attr);

    /*Comprobación de que se han abierto correctamente los buffers */
    if ((almacen1 == -1) || (almacen2 == -1)) {
        perror ("mq_open");
        exit(EXIT_FAILURE);
    }

    /* Se reinicia la semilla para generar números aleatorios */
    srand(time(NULL));

    printf("---------------Consumidor FIFO---------------------\n");
    consumidor();

    mq_getattr(almacen2, &attr);
    printf("A la cola le quedan C:%ld\n",attr.mq_curmsgs);
    mq_getattr(almacen1, &attr);
    printf("A la cola le quedan P:%ld\n",attr.mq_curmsgs);

    /* Se cierran los buffers*/
    mq_close(almacen1);
    mq_close(almacen2);

    /* Se destruyen las colas */
    mq_unlink("/ALMACEN1");
    mq_unlink("/ALMACEN2");

    exit(EXIT_SUCCESS);
}