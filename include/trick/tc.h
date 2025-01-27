/*
PURPOSE:
     (Specify the trickcomm communications library API and structure definition)
PROGRAMMERS:
    (((Kyle Harvey) (MDAC) (1993) (--) (Initial Version))
    ((Eddie J. Paddock) (LinCom) (March 1996) (--) (Modified Version))
    ((Scott Killingsworth) (LinCom) (July 1998) (--) (Cleanup and add WIN32)))
*/

#ifndef TC_H
#define TC_H

#ifdef __WIN32__
#   include <winsock2.h>
#   include <ws2tcpip.h>
#else
#  include <arpa/inet.h>
#endif

#include "trick/trick_error_hndlr.h"

#if (__linux__)
#  define TC_NOSIGNAL MSG_NOSIGNAL
#else
#  define TC_NOSIGNAL 0
#endif

/* Status codes */
#define TC_SUCCESS                      0
#define TC_COULD_NOT_OPEN_SOCKET        1
#define TC_COULD_NOT_BIND_SOCKET        2
#define TC_COULD_NOT_LISTEN_SOCKET      3
#define TC_COULD_NOT_ACCEPT             4
#define TC_DRIVER_NOT_INSTALLED         5
#define TC_DRIVER_ALREADY_INSTALLED     6
#define TC_DRIVER_IS_NULL               7
#define TC_HOST_NOT_FOUND               8
#define TC_COULD_NOT_CONNECT            9
#define TC_RESOURCE_EXCEEDED            10
#define TC_DRIVER_NOT_FOUND             11
#define TC_DRIVER_ALREADY_DISCONNECTED  12
#define TC_CONN_DISABLED                13

#define TC_COULD_NOT_SET_TTL            14
#define TC_COULD_NOT_ADD_MEMBERSHIP     15

#define TC_EWOULDBLOCK                  -1
#define TC_READWRITE_TIMEOUT            -2
#define TC_BROKEN_PIPE                  -3

/* Set default for blocking timeout limit high */
#define MAX_BLOCKIO_TIMEOUT_LIMIT   1.0e20

/* WIN32 Support */
#ifdef __WIN32__
#  define TRICKCOMM_SOCKET_FAMILY PF_INET
#  define TRICKCOMM_SOCKET_PROTO IPPROTO_TCP
#  define TRICKCOMM_INVALID_SOCKET -1
#  define TRICKCOMM_EWOULDBLOCK WSAEWOULDBLOCK
#  define TRICKCOMM_EINTR WSAEINTR
#  define TRICKCOMM_EAGAIN WSATRY_AGAIN
#  define TRICKCOMM_EPIPE WSAENOTCONN
#  define EWOULDBLOCK WSAEWOULDBLOCK
#  define CLOSE_SOCKET closesocket
#  define tc_errno WSAGetLastError()
int tc_StartWinSock(void);
#else
#  define TRICKCOMM_SOCKET_FAMILY AF_INET
#  define TRICKCOMM_SOCKET_PROTO 0
#  define TRICKCOMM_INVALID_SOCKET -1
#  define TRICKCOMM_EWOULDBLOCK EWOULDBLOCK
#  define TRICKCOMM_EINTR EINTR
#  define TRICKCOMM_EAGAIN EAGAIN
#  define TRICKCOMM_EPIPE EPIPE
#  define CLOSE_SOCKET close
#  define tc_errno errno
#endif

/* Define a macro to yield the processor - TC_RELEASE() */
#if ( __WIN32__ )
#  define TC_RELEASE() Sleep(0)
#else
#  define TC_RELEASE() usleep(1)
#endif

#define TC_TAG_LENGTH 80
#define TC_BYTE_ORDER_NDX   0
#define TC_LONG_SIZE_NDX    1
#define TC_BYTE_INFO_LENGTH 2
#define TC_MULT_PORT 12345
#define TC_MULT_GROUP "225.0.0.40"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TC_COMM_FALSE = 0,      /* -- False flag value    */
    TC_COMM_TRUE = 1,       /* -- True flag value     */
    TC_COMM_NO = 0,         /* -- No flag value       */
    TC_COMM_YES = 1,        /* -- Yes flag value      */
    TC_COMM_DISABLED = 0,   /* -- Disabled flag value */
    TC_COMM_ENABLED = 1     /* -- Enabled flag value  */
} TCCommFlag;

/*
 * WARNING : These flags are the reverse logic of BLOCKIO and NOBLOCKIO.
 *           This is so that if a user does not set the blockio_flag
 *          in a device, the device will default to blocking
 */
typedef enum {
    TC_COMM_BLOCKIO = 0,        /* -- Set blocking I/O     */
    TC_COMM_NOBLOCKIO = 1,      /* -- Set non-blocking I/O */
    TC_COMM_TIMED_BLOCKIO = 2,  /* -- Set timed software block_io */
    TC_COMM_ALL_OR_NOTHING = 3  /* -- Set non-blocking to
                                          all or nothing */
} TCCommBlocking;

typedef struct {

    int socket;                        /* ** Socket file descriptor */
    int socket_type ;                  /* ** Socket type SOCK_STREAM or SOCK_DGRAM */
    TCCommFlag disabled;               /* -- Connection enabled flag */
    TCCommFlag disable_handshaking;    /* -- Set this to skip handshaking in tc_connect and tc_accept. */
    double blockio_limit;              /* s  Block I/O timeout limit */
    TCCommBlocking blockio_type;       /* -- E.g. spinloops */
    unsigned char byte_info[TC_BYTE_INFO_LENGTH]; /* ** byte swap info */
    int client_id;                     /* -- client id, other side.         Set by tc_accept. */
    char client_tag[TC_TAG_LENGTH];    /* -- client tag, other side.        User specified.
                                                                              Defaults if unspecified.*/
    struct in_addr client_addr;        /* ** client tag, other side.        Set by tc_connect. */
    TrickErrorHndlr *error_handler;    /* ** error handler.                 User specified.
                                                                              Defaults if unspecified.*/
    char *hostname;                    /* -- Hostname of server/client.     User specified. */
    int port;                          /* -- TCP/IP Port number.            User specified. */
    struct sockaddr_in remoteServAddr; /* ** Remote server address for UDP. Set by tc_init_udp_client or
                                                                                   tc_init_mcast_client */
    char *mcast_group;                 /* -- Multicast group address.       User specified. */
    unsigned char ttl ;                /* ** IP MULTICAST "Time to live"    User specified. */
    struct sockaddr_in cliAddr;        /* ** Client Address from tc_read for UDP. Set by tc_read. */
} TCDevice;

/* Struct to send over the multicast port */
typedef struct {
    char addr[40];                  /* ** address of connection */
    int port;                       /* ** port of connection */
    int pid;                        /* ** pid of connection */
    int conn_initiated;             /* ** Return flag to initiate
                                          connection */
    int disabled;                   /* ** 0=Communication enabled
                                          1=device disabled */
    char my_tag[TC_TAG_LENGTH];     /* ** tag of my process */
    char other_tag[TC_TAG_LENGTH];  /* ** tag of the other side */
} SEND_ME;

/* Multicast broadcasting info struct */
typedef struct {
    SEND_ME send_me;            /* ** Struct to continuously send */
    TCDevice *device;           /* ** Device to connect to */
    struct sockaddr_in addr;    /* ** Address to multicast on */
    int fd;                     /* ** File descriptor to write to */
} BC_INFO;

#ifdef __cplusplus
}
#endif

#endif
