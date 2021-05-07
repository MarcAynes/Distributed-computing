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
#include "LamportMutex.h"
#include "RicardMutex.h"


#define nombreDeServers 7
#define printF(x) write(1, x, strlen(x))
#define portAConnectar 8200 //port base si tenim 10 servers, tindrem del 2200 - 2210
//#define portMeu 8200

int portMeu = 8200;

int fdHigh; //read
int sockHFd; //write

int number = -1;

int fdSocketsRead[3];
int *sockfd;

pthread_t thread_id;
pthread_t mainId;
pthread_t dedicat;

int numConnections;

int prioritat = 1;

int regioCritica = 0;

char vullTreballar = 0;

char treballant = 0;

char flagThreadFinished = 0;

void highWeightLamport(int jo)
{
    char token = 1;
    char basura;

    //printf("\n%d - inici hiahgLamport \n", number);

    while(1)
    {
        while(!token)
        {
            printf("HighWeight Lamport token: %d\n", token);
            read(fdHigh, &token, 1);
        }

        int i = 0;
        for(i = 0; i < 3; i++)
        {
            write(sockfd[i], "G", 1);
        }

        for(i = 0; i < 3; i++)
        {
            read(fdSocketsRead[i], &basura, 1);
        }

        write(sockHFd, &token, 1);
        token = 0;
    }
}

void lightWeightLamport(int jo)
{
    int fdServer;
    //printf("%d - lightLamport\n", number);

    while(1)
    {
        while (!getRunLamport())
            usleep(100);

        //printf("%d - 2lightLamport\n", number);
        setRunLamport(0);
        requestCS(sockfd, fdSocketsRead, fdServer, jo);

        int i;
        for (i = 0; i < 10; i++)
        {
            printf("soc el proces lightweight Lamport %d\n", number-1);
            sleep(1);
        }

        releaseCS(sockfd, jo);
        write(sockfd[0], "G", sizeof(char));
    }
}

void highWeightRicard(int jo)
{
    char token = 0;
    char basura;
    //printf("\n %d - inici hiagh Ricard \n", number);

    while(1)
    {
        while(!token)
        {
            //printf("HighWeight Ricard token: %d\n", token);
            read(fdHigh, &token, 1);
        }

        int i = 0;
        for(i = 0; i < 2; i++)
        {
            write(sockfd[i], "G", 1);
        }

        for(i = 0; i < 2; i++)
        {
            read(fdSocketsRead[i], &basura, 1);
        }
        printf("enviar token a lamport\n");

        write(sockHFd, &token, 1);
        token = 0;
    }
}

void lightWeightRicard()
{
    while(1)
    {
        while (!runRicard)
            usleep(100);

        runRicard = 0;
        requestRicardCS(sockfd);

        int i;
        for (i = 0; i < 10; i++)
        {
            printf("soc el proces lightweight Ricard %d\n", number-4);
            sleep(1);
        }
        releaseRicardCS();
        write(sockfd[0], "G", sizeof(char));
    }
}

void* serverHigh()
{
    printF("server obert\n");

    struct sockaddr_in my_addr, cli_addr;
    int fdSocket = 0;

    fdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(-1 ==  fdSocket)
    {
        printF("Error en la creacion del socket\n");
    }

    portMeu = 9000+number;

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((uint16_t) portMeu);
    inet_aton("127.0.0.1", &my_addr.sin_addr);

    int bindN = bind(fdSocket, (struct sockaddr *) &my_addr, sizeof(my_addr));

    if(bindN == -1)
    {
        printF("Error en el bind\n");
    }

    listen(fdSocket, 1);
    while(1)
    {
        socklen_t len = sizeof(cli_addr);

        fdHigh = accept(fdSocket, (struct sockaddr *) &cli_addr, &len);

        if (fdHigh > -1)
            break;

        printf("%d - Error high server\n", number);
    }

    //pthread_kill(mainId, SIGUSR1);
    flagThreadFinished = 1;
    pthread_exit(NULL);
}

