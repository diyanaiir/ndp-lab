/*Write a single server and multiple client program to illustrate multiple clients
communicating with a concurrent server. The client1 on establishing successful
connection sends ”Institute Of” string to the server along with its socket address.
The client2 on establishing successful connection sends ”Technology” string to the
server along with its socket address. The server opens a text file having the keyword
”Manipal”, append the keywords ”Institute of” and ”Technology” and displays
”Manipal Institute of Technology” along with the socket addresses of the clients . If
the number of clients connected exceeds 2, the server sends ”terminate session” to
all clients and the program terminates.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8889

void communicate_with_server(const char *message)
{
    int sock = 0;
    struct sockaddr_in serv_addr, local_addr;
    char buffer[1024] = {0};
    socklen_t addr_len = sizeof(local_addr);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return;
    }

    // Get and print the local socket address
    if (getsockname(sock, (struct sockaddr *)&local_addr, &addr_len) == 0)
    {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &local_addr.sin_addr, ip, sizeof(ip));
        printf("Client connected from IP: %s, Port: %d\n", ip, ntohs(local_addr.sin_port));
    }

    send(sock, message, strlen(message), 0);
    printf("Message sent: %s\n", message);

    int bytes_received = read(sock, buffer, sizeof(buffer));
    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0';
        printf("Server response: %s\n", buffer);

        if (strcmp(buffer, "terminate session") == 0)
        {
            printf("Session terminated by server.\n");
        }
    }

    close(sock);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <client_number>\n", argv[0]);
        return 1;
    }

    int client_number = atoi(argv[1]);

    if (client_number == 1)
    {
        communicate_with_server("Institute Of");
    }
    else if (client_number == 2)
    {
        communicate_with_server("Technology");
    }
    else
    {
        fprintf(stderr, "Invalid client number. Use 1 or 2.\n");
        return 1;
    }

    return 0;
}
