/*Write two separate C programs using UNIX socket APIs illustrate half duplex mode
chat application between a single client and server connection less service in which
the server estimates and prints all permutations of a string sent by the client.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>  // for strlen


int main(){



    int s,ns,r,x;
    char port [30];
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t slen= sizeof (serveraddr);
    printf("Enter port no. \n");
    fgets(port, sizeof(port),stdin);
    x=atoi(port);
    char buffer [50];
    s= socket (AF_INET, SOCK_DGRAM,0);
    if (s==-1){
        perror ("socket not created");
        exit (1);
    }
    printf ("Socket created \n");

    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1"); //explicityly mention ip since udp
    serveraddr.sin_family= AF_INET;
    serveraddr.sin_port=htons(x);

    


    printf ("Enter a string : ");
    fgets (buffer, sizeof(buffer),stdin);
    buffer[strcspn(buffer, "\n")]='\0';
    sendto (s,buffer,strlen (buffer), 0, (struct sockaddr *)& serveraddr, slen);
    printf ("server is writing perumatations....");
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr *)& serveraddr,& slen);
        if (n < 0) {
            perror("Receive failed");
            break;
        }
        
        buffer[n] = '\0';  // Null-terminate the received string
        printf("Server response: %s\n", buffer);

        if (strcmp(buffer, "END") == 0) {
        printf("Server finished sending permutations.\n");
        break;
}
    }

    close(s);
    return 0;

}