void* serverConnection()
{
	printf("server obert - %d\n", number);

    struct sockaddr_in my_addr, cli_addr;
    int fdSocket = 0;

    fdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(-1 ==  fdSocket)
	{
		printf("%d - Error en la creacion del socket\n", number);
	}

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((uint16_t) portMeu);
    inet_aton("127.0.0.1", &my_addr.sin_addr);

    int bindN = bind(fdSocket, (struct sockaddr *) &my_addr, sizeof(my_addr));
	
	if(bindN == -1)
	{
		printf("%d - Error en el bind\n", number);
	}

    numConnections = 0;
    listen(fdSocket, 3);
    while(1)
    {
        socklen_t len = sizeof(cli_addr);

        int fdClient = accept(fdSocket, (struct sockaddr *) &cli_addr, &len);
        if(fdClient < 3)
        {
			printf("fd: %d\n", fdClient);
		}
        fdSocketsRead[numConnections] = fdClient;
        numConnections++;

        if(number != 0 && number <= 3)                                          //no fa falta fer dedicats si soc el server jefe, es pot controlar manualment
            pthread_create(&dedicat, NULL, dedicatedServer, (void*)&fdClient);

        if (number > 4)
            pthread_create(&dedicat, NULL, dedicatedServerRicard, (void*)&fdClient);

        if (numConnections == 3)
        {
            break;
        }

        if(numConnections == 2 && number > 3)
        {
            break;
        }

    }

    //pthread_kill(mainId, SIGUSR1);
    flagThreadFinished = 1;
    pthread_exit(NULL);

}

void nothingHere()
{
    flagThreadFinished = 1;
}

