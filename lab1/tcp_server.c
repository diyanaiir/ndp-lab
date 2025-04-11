#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
int main (){

    int i,x,s,r,ns, num,ch;
    socklen_t clen;
    struct sockaddr_in clientaddr ,serveraddr;
    int arr[50];
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

    r= bind (s,(struct sockaddr *)& serveraddr, sizeof(serveraddr));
    if (r==-1){
        printf ("binding error");
        exit(0);
    }
    r=listen (s,0);
     if (r==-1){
        close(s);
        exit(0);
    }
    clen = sizeof (clientaddr);
    ns = accept (s, (struct sockaddr *)& clientaddr,& clen);
     if (ns==-1){
       close (s);
        exit(0);
    }
    printf ("Socket accepted");

    while (1){
        char msg[]= "Send number of elements";
        send (ns , msg, sizeof(msg),0);
       
       //receiving num
       recv (ns, &num, sizeof (num), 0);
       printf ("Received number of elements are %d", num);

        //accepting elemetns
        for (int i =0;i<num;i++){
            recv(ns, &arr[i],sizeof (arr[i]), 0);
            printf("Received number %d", arr[i]);
        }


        //receiving choice

        recv (ns , &ch, sizeof (ch), 0);
        printf ("choice selected is :%d", ch);
        if (ch==1){


            //bubble sort 
            for (int i=0;i<num-1;i++){
                for (int j=i+1;j<num;j++){
                    if (arr[j]<arr[i]){
                        int temp = arr[j];
                        arr[j]=arr[i];
                        arr[i]=temp;
                    }
                }
            }

            //send sorted to client
            for (int i =0;i<num;i++){
                send (ns, &arr[i], sizeof(int),0);
                printf ("Sent sorted elemet %d", arr[i]);
            }
        }

        else if (ch==2){
            int key;
            int flag=0;
            recv (ns, &key, sizeof (int),0);

            for (int i=0;i<num;i++){
                if (arr[i]==key) {
                    flag=1;
                    break;
                }
            }
            if (flag==1){
                send (ns, "number found", sizeof ("number found"), 0);
            }
            else if (flag==0){
                send (ns, "number not found", sizeof ("number not found"), 0);
            }
        }
        else {

            send (ns, "invalid input", sizeof ("invalid input"), 0);
        }
    }
    close (ns);
    close (s);
    return 0;
}