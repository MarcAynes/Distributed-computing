#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>


#define nombreDeServers 3
#define printF(x) write(1, x, strlen(x))
#define portAConnectar 8200 //port base si tenim 10 servers, tindrem del 2200 - 2210
#define portMeu 8202

pthread_t thread_id;

char* numeroPelQueEsMontaAixo = "0";

int numThreads;

int prioritat = 1;

int regioCritica = 0;

char vullTreballar = 0;

char treballant = 0;

typedef struct{
    int fd;
}infoThread;

infoThread* threadList;

/*
 *
 * Protocol de comunicació:
 *
 * R: Vull accedir a la regió critica.
 *
 * D: dades noves enviant-se
 *
 * K: OK -> pots treballar
 *
 *
 */


void* dedicatedServer(void* info){

    infoThread thread = *((infoThread*) info);
    int localFd = thread.fd;

    char messageCode;

    while(1){
        read(localFd, &messageCode, 1);
        switch (messageCode){
            case 'R':
                ;
                int prioritatClient;
                read(localFd, &prioritatClient, sizeof(int));

                while (treballant){         //si estic treballant espero al acabar per respondre que poden continuar
                    continue;
                }

                if (vullTreballar){         //si vull treballar haig de mirar la prioritat
                    if(prioritatClient < prioritat){ //ell té mes prioritat que jo
                        write(localFd, "K", 1);
                    }else{
                        while (vullTreballar){         //quan no vulgui treballar li enviare el ok.
                            continue;
                        }

                        write(localFd, "K", 1);
                    }
                }else{                      //si no vull treballar, ells poden actuar perfectament
                    write(localFd, "K", 1);
                }
                break;

            case 'D':   //nova dada a modificar
                ;
                int novaDada;
                read(localFd, &novaDada, sizeof(int));
                regioCritica = novaDada;
                char string[20];
                sprintf(string, "%d\n", regioCritica); //printem la dada per pantalla
                printF(string);
                break;

            default:
                printF("Error amb les trames");
                kill(getpid(), SIGINT);
                break;
        }
    }


}


void* serverConnection(){
	
	printF("server obert\n");

    struct sockaddr_in my_addr, cli_addr;
    int fdSocket = 0;

    fdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(-1 ==  fdSocket){
		printF("Error en la creacion del socket\n");
	}

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((uint16_t) portMeu);
    inet_aton("127.0.0.1", &my_addr.sin_addr);

    int bindN = bind(fdSocket, (struct sockaddr *) &my_addr, sizeof(my_addr));
	
	if(bindN == -1){
		printF("Error en el bind\n");
	}

    threadList = (infoThread * )malloc(sizeof(infoThread));
    numThreads = 0;
    listen(fdSocket, 5);
    while(1) {
        socklen_t len = sizeof(cli_addr);

        int fdClient = accept(fdSocket, (struct sockaddr *) &cli_addr, &len);
        threadList = (infoThread *) realloc(threadList, sizeof(infoThread) * (numThreads + 1));
        threadList[numThreads].fd = fdClient;
        if (-1 != fdClient) {
            pthread_t thread;

            if (0 != pthread_create(&thread, NULL, dedicatedServer, (void *) (&threadList[numThreads]))) {

                //error al creat thread

            } else {

                //thread creat
                numThreads++;
            }
        }
    }
}
/*
int main() {

    int estat = pthread_create(&thread_id, NULL, serverConnection, NULL);

    struct sockaddr_in *serv_addr;
    serv_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

    int i;

    int port = portAConnectar;

    int *sockfd = (int*)malloc(sizeof(int));

    for(i = 0; i < nombreDeServers; i++){
        if(portMeu != port + i ) {
            serv_addr = (struct sockaddr_in *)realloc(serv_addr, sizeof(struct sockaddr_in)*(i+1));

            serv_addr[i].sin_port = htons((uint16_t) (port + i));
            inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr);
            serv_addr[i].sin_family = AF_INET;

            sockfd = (int*)realloc(sockfd, sizeof(int)*(i+1));

            sockfd[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            int connectS = connect(sockfd[i], (struct sockaddr *) &serv_addr[i], sizeof(serv_addr));
        }
    }

    char flag = 0;

    while(1) {

        char *pepe = (char *) malloc(sizeof(char) * 10);

        //if(flag){

         //   pepe[0] = '2';
        //}else{

            read(0, pepe, 10);
        //}


        if (pepe[0] == '1') {

            printF(numeroPelQueEsMontaAixo);
        }

        if (treballa){
            free(pepe);
            continue;
        }

        if(pepe[0] == '2'){
            treballa = 1;
            int i;
            for(i = 0; i < numThreads; i++){

                write(threadList[i].fd, prioritat, sizeof(int));
            }

            char gutavo;
            flag = 0;
            for(i = 0; i < numThreads; i++){

                read(threadList[i].fd, &gutavo, sizeof(char));

                if (gutavo != 'K'){

                    flag = 1;
                    free(pepe);
                    break;
                }
            }

            if (flag){

                continue;
            }

            numeroPelQueEsMontaAixo[0]++;
            flag = 0;
            treballa = 0;
            for(i = 0; i < numThreads; i++){

                write(threadList[i].fd, numeroPelQueEsMontaAixo[0], sizeof(char));
            }

        }

        free(pepe);
    }

    return 0;
}*/

