#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<netinet/in.h>
#include <sys/socket.h>

int main() {
    int sockfd,x;
    socklen_t slen;
    char port[30];
    int a=0, e=0,i=0,o=0,u=0;
    char buff [50];
    struct sockaddr_in serveraddr, clientaddr;
    printf ("Enter port no: ");
    fgets(port,sizeof(port),stdin);
    x = atoi(port);  // convert string to integer


    sockfd = socket(AF_INET, SOCK_DGRAM,0);
    if (sockfd==-1){
        printf ("socket not created");
        exit (1);
    }
    printf ("socket created");

     serveraddr.sin_family= AF_INET;
    serveraddr.sin_port= htons(x);
    serveraddr.sin_addr.s_addr= htonl(INADDR_ANY);

    slen = sizeof (serveraddr);
    printf ("socket binded");
    while (1){
        
        printf ("\n Enter a string to check pali or not :\n");
        fgets (buff,sizeof (buff),stdin);
        buff[strcspn(buff, "\n")]='\0';
        sendto (sockfd,buff, sizeof(buff), 0,(struct sockaddr *)& serveraddr, slen);

        if (strcmp(buff, "halt")==0){
            break;
        }
        char result [512];
        int n =recvfrom(sockfd, result,sizeof (result), 0, (struct sockaddr *)& serveraddr,& slen);
        result[n]='\0';
        printf ("Server response: %s",result);


    }
    
    
close (sockfd);
return 0;


}