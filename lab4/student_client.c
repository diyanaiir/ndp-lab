#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

//code works competely except client doesnt print 3rd case result

int main()
{
    int s, r, recb, sntb, x;
    struct sockaddr_in server;
    char buff[200];
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        printf("\nSocket creation error.");
        exit(0);
    }
    printf("\nSocket created.");

    server.sin_family = AF_INET;
    server.sin_port = htons(3383);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    r = connect(s, (struct sockaddr *)&server, sizeof(server));
    if (r == -1)
    {
        printf("\nConnection error.");
        exit(0);
    }
    printf("\nSocket connected.");
    while (1)
    {
        int i;
        char temp[50];
        printf("\n\n");
        int ch;
        int len;
        printf("Enter a choice: \n 1.Reg No \n 2.Name of Student \n 3. Subject code");
        scanf("%d", &ch);
        switch (ch)
        {
        case 1:
            printf("Enter Reg No : ");
            scanf("%s", temp);
            len = strlen(temp);
            memset(buff, 0, sizeof(buff));

            buff[0] = 1;
            buff[1] = len;
            for (int i = 0; i < len; i++)
            {
                buff[i + 2] = temp[i];
            }
            temp[len] = '\0';

            buff[len + 2] = '\0';
            send(s, buff, len + 3, 0);
            memset(buff, 0, sizeof(buff));

            recv(s, buff, sizeof(buff), 0);
            printf("%s", buff);
            break;

        case 2:
            printf("Enter Name : ");
            scanf("%s", temp);
            len = strlen(temp);
            memset(buff, 0, sizeof(buff));

            buff[0] = 2;
            buff[1] = len;
            for (int i = 0; i < len; i++)
            {
                buff[i + 2] = temp[i];
            }
            i++;
            buff[i] = '\0';
            send(s, buff, strlen(buff), 0);
            memset(buff, 0, sizeof(buff));

            recv(s, buff, sizeof(buff), 0);
            printf("%s", buff);
            break;

        case 3:
            printf("Enter Subject : ");
            scanf("%s", temp);
            memset(buff, 0, sizeof(buff));

            len = strlen(temp);
            buff[0] = 3;
            buff[1] = len;
            for (int i = 0; i < len; i++)
            {
                buff[i + 2] = temp[i];
            }

            send(s, buff, len+2, 0);
            memset(buff, 0, sizeof(buff));

            recv(s, buff, sizeof(buff), 0);
            printf("%s", buff);
            break;

        case 4:
            buff[0] = 4;
            send(s, buff, strlen(buff), 0);
            exit(1);

            break;
        default:
            printf("Try again!");
        }
    }
}