#include <stdio.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>

#define printF(x) write(1, x, strlen(x))

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

int main() {

    int estat = pthread_create(&thread_id, NULL, serverConnection, NULL);

    struct sockaddr_in serv_addr;

    serv_addr.sin_port = htons((uint16_t) atoi("2201"));
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_family = AF_INET;

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int connectS = connect( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

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
}