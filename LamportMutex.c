#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "LamportMutex.h"

#define bool char

char run = 0;
int v[7];
int id;
int q[7];

void initMutex(int myId)
{
    //printf("\ninitLamport\n");
    id = myId;
    int i;

    for (i = 0; i < 7; i++)
    {
        q[i] = 999999999;
        v[i] = 0;
    }

    v[myId] = 1;
}

bool isGrater(int entry1, int pid1, int entry2, int pid2)
{
    //printf("%d(v peticio) - %d(jo) - %d(v altre peticio) - %d(ell)\n", entry1, pid1, entry2, pid2);
    return ((entry1 > entry2) || ((entry1 == entry2) && (pid1 > pid2)));
}

bool okayCS(int vAux[2], int idAux[2])
{
    int i;
    for(i = 0; i < 2; i++)
    {
        if (isGrater(q[id], id, q[idAux[i]], idAux[i]))
            return 0;
        if (isGrater(q[id], id, v[idAux[i]], idAux[i]))
            return 0;
    }

    return 1;
}

void requestCS(int* fdW, int fdR[3], int fdServer, int jo)
{
    v[id]++;
    q[id] = v[id];
    int i;
    int vAux[2];
    int idAux[2];

    //broadcast
    for(i = 1; i < 3; i++)
    {
        write(fdW[i], "request", strlen("request")* sizeof(char));
        write(fdW[i], &q[id], sizeof(int));
        write(fdW[i], &id, sizeof(int));
    }

    char buff[3];
    for(i = 1; i < 3; i++)
    {
        read(fdW[i], buff, strlen("ack")* sizeof(char));
        read(fdW[i], &vAux[i-1], sizeof(int));
        read(fdW[i], &idAux[i-1], sizeof(int));

        v[idAux[i-1]] = v[idAux[i - 1]] > vAux[i - 1] ? v[idAux[i - 1]] : vAux[i - 1];
        v[id] = v[id] > vAux[i - 1] ? v[id] + 1 : vAux[i - 1] + 1;
    }

    while(!okayCS(vAux, idAux))
    {
        sleep(1);
    }
}

void* dedicatedServer(void* info)
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
            run = 1;
            continue;
        }

        read(localFd, &vAux, sizeof(int));
        read(localFd, &idAux, sizeof(int));

        //idAux = src
        v[idAux] = v[idAux] > vAux ? v[idAux] : vAux;
        v[id] = v[id] > vAux ? v[id] + 1 : vAux + 1;

        if (!strcmp(buff, "request"))
        {
            q[idAux] = vAux;
            write(localFd, "ack", strlen("ack")* sizeof(char));
            write(localFd, &v[id], sizeof(int));
            write(localFd, &id, sizeof(int));
            //printf("request - %d(jo) - %d(ell) - %d(v ell)\n", id, idAux, vAux);
        }

        if (!strcmp(buff, "release"))
        {
            q[idAux] = 999999999;
            //printf("release - %d(jo) - %d(altre)\n", id, idAux);
        }
    }
}

void releaseCS(int* fdW, int jo)
{
    q[id] = 999999999;

    int i;
    for(i = 1; i < 3; i++)
    {
        write(fdW[i], "release", strlen("release")* sizeof(char));
        write(fdW[i], &v[id], sizeof(int));
        write(fdW[i], &id, sizeof(int));
    }
}

char getRunLamport()
{
    return run;
}

void setRunLamport(char num)
{
    run = num;
}