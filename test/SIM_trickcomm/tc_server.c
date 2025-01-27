
#include <stdio.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>

#include "test_struct.h"

#if (__linux__)
#  define TC_NOSIGNAL MSG_NOSIGNAL
#else
#  define TC_NOSIGNAL 0
#endif

int main () {
    int nbytes ;
    TEST_DATA_STRUCT tds ;

    int on = 1;
    int listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);;
    struct sockaddr_in s_in;

    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, (socklen_t) sizeof(on));
    setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &on, (socklen_t) sizeof(on));
    memset(&s_in, 0 , sizeof(struct sockaddr_in)) ;
    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = INADDR_ANY ;
    s_in.sin_port = htons((short)9000);
    bind(listen_socket, (struct sockaddr *)&s_in, sizeof(s_in)) ;
    listen(listen_socket, SOMAXCONN) ;

    socklen_t length;
    int connection = accept(listen_socket, (struct sockaddr *) &s_in, &length);
    setsockopt(connection, IPPROTO_TCP, TCP_NODELAY, (const void *) &on, (socklen_t) sizeof(on));

    struct sockaddr_in cliAddr;
    struct sockaddr_in remoteServAddr;
    unsigned int cliLen = sizeof(struct sockaddr_in);
    nbytes = 1 ;
    while( nbytes > 0 ) {
        nbytes = recvfrom(connection, &tds, sizeof(TEST_DATA_STRUCT),
                          TC_NOSIGNAL, (struct sockaddr *) &cliAddr, &cliLen) ;
        fprintf(stderr, "nbytes = %d\n", nbytes ) ;
        fprintf(stderr, "tc_server int1 =%x int2 =%x\n", tds.int1 , tds.int2 ) ;
        fprintf(stderr, "tc_server long1=%lx long2=%lx\n", tds.long1 , tds.long2 ) ;
        if ( nbytes == sizeof(TEST_DATA_STRUCT)) {
            sendto(connection, &tds, sizeof(TEST_DATA_STRUCT), TC_NOSIGNAL,
                                    (struct sockaddr *) &remoteServAddr,
                                    (socklen_t) sizeof(struct sockaddr_in)) ;
        }
    }

    return 0 ;
}

