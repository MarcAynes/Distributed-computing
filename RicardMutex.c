#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "RicardMutex.h"

int myts = 999999999;
int c = 1;
int pendingQ[2];
int numOkay = 0;
int id;

void initRicard(int idAux)
{
    //printf("\ninitRicard\n");
    id = idAux;
    pendingQ[0] = -1;
    pendingQ[1] = -1;
}

void requestRicardCS(int* fdW)
{
    c++;
    myts = c;

    numOkay = 0;

    int i;
    //broadcast
    for(i = 1; i < 2; i++)
    {
        //printf("%d(jo) - %d(fd desti)\n", id, fdW[i]);
        write(fdW[i], "request", strlen("request")* sizeof(char));
        write(fdW[i], &myts, sizeof(int));
        write(fdW[i], &id, sizeof(int));
    }

    char buffer[4];
    int cAux;
    int idAux;
    for(i = 1; i < 2; i++)
    {
        read(fdW[i], buffer, strlen("okay")* sizeof(char));
        read(fdW[i], &cAux, sizeof(int));
        read(fdW[i], &idAux, sizeof(int));
        //printf("%d(jo) - okay rebut de - %d(id rebut)\n", id, idAux);
        c = c > cAux ? c + 1 : cAux + 1;
    }

    /*while (numOkay < 1) // 1 = N - 1 => N = 2 => 2 - 1
    {
        sleep(1);
    }*/
}

void releaseRicardCS()
{
    myts = 999999999;

    //we should need a for here but we only have one server more so we can hardcore this point.
    write(pendingQ[0], "okay", strlen("okay")* sizeof(char));
    write(pendingQ[0], &c, sizeof(int));
    write(pendingQ[0], &id, sizeof(int));
    pendingQ[0] = -1;
}

void* dedicatedServerRicard(void* info)
{
    int thread = *((int*) info);
    int localFd = thread;
    char buff[8];
    int vAux;
    int idAux;

    while(1)
    {
        read(localFd, buff, sizeof(char)*7);
        buff[7] = '\0';

        if (buff[0] == 'G')
        {
            runRicard = 1;
            continue;
        }

        read(localFd, &vAux, sizeof(int));
        read(localFd, &idAux, sizeof(int));

        c = c > vAux ? c + 1 : vAux + 1;

        if (!strcmp(buff, "request"))
        {
            //printf("%d(jo) - %d(myts) - %d(vaux) - %d(id)\n", id, myts, vAux, idAux);
            if ((myts == 999999999) || (vAux < myts) || ((vAux == myts) && (idAux < id)))
            {
                //printf("okay - %d(jo)\n", id);
                write(localFd, "okay", strlen("okay")* sizeof(char));
                write(localFd, &c, sizeof(int));
                write(localFd, &id, sizeof(int));
            }else{
                pendingQ[0] = localFd;
            }
        }

        if (!strcmp(buff, "okay"))
        {
           numOkay++;
        }
    }
}