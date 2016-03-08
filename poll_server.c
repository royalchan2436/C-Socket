#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "lists.h"

#ifndef PORT
  #define PORT 55651
#endif
#define DELIM " \n"

void addclient(int fd);
void removeclient(int fd);
char* format(char *input, Poll **head);



struct client{
	int fd;
	char name[20];
	struct client *next;
}*first = NULL;

/* Most from Sample Code */

void addclient(int fd){
	struct client *p = malloc(sizeof(struct client));
	if (!p) {
		printf("malloc fail!\n");
		exit(1);
	}
    p->fd = fd;
    write(p->fd, "what is your name\n", sizeof("what is your name\n"));
    read(p->fd, &p->name, sizeof(p->name));
    write(p->fd, "You can type your command now\n", sizeof("You can type your command now\n"));
    p->next = first;
    first = p;
}

/* From Sample Code */
void removeclient(int fd){
	struct client **p;
	for(p = &first; *p && (*p)->fd != fd; p = &(*p)->next)
	  ;
    if(*p){
    	close((*p)->fd);
    	struct client *t = (*p)->next;
    	free(*p);
    	*p = t;
    }
}

int main(void){
	int listenfd, soc, maxfd, nbytes;
	struct sockaddr_in addr, clientaddr;
	char buffer[256];
	fd_set read_set, ready_set;
    socklen_t addrlen = sizeof(clientaddr);
    char *hold;
    int no = 1;
    struct client *p;
    Poll *poll_list = NULL;

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0){
		perror ("socket fail\n");
		exit(1);
	}
	if((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &no, sizeof(int))) == -1) {
        perror("setsockopt fail\n");
    }
	bzero ( &addr, sizeof(addr) );
	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr=htonl(INADDR_ANY) ;
	
	if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr))<0){
		perror("connect fail\n");
		exit(1);
	}

	if (listen(listenfd, 5)) {
		perror("listen fail\n");
		exit(1);
    }

    while(1){
    	maxfd = listenfd;
    	FD_ZERO(&read_set);
    	FD_SET(listenfd, &read_set);
    	for (p = first; p; p = p->next) {
			FD_SET(p->fd, &read_set);
			if (p->fd > maxfd)
				maxfd = p->fd;
		}
		ready_set = read_set;
		if(select(maxfd+1,&ready_set,NULL,NULL,NULL)<0){
				perror("select fail\n");
		}else{
			if(FD_ISSET(listenfd, &ready_set)){
				soc = accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);
				addclient(soc);
			}else{
				for(p = first; p; p = p->next){
					if(FD_ISSET(p->fd, &ready_set)){
						nbytes = read(p->fd, buffer, sizeof(buffer) - 1);
						if(nbytes > 0){
							buffer[nbytes] = '\0';
							if(strncmp(buffer, "quit", 4) == 0){
								removeclient(p->fd);
							}else{
								hold = format(buffer, &poll_list);
							    if(strncmp(hold, "sus", 3) != 0){
                                    write(p->fd, hold, strlen(hold));
							        free(hold); 
							    }
							}
							
						}  
					}
				}
		    }
		}
    }
}




char* format(char *input, Poll **head){
     
     int cmd_argc;
     char *cmd_argv[12];
     char *next_token = strtok(input, DELIM);
     cmd_argc = 0;
     while (next_token != NULL) {
        if (cmd_argc >= 11) {
        	char *ans = malloc(20);
        	strcpy(ans, "Too many arguments!");
            return ans;
        }
        cmd_argv[cmd_argc] = next_token;
        cmd_argc++;
        next_token = strtok(NULL, DELIM);
     }
     char *temp;
     temp = process_args(cmd_argc, cmd_argv, head);
     return temp;
}