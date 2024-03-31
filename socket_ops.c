#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


struct sockaddr_in serv_addr, cli_addr;
struct sockaddr_in udp_serv_addr, udp_cli_addr, dest_addr;


void tcplistenfunc()
{
    // Sets up a new listening TCP socket 'object' for listening
    // and returns its address onto the stack
    if (data_stack_ptr < 1) {
        printf("tcplisten -- need <port_number> on the\n");
        return;
    }
    DCLANG_INT32 sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DCLANG_INT32 true = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(DCLANG_INT32));
    if (sockfd < 0) {
       perror("tcplisten -- ERROR opening socket.");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    DCLANG_INT32 portno = (DCLANG_INT32) dclang_pop();
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0)
          perror("ERROR on binding");
    listen(sockfd, 5);
    push((DCLANG_INT32) sockfd);
}


void tcpacceptfunc()
{
    // Take a given tcplisten-ready socket object and actually make it
    // accept a connection. Returns a handle to the established connection.
    // Can be used inside a loop as a basis for a "multi-connection handling"
    // forking server via `fork`.
    if (data_stack_ptr < 1) {
        printf("tcpaccept -- need <socket> on the stack");
        return;
    }
    DCLANG_UINT32 clilen = sizeof(cli_addr);
    DCLANG_INT32 sockfd = (DCLANG_INT32) dclang_pop();
    DCLANG_INT32 newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) perror("tcpaccept -- ERROR on accept step!");
    push((DCLANG_INT32) newsockfd);
}


void tcpconnectfunc()
{
    // A tcp client word. This can reach out to a pre-established tcp server
    // already set up with the above words.
    if (data_stack_ptr < 2) {
        printf("tcpconnect -- need <host> <port> on the stack");
        return;
    }
    DCLANG_INT32 sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) perror("tcpconnect -- ERROR opening socket");
    DCLANG_INT32 portno = (DCLANG_INT32) dclang_pop();
    struct sockaddr_in host_addr;
    char *host = (char *) (DCLANG_PTR) dclang_pop();
    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "tcpconnect -- ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &host_addr, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&host_addr.sin_addr.s_addr, server->h_length);
    host_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0)
        perror("tcpconnect -- ERROR connecting");
    push((DCLANG_INT32) sockfd);
}


/////////
// UDP //
/////////

// Possible TODO: make these more 'modular' and configurable w/re: socket opts.

void udprecvfunc() {
    // Receive data over UDP
    if (data_stack_ptr < 3) {
        printf("udprecv -- need <port_number> <max_bytes> <buffer> on the stack\n");
        return;
    }
    // stack values
    char *buffer = (char *) (DCLANG_PTR) dclang_pop();
    DCLANG_INT32 max_bytes = (DCLANG_INT32) dclang_pop();
    DCLANG_INT32 portno = (DCLANG_INT32) dclang_pop();
    // make a socket
    DCLANG_INT32 sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("udprecv -- ERROR opening socket");
        return;
    }
    socklen_t udp_clilen = sizeof(udp_cli_addr);
    bzero((char *) &udp_serv_addr, sizeof(udp_serv_addr));
    udp_serv_addr.sin_family = AF_INET;
    udp_serv_addr.sin_addr.s_addr = INADDR_ANY;
    udp_serv_addr.sin_port = htons(portno);
    // bind the socket
    if (bind(sockfd, (struct sockaddr *) &udp_serv_addr,
             sizeof(udp_serv_addr)) < 0) {
        perror("udprecv -- ERROR on binding");
    }
    ssize_t num_bytes = recvfrom(
        sockfd, buffer, max_bytes, 0,
        (struct sockaddr *)&udp_cli_addr, &udp_clilen
    );
    if (num_bytes < 0) {
        perror("udprecv -- ERROR receiving data");
    }
    buffer[num_bytes] = '\0'; // Null terminate the received data
    close(sockfd);
    push((DCLANG_INT32) num_bytes);
}


void udpsendfunc() {
    // Send data over UDP to a specified host and port
    if (data_stack_ptr < 3) {
        printf("udpsend -- need <host> <port> <buffer> on the stack\n");
        return;
    }
    // stack values
    char *buffer = (char *) (DCLANG_PTR) dclang_pop();
    DCLANG_INT32 portno = (DCLANG_INT32) dclang_pop();
    char *host = (char *) (DCLANG_PTR) dclang_pop();
    // make a socket
    DCLANG_INT32 sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("udpsend -- ERROR opening socket");
        return;
    }
    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "udpsend -- ERROR, no such host\n");
        return;
    }
    bzero((char *) &dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    bcopy(
        (char *)server->h_addr,
        (char *)&dest_addr.sin_addr.s_addr,
        server->h_length
    );
    dest_addr.sin_port = htons(portno);
    ssize_t num_bytes = sendto(
        sockfd, buffer, strlen(buffer) + 1, 0,
        (struct sockaddr *)&dest_addr, sizeof(dest_addr)
    );
    if (num_bytes < 0) {
        perror("udpsend -- ERROR sending data");
    }
    close(sockfd);
    push((DCLANG_INT32) num_bytes);
}
