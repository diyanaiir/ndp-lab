#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

//code works competely except client doesnt print 3rd case result


struct student
{

    char regno[10];
    char name[30];
    char addr[30];
    char dept[20];
    char sem[10];
    char sec[10];
    char sid[3][10];
    char marks[3][10];
};

void add_student(struct student *s, char *regno, char *name, char *addr, char *dept, char *sem, char *sec, char *sub1, char *sub2, char *sub3, char *marks1, char *marks2, char *marks3)
{

    strcpy(s->regno, regno);
    strcpy(s->name, name);
    strcpy(s->addr, addr);
    strcpy(s->dept, dept);
    strcpy(s->sem, sem);
    strcpy(s->sec, sec);
    strcpy(s->sid[0], sub1);
    strcpy(s->sid[1], sub2);
    strcpy(s->sid[2], sub3);
    strcpy(s->marks[0], marks1);
    strcpy(s->marks[1], marks2);
    strcpy(s->marks[2], marks3);
}

int main()
{

    int s, r, recb, sntb, x, ns, a = 0;
    socklen_t len;
    struct sockaddr_in server, client;
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
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    r = bind(s, (struct sockaddr *)&server, sizeof(server));
    if (r == -1)
    {
        printf("\nBinding error.");
        exit(0);
    }
    printf("\nSocket binded.");

    r = listen(s, 1);
    if (r == -1)
    {
        close(s);
        exit(0);
    }
    printf("\nSocket listening.");

    len = sizeof(client);

    ns = accept(s, (struct sockaddr *)&client, &len);
    if (ns == -1)
    {
        close(s);
        exit(0);
    }
    printf("\nSocket accepting.\n");

    struct student stud[3];

    add_student(&stud[0], "220953524", "Diya Nair", "noida", "CCE", "sem6", "A", "NPACN", "DMPA", "EEFM", "100", "100", "100");
    add_student(&stud[1], "220953987", "Harry Potter", "Pune", "CCE", "sem6", "A", "NPACN", "DMPA", "EEFM", "99", "100", "99");
    add_student(&stud[2], "220953529", "Hagrid", "Mumbai", "AIML", "sem6", "B", "NPACN", "DMPA", "EEFM", "50", "70", "90");

    while (1)
    {   memset(buff, 0, sizeof(buff));

        recv(ns, buff, sizeof(buff), 0);
        char temp[50];
        int ch = buff[0];
        printf ("Choice chosen : %d \n", ch);
        int len = buff[1];
        int i;
        for (i = 0; i < len; i++)
        {
            temp[i] = buff[i + 2];
        }
        temp[len] = '\0';  // Correct and cleaner


        if (ch == 4)
        {
            break;
        }
        char pid_str[10];
        pid_t pid = fork();
        if (pid == 0)
        {
            // child
            if (ch == 1)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (strcmp(stud[i].regno, temp) == 0)
                    {

                        strcpy(buff, " ");
                        strcpy(buff, " Students Name : ");
                        strcat(buff, stud[i].name);
                        strcat(buff, "\n Address is :");
                        strcat(buff, stud[i].addr);
                        strcat(buff, "\n PID : ");
                        int PID = getpid();
                        sprintf(pid_str, "%d", PID);
                        strcat(buff, "\n PID is  :");
                        strcat(buff, pid_str);
                        printf ("from db : \n %s", buff);
                        break;
                    }
                }


                send(ns, buff, sizeof(buff), 0);
                exit(0);

            }

            else
            {
                int pid2 = fork();
                if (pid2 == 0)
                {
                    if (ch == 2)
                    {
                        for (int i = 0; i < 3; i++)
                        {
                            if (strcmp(stud[i].name, temp) == 0)
                            {

                                strcpy(buff, " ");
                                strcpy(buff, " Dept : ");
                                strcat(buff, stud[i].dept);
                                strcat(buff, "\n Sem :");
                                strcat(buff, stud[i].sem);
                                strcat(buff, "\n Sec :");
                                strcat(buff, stud[i].sec);
                                strcat(buff, "\n Courses Reg :");
                                for (int j = 0; j < 3; j++)
                                {
                                    strcat(buff, stud[i].sid[j]);
                                    strcat(buff, " ");
                                }
                                strcat(buff, "\n PID : ");
                                int PID = getpid();
                                sprintf(pid_str, "%d", PID);
                                strcat(buff, "\n PID is  :");
                                strcat(buff, pid_str);
                            }
                        }
                    }

                    send(ns, buff, sizeof(buff), 0);
                    exit(0);

                }
                else
                {
                    int pid3 = fork();
                    if (pid3 == 0)
                    {
                        if (ch == 3)
                        {
                                memset(buff, 0, sizeof(buff));

                            for (int i = 0; i < 3; i++)
                            {
                                for (int j = 0; j < 3; j++)
                                {
                                    if (strcmp(stud[i].sid[j], temp) == 0)
                                    {

                                        strcpy(buff, " Marks are:  \n");
                                        strcat(buff, stud[i].name);
                                        strcat(buff, ": ");
                                        strcat(buff, stud[i].marks[j]);

                                        int PID = getpid();
                                        sprintf(pid_str, "%d", PID);
                                        strcat(buff, "\n PID is  :");
                                        strcat(buff, pid_str);
                                        printf ("db says : \n %s", buff );
                                    }
                                }
                            }
                        }


                        send(ns, buff, strlen(buff), 0);
                        exit(0);

                    }
                    else if (pid3>0){
                        wait(NULL); // for parent
                    }
                }
            }

        }
    }
}
