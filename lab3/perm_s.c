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


void swap (char *x, char * y){
    char temp = * x;
    *x= *y;
    *y= temp;

}

void permute ( char * str, int l, int r,int s, struct sockaddr_in clientaddr, socklen_t clen ){
    if (l==r){
        printf ("%s \n",str);
        sendto(s, str, strlen(str), 0, (struct sockaddr *)&clientaddr, clen);
    }
    else{
        for (int i=l;i<=r;i++){
            swap (str+l,str+i);
            permute (str,l+1, r,s,clientaddr,clen);
            swap (str+l,str+i);
        }
    }
}

int main(){

    int s,ns,r,x;
    char port [30];
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t clen= sizeof (clientaddr);
    printf("Enter port no. \n");
    fgets(port, sizeof(port),stdin);
    x=atoi(port);
    char buffer [50];
    s= socket (AF_INET, SOCK_DGRAM,0);
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serveraddr.sin_family= AF_INET;
    serveraddr.sin_port=htons(x);

    r= bind (s,(struct sockaddr *) & serveraddr, sizeof (serveraddr));


    printf ("server is waiting... \n");

    while (1){    
        memset (buffer, 0,sizeof(buffer));
        int n= recvfrom(s,buffer, sizeof (buffer),0, (struct sockaddr *)& clientaddr,& clen);
        buffer[n]='\0';
        printf ("Received string : %s\n", buffer);

        printf ("All permutations of the string :   \n");
        permute(buffer,0,strlen (buffer)-1,s,clientaddr,clen);
        



    }
close (s);
exit (1);
}