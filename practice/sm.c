#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8082
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
                    bookings[i].active = 0; // make inactive
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
    char buffer[1024] = {0};
    strcat(buffer, "Your Transactions:\n");
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

void handle_client(int client_sock) {
    int client_id = client_sock; // Using socket descriptor as client ID
    char choice;

    while (recv(client_sock, &choice, sizeof(char), 0) > 0) {
        switch (choice) {
            case '1': list_movies(client_sock); break;
            case '2': book_ticket(client_sock, client_id); break;
            case '3': cancel_reservation(client_sock); break;
            case '4': transaction_history(client_sock, client_id); break;
            case '5': close(client_sock); return;
        }
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    pthread_mutex_init(&lock, NULL);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_CLIENTS);

    printf("Server started on port %d...\n", PORT);

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len))) {
        handle_client(client_sock); // Handle the client sequentially
    }

    pthread_mutex_destroy(&lock);
    close(server_sock);
    return 0;
}
