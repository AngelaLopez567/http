
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT "8000"
#define MAX_NB_QUEUED_CONNECTIONS 1024
#define BUFFER_SIZE 1000

int main() {

    struct addrinfo hints;
    struct addrinfo *address_info_result, *address_info_result_pointer;
    struct sockaddr peer_address;
    char peer_communication_buffer[1000];
    int address_info_return_code;
    int initial_socket_fd;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC; /* Accept IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM; /* Compatible with TCP */
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE; /* Accept wildcard adresses */

    address_info_return_code = getaddrinfo(NULL, PORT, &hints, &address_info_result);

    if (address_info_return_code != 0) {
        printf("%s", gai_strerror(address_info_return_code));
        exit(EXIT_FAILURE);
    }

    /* iterate over potential adresses and try to create a socket from it then bind */
    for (address_info_result_pointer = address_info_result;
         address_info_result != NULL; address_info_result_pointer = address_info_result->ai_next) {
        initial_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (initial_socket_fd == -1) {
            continue;
        }

        if (bind(initial_socket_fd, address_info_result_pointer->ai_addr, address_info_result_pointer->ai_addrlen) ==
            0) {
            break;
        }

        printf("Unable to create or bind socket");
        exit(EXIT_FAILURE);

    }

    if (listen(initial_socket_fd, MAX_NB_QUEUED_CONNECTIONS) == -1) {
        printf("Failed to listen");
        exit(EXIT_FAILURE);
    }

    while(1){
        socklen_t peer_address_length = sizeof peer_address;
        int peer_fd = accept(initial_socket_fd, &peer_address, &peer_address_length );
        if( peer_fd == -1 ){
            printf("Failed to accept connection \n");
            continue;
        }
        printf("connection occured\n");

       long nb_bytes_read = recv(peer_fd, &peer_communication_buffer, BUFFER_SIZE, 0);
        if(nb_bytes_read == -1){
            printf("An error occured while reading bytes from the peer communication buffer\n");
            continue;
        }
        printf("Read %zd bytes from buffer\n", nb_bytes_read);
    }

}