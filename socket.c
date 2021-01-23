#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "socket.h"


struct server_sockets host(int port) {
    struct server_sockets conf = {INVALID_SOCKET, INVALID_SOCKET};
    SOCKADDR_IN sin;
    SOCKADDR_IN csin;
    socklen_t recsize = sizeof(csin);
    int sock_err;


    #if defined (WIN32)
        WSADATA WSAData;
        if (WSAStartup(MAKEWORD(2,2), &WSAData)) {
            printf("Impossible d'utiliser les sockets...\n");
            return conf;
        }
    #endif   

    conf.s_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(conf.s_sock != INVALID_SOCKET) {
        printf("La socket %d est maintenant ouverte en mode TCP/IP\n", conf.s_sock);

        sin.sin_addr.s_addr    = htonl(INADDR_ANY);   /* Adresse IP automatique */
        sin.sin_family         = AF_INET;             /* Protocole familial (IP) */
        sin.sin_port           = htons(port);         /* Listage du port */
        sock_err = bind(conf.s_sock, (SOCKADDR*)&sin, sizeof(sin));


        if (sock_err != SOCKET_ERROR) {
            sock_err = listen(conf.s_sock, 5);
            printf("Listage du port %d...\n", port);

            if(sock_err != SOCKET_ERROR) {
                printf("Patientez pendant que le client se connecte sur le port %d...\n", port);        

                conf.c_sock = accept(conf.s_sock, (SOCKADDR*)&csin, &recsize);
                printf("Un client se connecte avec la socket %d de %s:%d\n", conf.c_sock, inet_ntoa(csin.sin_addr), htons(csin.sin_port));
            }
        }
    }

    return conf;
}

 
 
SOCKET join(char *ip_address, int port) {
    #if defined (WIN32)
        WSADATA WSAData;
        if (WSAStartup(MAKEWORD(2,2), &WSAData)) {
            printf("Impossible d'utiliser les sockets...\n");
            return INVALID_SOCKET;
        }
    #endif
 
    SOCKADDR_IN sin;
   
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_addr.s_addr = inet_addr(ip_address);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if (connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR) {
        printf("Connection à %s sur le port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
        return sock;
    } else {
        printf("Impossible de se connecter\n");
        return INVALID_SOCKET;
    }
}

void c_close(SOCKET sock) {
    shutdown(sock, 2);
    closesocket(sock);

    #if defined (WIN32)
        WSACleanup();
    #endif
}

void s_close(struct server_sockets conf) {
    shutdown(conf.c_sock, 2);

    closesocket(conf.s_sock);

    #if defined (WIN32)
        WSACleanup();
    #endif
}