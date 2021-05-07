#ifndef DISTRIBUTED_COMPUTING_LAMPORTMUTEX_H
#define DISTRIBUTED_COMPUTING_LAMPORTMUTEX_H

char getRunLamport();

void setRunLamport(char num);

void initMutex(int myId);

void requestCS(int* fdW, int fdR[3], int fdServer, int jo);

void* dedicatedServer(void* info);

void releaseCS(int* fdW, int jo);

#endif //DISTRIBUTED_COMPUTING_LAMPORTMUTEX_H