int main(){
    //creacio del servidor on acceptarem connexions
    pthread_create(&thread_id, NULL, serverConnection, NULL);

    struct sockaddr_in *serv_addr;
    serv_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

    int i;

    int port = portAConnectar;

    int *sockfd = (int*)malloc(sizeof(int));

    //connexió amb tots els altres servidors
    for(i = 0; i < nombreDeServers; i++){
        //si el port al que ens volem connectar no es el meu, creem el socket, si es meu fem el següent.
        if(portMeu != port ) {
            serv_addr = (struct sockaddr_in *)realloc(serv_addr, sizeof(struct sockaddr_in)*(i+1));
			
			memset(&serv_addr[i], 0, sizeof(serv_addr[i]));
			serv_addr[i].sin_family = AF_INET;
            serv_addr[i].sin_port = htons((uint16_t) (port));
			serv_addr[i].sin_addr.s_addr = inet_addr("127.0.0.1");
            //inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr);

            sockfd = (int*)realloc(sockfd, sizeof(int)*(i+1));
            sockfd[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			printf("%d\n", sockfd[i]);
            int connectS;
            do{
                connectS = connect(sockfd[i], (struct sockaddr *) &serv_addr[i], sizeof(serv_addr[i]));
                sleep(1);
				char buff[128];
				int bytes = sprintf(buff, "errno says: %s\n", strerror(errno)); // molt útil
            	write(1, buff, bytes);
				//comprovació error al crear la connexió (el client pot no estar engegat en aquell moment)
			}while(connectS == -1); //-1 error 0 correct

        }
        port++;
    }

    //codi per treballar
    printF("Tots els servidors en linia\n");
    char basura;
    while(1){
        //lectura de \n que indicara que volem accedir a regio critica
        read(0, &basura, 1);
        vullTreballar = 1;  //volem treballar aixi que hem d'avisar als threads amb la variable global
        port = portAConnectar;

        //iterem per tots els servers creats (nombre total de servers 4 - 1 ja que el 4 soc jo)
        for(i = 0; i < nombreDeServers - 1; i++){
            write(sockfd[i], "R", 1); //R = Regio critica -> vull accedir a la regio critica
            write(sockfd[i], &prioritat, sizeof(int));  //enviem el nostre id (prioritat)
        }

        char response;
        for(i = 0; i < nombreDeServers - 1; i++){
            read(sockfd[i], &response, 1);
            if(response == 'K'){            //K = OK, perfe -> tens permis
                continue;
            }
        }

        treballant = 1;
        regioCritica++;

        for(i = 0; i < nombreDeServers - 1; i++){
            write(sockfd[i], "D", 1); //D: data -> nova info disponible
            write(sockfd[i], &regioCritica, sizeof(int));  //enviem la nova dada
        }

        vullTreballar = 0;
        treballant = 0;

    }


}
