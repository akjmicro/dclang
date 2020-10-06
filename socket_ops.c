#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


struct sockaddr_in serv_addr, cli_addr, host_addr;


static void tcplistenfunc()
{
    // Sets up a new listening TCP socket 'object' for listening
    // and returns its address onto the stack
    if (data_stack_ptr < 1) {
        printf("tcplisten -- need <port_number> on the\n");
        return;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int true = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
    if (sockfd < 0) {
       perror("tcplisten -- ERROR opening socket.");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    int portno = (int) pop();
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0)
          perror("ERROR on binding");
    listen(sockfd, 5);
    push((int) sockfd);
}


static void tcpacceptfunc()
{
    // Take a given tcplisten-ready socket object and actually make it
    // accept a connection. Returns a handle to the established connection.
    // Can be used inside a loop as a basis for a "multi-connection handling"
    // forking server via `fork`.
    if (data_stack_ptr < 1) {
        printf("tcpaccept -- need <socket> on the stack");
        return;
    }
    int clilen = sizeof(cli_addr);
    int sockfd = (int) pop();
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) perror("tcpaccept -- ERROR on accept step!");
    push((int) newsockfd);
}


static void tcpconnectfunc()
{
    // A tcp client word. This can reach out to a pre-established tcp server
    // already set up with the above words.
    if (data_stack_ptr < 2) {
        printf("tcpconnect -- need <host> <port> <requestbuf> <responsebuf> on the stack");
        return;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) perror("tcpconnect -- ERROR opening socket");
    int portno = (int) pop();
    char *servername = (char *) (MYUINT) pop();
    struct hostent *server = gethostbyname(servername);
    if (server == NULL) {
        fprintf(stderr,"tcpconnect -- ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &host_addr, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&host_addr.sin_addr.s_addr, server->h_length);
    host_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0)
        perror("tcpconnect -- ERROR connecting");
    push((int) sockfd);
}
