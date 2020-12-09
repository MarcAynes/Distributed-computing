#include <stdio.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>

#define nombreDeServers 1
#define printF(x) write(1, x, strlen(x))
#define portAConnectar 2200 //port base si tenim 10 servers, tindrem del 2200 - 2210
#define portMeu 2201

pthread_t thread_id;

char* numeroPelQueEsMontaAixo = "0";

int numThreads;

int prioritat = 1;

char treballa = 0;

typedef struct{

    int fd;
}infoThread;

infoThread* threadList;


void dedicatedServer(void* info){

    infoThread thread = *((infoThread*) info);
    int localFd = thread.fd;

    int joan;

    while (1) {
        read(localFd, &joan, sizeof(int));

        if (joan > prioritat && treballa) {
            char alex = 'F';
            int i;

            for (i = 0; i < numThreads; i++) {
                write(localFd, &alex, sizeof(char));
            }
        } else {
            char alex = 'K';
            int i;

            for (i = 0; i < numThreads; i++) {
                write(localFd, &alex, sizeof(char));
            }

            char nouNum;
            read(localFd, &nouNum, sizeof(char));

            numeroPelQueEsMontaAixo[0] = nouNum;
            treballa = 0;
        }
    }


}


void serverConnection(){

    struct sockaddr_in my_addr, cli_addr;
    int fdSocket = 0;

    fdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((uint16_t) atoi("2201"));
    inet_aton("127.0.0.1", &my_addr.sin_addr);

    int bindS = bind(fdSocket, (struct sockaddr *) &my_addr, sizeof(my_addr));

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
    int estat = pthread_create(&thread_id, NULL, serverConnection, NULL);

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

            serv_addr[i].sin_port = htons((uint16_t) (port));
            inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr);
            serv_addr[i].sin_family = AF_INET;

            sockfd = (int*)realloc(sockfd, sizeof(int)*(i+1));

            sockfd[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            int connectS;
            do{
                connectS = connect(sockfd[i], (struct sockaddr *) &serv_addr[i], sizeof(serv_addr));
                //comprovació error al crear la connexió (el client pot no estar engegat en aquell moment)
            }while(!connectS); //-1 error 0 correct


        }
    }
}