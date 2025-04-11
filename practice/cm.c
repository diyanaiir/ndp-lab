#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082

int sock;

void print_menu() {
    printf("\nMovie Booking System\n");
    printf("1. List Movies\n");
    printf("2. Book Ticket\n");
    printf("3. Cancel Reservation\n");
    printf("4. Transaction History\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
    fflush(stdout);
}

int main() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    char choice;
    while (1) {
        print_menu();
        scanf(" %c", &choice);
        send(sock, &choice, sizeof(char), 0);

        if (choice == '1' || choice == '4') {
            // These choices will be handled by the server and received in the receive_handler
            sleep(1); // Allow time for the server to respond
            char buffer[1024];
            int bytes = recv(sock, buffer, sizeof(buffer), 0);
            if (bytes <= 0) {
                // Server disconnected or error in receiving data
                printf("\nServer disconnected.\n");
                close(sock);
                exit(0);
            }
            printf("\n[Server]: %s\n", buffer);
        } else if (choice == '2') {
            int movie_index, seats;
            printf("Enter movie number: ");
            scanf("%d", &movie_index);
            printf("Enter number of seats: ");
            scanf("%d", &seats);

            send(sock, &movie_index, sizeof(int), 0);
            send(sock, &seats, sizeof(int), 0);

            int response[2];
            recv(sock, response, sizeof(response), 0);

            if (response[0] != -1)
                printf("Booking successful! ID: %d, Total: %d\n", response[0], response[1]);
            else
                printf("Booking failed!\n");
        } else if (choice == '3') {
            int booking_id;
            printf("Enter booking ID to cancel: ");
            scanf("%d", &booking_id);

            send(sock, &booking_id, sizeof(int), 0);

            char buffer[50] = {0};
            recv(sock, buffer, sizeof(buffer), 0);

            printf("%s\n", buffer);
        } else if (choice == '5') {
            send(sock, &choice, sizeof(char), 0); // Optional: send exit request to server
            close(sock);
            break;
        }
    }

    return 0;
}
