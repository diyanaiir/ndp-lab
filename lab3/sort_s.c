/*Write two separate C programs (one for server and other for client) using socket
APIs, to implement the following connection-oriented client-server model.
(a) The user at the client side sends an alphanumeric string to the server.
(b) The child process at the server sorts the numbers of the alphanumeric string
in ascending order. The parent process at the server sorts the characters of the
alphanumeric string in descending order.
(c) Both the processes send the results to the client along with its corresponding
process ID.*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sort_numbers_asc (const void *a, const void * b){
    return (*(int *)a- *(int *)b);
}

int sort_chars_desc(const void *a, const void * b){
    return (*(int *)b- *(int *)a);
}
void seperate (char * input, char * chars, int * numbers, int * num_count){
    *num_count=0;
    int char_index=0;
    for (int i=0;input[i]!='\0';i++){
        if (isdigit(input[i])){
            numbers[*num_count]=input[i]-'0';
            (*num_count)++;
        }
        else 
        {chars[char_index]=input[i];
            (char_index)++;
        }
        chars[char_index]='\0';
    }
}

int main (){
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];


    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connections...\n");

    // Accept client connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    char chars[20];
    int numbers[20];
    int num_count;
    memset (buffer, 0 , sizeof (buffer));
    //receive string 

    recv (client_fd, buffer, sizeof (buffer),0);
    printf ("Received string is : %s \n", buffer);
    seperate(buffer,chars, numbers, &num_count);
    pid_t pid= fork();
    if (pid==0){
        

        qsort(numbers, num_count, sizeof (int),sort_numbers_asc);
        printf ("Child PID: %d \n", getpid());
        printf ("Sorted numbers are : ");
        for (int i=0;i<num_count;i++){
            printf ("%d", numbers[i]);
        }
        send(client_fd, "Sorted numbers: ", 16, 0);
        for (int i = 0; i < num_count; i++) {
            char num_str[2];
            sprintf(num_str, "%d", numbers[i]);
            send(client_fd, num_str, strlen(num_str), 0);
        }
        
        exit (1);

    }

    else if (pid>0){
        //parent process
        qsort(chars,strlen (chars),sizeof (char), sort_chars_desc);
        printf("Parent PID: %d\n", getpid());
        printf("Sorted characters (descending order): %s\n", chars);
        send (client_fd, "sorted chars:  ", sizeof ("sorted chars:  "), 0);
        send (client_fd, chars, strlen(chars), 0);
        wait(NULL);
        close(client_fd);
    }

    else {
        perror("fork failed");
        close (client_fd);
        close (server_fd);
    exit (1);

    }
    close (server_fd);
    exit (1);

}