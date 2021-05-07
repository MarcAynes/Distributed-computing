#ifndef DISTRIBUTED_COMPUTING_RICARDMUTEX_H
#define DISTRIBUTED_COMPUTING_RICARDMUTEX_H

char runRicard;

void releaseRicardCS();

void initRicard(int idAux);

void requestRicardCS(int* fdW);

void* dedicatedServerRicard(void* info);

#endif //DISTRIBUTED_COMPUTING_RICARDMUTEX_H
