#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_MOVIES 5
#define MAX_BOOKINGS 50

typedef struct {
    char name[50];
    char showtime[10];
    int seats;
    int price;
} Movie;

typedef struct {
    int booking_id;
    int client_id;
    char movie_name[50];
    int seats;
    int total_price;
    int active;
} Booking;

typedef struct {
    int client_sock;
    int client_id;
} ClientInfo;

Movie movies[MAX_MOVIES] = {
    {"Movie A", "10:00 AM", 50, 200},
    {"Movie B", "1:00 PM", 40, 250},
    {"Movie C", "4:00 PM", 30, 180},
    {"Movie D", "7:00 PM", 20, 220},
    {"Movie E", "10:00 PM", 25, 300}
};

Booking bookings[MAX_BOOKINGS];
int booking_count = 0;
pthread_mutex_t lock;

void list_movies(int client_sock) {
    char buffer[1024] = "Movies:\n";
    for (int i = 0; i < MAX_MOVIES; i++) {
        char temp[256];
        sprintf(temp, "%d. %s | Showtime: %s | Seats: %d | Price: %d\n", 
                i + 1, movies[i].name, movies[i].showtime, movies[i].seats, movies[i].price);
        strcat(buffer, temp);
    }
    send(client_sock, buffer, strlen(buffer), 0);
}

void book_ticket(int client_sock, int client_id) {
    int movie_index, seats;
    recv(client_sock, &movie_index, sizeof(int), 0);
    recv(client_sock, &seats, sizeof(int), 0);

    pthread_mutex_lock(&lock);
    if (movie_index >= 1 && movie_index <= MAX_MOVIES && movies[movie_index - 1].seats >= seats) {
        movies[movie_index - 1].seats -= seats;
        bookings[booking_count].booking_id = booking_count + 1;
        bookings[booking_count].client_id = client_id;
        strcpy(bookings[booking_count].movie_name, movies[movie_index - 1].name);
        bookings[booking_count].seats = seats;
        bookings[booking_count].total_price = seats * movies[movie_index - 1].price;
        bookings[booking_count].active = 1;
        booking_count++;

        int response[2] = {bookings[booking_count - 1].booking_id, bookings[booking_count - 1].total_price};
        send(client_sock, response, sizeof(response), 0);
    } else {
        int response[2] = {-1, 0};
        send(client_sock, response, sizeof(response), 0);
    }
    pthread_mutex_unlock(&lock);
}

void cancel_reservation(int client_sock) {
    int booking_id;
    recv(client_sock, &booking_id, sizeof(int), 0);

    pthread_mutex_lock(&lock);
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].booking_id == booking_id && bookings[i].active) {
            for (int j = 0; j < MAX_MOVIES; j++) {
                if (strcmp(bookings[i].movie_name, movies[j].name) == 0) {
                    movies[j].seats += bookings[i].seats; //increase seats
                    bookings[i].active = 0;//make inactive---deleting
                    send(client_sock, "Success", 8, 0);
                    pthread_mutex_unlock(&lock);
                    return;
                }
            }
        }
    }
    send(client_sock, "Invalid ID", 10, 0);
    pthread_mutex_unlock(&lock);
}

void transaction_history(int client_sock, int client_id) {
    char buffer[1024] = "Your Transactions:\n";
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].client_id == client_id && bookings[i].active) {
            char temp[256];
            sprintf(temp, "ID: %d | Movie: %s | Seats: %d | Price: %d\n", 
                    bookings[i].booking_id, bookings[i].movie_name, bookings[i].seats, bookings[i].total_price);
            strcat(buffer, temp);
        }
    }
    send(client_sock, buffer, strlen(buffer), 0);
}

void *receive_from_client(void *arg) {
    ClientInfo *client = (ClientInfo *)arg; //points to clientinfo struct
    char choice;

    while (recv(client->client_sock, &choice, sizeof(char), 0) > 0) {
        switch (choice) {
            case '1': list_movies(client->client_sock); break;
            case '2': book_ticket(client->client_sock, client->client_id); break;
            case '3': cancel_reservation(client->client_sock); break;
            case '4': transaction_history(client->client_sock, client->client_id); break;
            case '5': return NULL;
        }
    }
    return NULL;
}

void *send_to_client(void *arg) {
    ClientInfo *client = (ClientInfo *)arg;
    while (1) {
        char message[] = ">> [Server] Still connected. Use menu options to book or view.\n";
        send(client->client_sock, message, strlen(message), 0);
        sleep(10); // Simulate periodic notification
    }
    return NULL;
}

void *handle_client(void *arg) {
    ClientInfo *client = (ClientInfo *)arg;

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, NULL, receive_from_client, arg);
    pthread_create(&send_thread, NULL, send_to_client, arg);

    pthread_join(recv_thread, NULL);
    pthread_cancel(send_thread); // stop push thread when client exits

    close(client->client_sock);
    free(client);
    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread;

    pthread_mutex_init(&lock, NULL);
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_CLIENTS);

    printf("Server started on port %d...\n", PORT);

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len))) {
        ClientInfo *client = malloc(sizeof(ClientInfo));
        client->client_sock = client_sock;
        client->client_id = client_sock; // Use socket descriptor as client ID

        pthread_create(&thread, NULL, handle_client, (void *)client);
        pthread_detach(thread);
    }

    pthread_mutex_destroy(&lock);
    close(server_sock);
    return 0;
}
