#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<netinet/in.h>
#include <sys/socket.h>
#include <ctype.h> 

int is_palindrom (char * word){
    int n = strlen(word);
    for (int i =0;i<(n/2);i++){
        if (word[i]!=word[n-i-1]){
            return 0;
        }
    }
    return 1;
}

void count_vowels(char *word, int * a, int * e, int * i, int * o, int *u){
    int n = strlen(word);
    for (int j=0;j<n;j++){
        char c = tolower(word[j]);
        if (c=='a') (*a)++;
        else if (c=='e') (*e)++;
        else if (c=='i') (*i)++;
        else if (c=='o') (*o)++;
        else if (c=='u') (*u)++;
    }
}

int main() {
    int sockfd,x;
    socklen_t clen;
    int a=0, e=0,i=0,o=0,u=0;
    char buff[50];
    struct sockaddr_in serveraddr, clientaddr;
    printf ("Enter port no: ");
    scanf ("%d",&x);

    sockfd = socket(AF_INET, SOCK_DGRAM,0);
    if (sockfd==-1){
        printf ("socket not created");
        exit (1);
    }

    serveraddr.sin_family= AF_INET;
    serveraddr.sin_port= htons(x);
    serveraddr.sin_addr.s_addr= htonl(INADDR_ANY);
    int r= bind (sockfd, (struct sockaddr *)& serveraddr,sizeof(serveraddr));
    if (r==-1){
        perror ("failed to bind");
        close (sockfd);
        exit (1);

    }
    clen= sizeof(clientaddr);
    printf ("socket binded, SERVER RUNNING");
    while(1){
            int n=recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)& clientaddr, &clen);
            if (n==-1){
                perror("couldnt receive");
                close(sockfd);
                exit(1);
            }
            buff[n]='\0';

            if (strcmp (buff, "halt")==0){
                printf ("closing");
                close (sockfd);
                exit (1);
            }
            int a=0, e=0,i=0,o=0,u=0;
            int pali_check=is_palindrom(buff);
            count_vowels(buff,&a,&e,&i,&o,&u);

            char result [512];
            snprintf(result, sizeof(result),"Palindrome? %s\n word: %s \n a: %d \n e: %d \n i: %d \n o: %d \n u: %d \n",pali_check?"yes":"no",buff,a,e,i,o,u );
            sendto (sockfd,result, sizeof (result), 0 , (struct sockaddr *)& clientaddr, clen);

    }
    close (sockfd);
    return 1;
    


}