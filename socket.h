#if defined (WIN32)
    #include <winsock2.h>
    typedef int socklen_t;
#elif defined (linux)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket(s) close(s)
    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
#endif

struct server_sockets {
    SOCKET s_sock;
    SOCKET c_sock;
};

struct server_sockets host(int port);
SOCKET join(char *ip_address, int port);
void c_close(SOCKET sock);
void s_close(struct server_sockets);