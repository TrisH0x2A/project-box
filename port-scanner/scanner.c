#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#define DIE(msg) perror(msg); exit(1);

void com(int src, int dst) {
    char buf[1024 * 4];
    int r, i, j;

    r = read(src, buf, 1024 * 4);

    while (r > 0) {
        i = 0;

        while (i < r) {
            j = write(dst, buf + i, r - i);

            if (j == -1) {
                DIE("write");
            }

            i += j;
        }

        r = read(src, buf, 1024 * 4);
    }

    if (r == -1) {
        DIE("read");
    }

    shutdown(src, SHUT_RD);
    shutdown(dst, SHUT_WR);
    close(src);
    close(dst);
    exit(0);
}

int open_forwarding_socket(char *forward_name, int forward_port) {
    int forward_socket;
    struct hostent *forward;
    struct sockaddr_in forward_address;

    forward = gethostbyname(forward_name);

    if (forward == NULL) {
        DIE("gethostbyname");
    }

    bzero((char *) &forward_address, sizeof(forward_address));
    forward_address.sin_family = AF_INET;
    bcopy((char *)forward->h_addr, (char *) &forward_address.sin_addr.s_addr, forward->h_length);
    forward_address.sin_port = htons(forward_port);

    forward_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (forward_socket == -1) {
        DIE("socket");
    }

    if (connect(forward_socket, (struct sockaddr *) &forward_address, sizeof(forward_address)) == -1) {
        DIE("connect");
    }

    return forward_socket;
}

void forward_traffic(int client_socket, char *forward_name, int forward_port) {
    int forward_socket;
    pid_t down_pid;

    forward_socket = open_forwarding_socket(forward_name, forward_port);
    down_pid = fork();

    if (down_pid == -1) {
        DIE("fork");
    }

    if (down_pid == 0) {
        com(forward_socket, client_socket);
    } else {
        com(client_socket, forward_socket);
    }
}


int open_listening_port(int server_port) {
    struct sockaddr_in server_address;
    int server_socket;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        DIE("socket");
    }

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        DIE("bind");
    }

    if (listen(server_socket, 40) == -1) {
        DIE("listen");
    }

    return server_socket;
}


void accept_connection(int server_socket, char *forward_name, int forward_port) {
    int client_socket;
    pid_t up_pid;

    client_socket = accept(server_socket, NULL, NULL);

    if (client_socket == -1) {
        DIE("accept");
    }

    up_pid = fork();

    if (up_pid == -1) {
        DIE("fork");
    }

    if (up_pid == 0) {
        forward_traffic(client_socket, forward_name, forward_port);
        exit(1);
    }

    close(client_socket);
}


void parse_arguments(int argc, char **argv, int *server_port, char **forward_name, int *forward_port) {
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments\n");
        fprintf(stderr, "Syntax:  %s listen_port forward_host [forward_port]\n", argv[0]);
        exit(1);
    }

    *server_port = atoi(argv[1]);

    if (*server_port < 1) {
        fprintf(stderr, "Listen port is invalid\n");
        exit(1);
    }

    *forward_name = argv[2];
    
    if (argc == 3) {
        *forward_port = *server_port;
    } else {
        *forward_port = atoi(argv[3]);

        if (*forward_port < 1) {
            fprintf(stderr, "Forwarding port is invalid\n");
            exit(1);
        }
    }
}


int main(int argc, char **argv) {
    int server_port, forward_port, server_socket;
    char *forward_name;

    parse_arguments(argc, argv, &server_port, &forward_name, &forward_port);
    signal(SIGCHLD,  SIG_IGN);
    server_socket = open_listening_port(server_port);

    while (1) {
        accept_connection(server_socket, forward_name, forward_port);
    }

    return 0; 
}