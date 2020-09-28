#include <stdio.h>
#include <pthread.h>

pthread_t thread_id;


void serverConnection(){

}

int main() {

    int estat = pthread_create(&thread_id, NULL, serverConnection, NULL);


    int estat;
    estat = pthread_create(&thread_id, NULL, serverConnection, NULL);

    struct sockaddr_in serv_addr;

    serv_addr.sin_port = htons((uint16_t) atoi("2201"));
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_family = AF_INET;

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int connectS = connect( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));



    return 0;
}