int main(int argc, char** argv)
{
    int k = 0;
    int process[7];
    //creem tots els processos high i light weight.
    /*for (k = 0; k < 6; k++)
    {
        int pid = fork();

        if (pid == 0)
        {
            //child
            number = k;
            break;
        }

        if (pid > 0)
        {
            //father
            process[k] = pid; // guardem el pid del fill per si el volem matar mes tard
            portMeu++;
        }

        if(pid == -1)
        {
            //error
            printF("Error creating forks\n");
            int j;
            for (j = 0; j < k; j++)
            {
                kill(process[j], SIGKILL); // vaya que ha pachaaadooo
            }

            return 0;
        }
    }*/

    //number = number == -1 ? 6 : number;
    number = atoi(argv[1]);
    portMeu += number;
    printf("%d\n", number);
    /*
     * number == 0 || 4 => HighWeight
     * number <= 3 => lamport
     * 3 < number < 7 => ricard
     */
    //creacio del servidor on acceptarem connexions
    signal(SIGUSR1, nothingHere);
    mainId = pthread_self();
    pthread_create(&thread_id, NULL, serverConnection, NULL);

    struct sockaddr_in *serv_addr;
    serv_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

    int i, j;

    int port = portAConnectar;

    sockfd = (int *) malloc(sizeof(int));
    int jo;
    //connexió amb tots els altres servidors

    if (number > 3)
    {
        j = 0;
        port += 4;
        for (i = 0; i < 3; i++)
        {
            //si el port al que ens volem connectar no es el meu, creem el socket, si es meu fem el següent.
            if (portMeu != port)
            {
                serv_addr = (struct sockaddr_in *) realloc(serv_addr, sizeof(struct sockaddr_in) * (j + 1));

                memset(&serv_addr[j], 0, sizeof(serv_addr[j]));
                serv_addr[j].sin_family = AF_INET;
                serv_addr[j].sin_port = htons((uint16_t) (port));
                serv_addr[j].sin_addr.s_addr = inet_addr("127.0.0.1");
                //inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr);

                sockfd = (int *) realloc(sockfd, sizeof(int) * (j + 1));
                sockfd[j] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                //printf("%d(fd a connectar) - %d(jo) - %d(index)\n", sockfd[j], number, j);
                int connectS;
                do{
                    connectS = connect(sockfd[j], (struct sockaddr *) &serv_addr[j], sizeof(serv_addr[j]));
                    sleep(1);
                    char buff[128];
                    int bytes = sprintf(buff, "%d - 1 errno says: %s\n", number, strerror(errno)); // molt útil
                    write(1, buff, bytes);
                    //comprovació error al crear la connexió (el client pot no estar engegat en aquell moment)
                } while (connectS == -1); //-1 error 0 correct
                j++;
            }
            else
            {
                jo = i;
            }
            port++;
        }
        //printf("%d(jo) - %d - %d - connectats final\n", number, sockfd[0], sockfd[1]);

    }else{
        j = 0;
        for (i = 0; i < 4; i++)
        {
            //si el port al que ens volem connectar no es el meu, creem el socket, si es meu fem el següent.
            if (portMeu != port)
            {
                serv_addr = (struct sockaddr_in *) realloc(serv_addr, sizeof(struct sockaddr_in) * (j + 1));

                memset(&serv_addr[j], 0, sizeof(serv_addr[j]));
                serv_addr[j].sin_family = AF_INET;
                serv_addr[j].sin_port = htons((uint16_t) (port));
                serv_addr[j].sin_addr.s_addr = inet_addr("127.0.0.1");
                //inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr);

                sockfd = (int *) realloc(sockfd, sizeof(int) * (j + 1));
                sockfd[j] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                //printf("%d\n", sockfd[i]);
                int connectS;
                do {
                    connectS = connect(sockfd[j], (struct sockaddr *) &serv_addr[j], sizeof(serv_addr[j]));
                    sleep(1);
                    char buff[128];
                    int bytes = sprintf(buff, "%d - 1 errno says: %s\n",number,  strerror(errno)); // molt útil
                    write(1, buff, bytes);
                    //comprovació error al crear la connexió (el client pot no estar engegat en aquell moment)
                } while (connectS == -1); //-1 error 0 correct
                j++;

            } else {
                jo = i;
            }
            port++;
        }
    }

    //codi per treballar
    //printF("Tots els servidors en linia\n");
    //printf("Tots els servidors en linia - %d\n", number);

    while(!flagThreadFinished)
    {
        usleep(100);
    }
    flagThreadFinished = 0;

    signal(SIGUSR1, nothingHere);
    sleep(1);

    if (number == 0)
    {
        //printf("%d - HighWeight lamport\n", number);
        pthread_create(&thread_id, NULL, serverHigh, NULL);
        //printf("%d - HighWeight lamport3\n", number);
        struct sockaddr_in *serv_addr1;
        serv_addr1 = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

        port = 9004;

        int sck;
        memset(serv_addr1, 0, sizeof(*serv_addr1));
        (*serv_addr1).sin_family = AF_INET;
        (*serv_addr1).sin_port = htons((uint16_t) (port));
        (*serv_addr1).sin_addr.s_addr = inet_addr("127.0.0.1");
        //inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr);
        sockHFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        int connectS;
        do {
            connectS = connect(sockHFd, (struct sockaddr *) serv_addr1, sizeof(*serv_addr1));
            sleep(1);
            char buff[128];
            int bytes = sprintf(buff, "2 0errno says: %s - %d\n", strerror(errno), connectS); // molt útil
            write(1, buff, bytes);
            //comprovació error al crear la connexió (el client pot no estar engegat en aquell moment)
        } while (connectS == -1); //-1 error 0 correct

        //printf("%d - buscant flagThreadFinished\n", number);
        while(!flagThreadFinished)
        {
            usleep(100);
        }
        flagThreadFinished = 0;

        highWeightLamport(jo);
    }

    if (number < 4)
    {
        //printf("%d - lightweight Lamport\n", number);
        initMutex(number);

        lightWeightLamport(jo);
    }

    if (number == 4)
    {
        //printf("%d - highWeight Ricard\n", number);
        pthread_create(&thread_id, NULL, serverHigh, NULL);

        struct sockaddr_in *serv_addr1;
        serv_addr1 = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

        port = 9000;

        int sck;

        memset(serv_addr1, 0, sizeof(*serv_addr1));
        serv_addr1->sin_family = AF_INET;
        serv_addr1->sin_port = htons((uint16_t) (port));
        serv_addr1->sin_addr.s_addr = inet_addr("127.0.0.1");
        //inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr);

        sockHFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        int connectS;
        do {
            connectS = connect(sockHFd, (struct sockaddr *) serv_addr1, sizeof(*serv_addr1));
            sleep(1);
            char buff[128];
            int bytes = sprintf(buff, "2 4errno says: %s - %d\n", strerror(errno), connectS); // molt útil
            write(1, buff, bytes);
            //comprovació error al crear la connexió (el client pot no estar engegat en aquell moment)
        } while (connectS == -1); //-1 error 0 correct

        while(!flagThreadFinished)
        {
            usleep(100);
        }

        flagThreadFinished = 0;

        highWeightRicard(jo);
    }

    //printf("%d - lightWeight Ricard\n", number);

    initRicard(number);

    lightWeightRicard();

}
