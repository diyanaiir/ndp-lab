/*Write two separate C programs using UNIX socket APIs illustrate full duplex mode
chat application between a single client and server using connection oriented service.
Display PID and PPID of both parent and child processes.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

int main (){
 int s,ns,r,x;
 socklen_t clen;
    char port [30];
    struct sockaddr_in serveraddr, clientaddr;
    printf("Enter port no. \n");
    fgets(port, sizeof(port),stdin);
    x=atoi(port);
    

    s= socket (AF_INET,SOCK_STREAM, 0);
    if (s==-1){
        perror ("socket not created");
        exit (1);
    }
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(x);
    serveraddr.sin_addr.s_addr= htonl(INADDR_ANY);

    r= bind (s,(struct sockaddr *)& serveraddr, sizeof(serveraddr));
    if (r==-1){
        perror ("binding error");
        close (s);
        exit (1);
    }
    r= listen(s, 1);
    clen= sizeof(clientaddr);
    ns= accept (s, (struct sockaddr *)&clientaddr, &clen);
    char buff [500];
    pid_t pid = fork();
    
    printf ("Connected to client \n");

    if (pid==0){
        //child process
        
        while (1)
        {//client->server
        memset (buff,0 ,sizeof(buff));
        int n= recv(ns, buff, sizeof(buff),0);
        printf ("Client said : %s\n",buff );

        //server->client
        printf ("Server: ");
        fgets(buff, sizeof(buff), stdin);
        send (ns, buff, sizeof(buff),0);
        }
        close (ns);
        exit (1);

    }
    else if (pid>0){
        wait (NULL); //WAIT FOR CHILD TO FINISH
    }

    else if (pid<0){
        perror ("Fork failed");
        close (s);
        exit (1);

    }


    close (s);
    exit (1);

}