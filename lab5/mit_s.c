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
#include <pthread.h>

#define MAX_CLIENTS 2

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t lock;

char part1[100] = "";
char part2[100] = "";
char addr1[INET_ADDRSTRLEN] = "";
char addr2[INET_ADDRSTRLEN] = "";

// Thread function to handle each client
void *handle_client(void *socket_desc)
{
    int sock = *(int *)socket_desc; //casting pointer back to int
    free(socket_desc);

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(sock, (struct sockaddr *)&client_addr, &addr_len); //gets client sock info eg ip
    char *client_ip = inet_ntoa(client_addr.sin_addr);

    char client_message[2000];
    int read_size;

    // Receive client message
    if ((read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        client_message[read_size] = '\0';

        pthread_mutex_lock(&lock);
        if (strlen(part1) == 0)
        {
            strcpy(part1, client_message);
            strcpy(addr1, client_ip);
        }
        else
        {
            strcpy(part2, client_message);
            strcpy(addr2, client_ip);
        }
        pthread_mutex_unlock(&lock);
    }

    if (read_size == 0)
    {
        printf("Client disconnected\n");
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }

    // Wait until both messages are received
    pthread_mutex_lock(&lock);
    if (strlen(part1) > 0 && strlen(part2) > 0)
    {
        FILE *f = fopen("file.txt", "r");
        if (f == NULL)
        {
            perror("Could not open file.txt");
            pthread_mutex_unlock(&lock);
            return 0;
        }

        char base[100];
        fgets(base, sizeof(base), f);
        base[strcspn(base, "\n")] = 0; // Remove newline if present
        fclose(f);

        printf("\nFinal Message:\n%s %s %s\n", base, part1, part2);
        printf("From clients:\n- %s\n- %s\n", addr1, addr2);
    }
    pthread_mutex_unlock(&lock);

    close(sock);

    pthread_mutex_lock(&lock);
    client_count--;
    pthread_mutex_unlock(&lock);

    return 0;
}

int main()
{
    int server_fd, new_socket, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    pthread_mutex_init(&lock, NULL);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8889);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 3);

    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        pthread_mutex_lock(&lock);
        if (client_count >= MAX_CLIENTS)
        {
            // Send terminate message to all clients and shut down
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                send(client_sockets[i], "terminate session", strlen("terminate session"), 0);
                close(client_sockets[i]);
            }
            printf("Maximum clients reached. Terminating session.\n");
            pthread_mutex_unlock(&lock);
            close(new_socket);
            break;
        }

        client_sockets[client_count++] = new_socket;
        pthread_mutex_unlock(&lock);

        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_sock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

        printf("Connection accepted and handler assigned.\n");
    }

    if (new_socket < 0)
    {
        perror("Accept failed");
        return 1;
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
