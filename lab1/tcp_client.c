#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

int main (){

    int i,x,s,r, num, ele,ch;
    socklen_t clen;
    char buff[50];
    struct sockaddr_in clientaddr ,serveraddr;
    printf ("Enter port no.: ");
    scanf ("%d", &x);

    s= socket (AF_INET, SOCK_STREAM, 0);
    if (s==-1){
        printf ("\n Socket error ");
        exit(0);
    }
    printf ("Socket successful");
     serveraddr.sin_family= AF_INET;
     serveraddr.sin_port= htons(x);
     serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    r= connect (s, (struct sockaddr *)&serveraddr, sizeof (serveraddr));
    if (r==-1){
        printf ("connection error");
        exit(0);
    }
    printf ("Connected!");

    while (1){
        memset(buff,0,sizeof(buff));
        recv (s, buff, sizeof(buff), 0);
        printf ("%s \n", buff);

        scanf ("%d", &num);
        send (s,&num,sizeof (num), 0);
        printf ("Input %d numbers", num);
        for (int i =0;i<num ;i++){
            printf("Enter element %d:   ", i+1);
            scanf ("%d",&ele);
            send (s, &ele, sizeof (ele), 0);
        }

        printf ("Enter: \n 1: sorting \n 2:searching    : ");
        scanf ("%d", &ch);
        send (s, &ch, sizeof(ch), 0);

        if (ch==1){
            for (int i = 0; i < num; i++)
            {	
            	
                recv(s, &ele, sizeof(ele), 0);
                printf("Sorted number: %d\n", ele);
            }
             	memset(buff, 0, sizeof(buff));
    		recv(s, buff, sizeof(buff), 0);
    		printf("%s\n", buff);
        }
        else if (ch==2){
            int key;
            printf ("Enter element to be searched ");
            scanf ("%d", &key);
            send (s, &key, sizeof (key), 0);

        }

    }

     

     close(s);
}