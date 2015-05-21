/****************************************************************************** 
* Author : Deepak Luitel
* TCP Echo Server
* Computer Networks
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

#define MAX_BUFFER 256
#define SUCCESS 1
#define ERROR -1

void serverErr(char *messages){
	perror(messages);
	exit(ERROR);
}
void clientError(char *add, int port){
	printf("**Error** from %s:%d\n",add,port);
	fflush(stdout);	
}
int compare(char *one, char *two){
	if(strcmp(one,two)==0)
		return 1;
return 0;
}

void serveClient(int cfd, struct sockaddr_in *clientaddr){

	char client_msg[MAX_BUFFER];
	int c1,c2,c3,c4,cookie,prev = 0;
	char magic[20], type[20], id[245], name[245], err[245];
	
	char *claddr = inet_ntoa(clientaddr->sin_addr);
	int clport = clientaddr->sin_port;

	memset(client_msg,0,MAX_BUFFER);
	
	while(recv(cfd, client_msg, MAX_BUFFER, 0)){		

		if(strlen(client_msg) > MAX_STR_SIZE){
			clientError(claddr,clport);
			break;
		}	
		int t = 0, c = 0;
		memset(err, 0, 1);
		memset(name, 0, 1);
		memset(magic,0, 1);
		sscanf(client_msg,"%s%s%s%s%s",magic,type,id,name,err);
	
		if(!compare(magic,MAGIC_STRING))
			t = 0;
		else if(prev == 0 && compare(err,""))
			t = compare(type,"HELLO");
		
		else if(prev == 1 && compare(name,"")){
			c = atoi(id);
			if (cookie == c)
				t = compare(type,"CLIENT_BYE");
			}
		int stat;
		stat = t;
		if(prev == 0 && stat){
			prev = 1;
			sscanf(claddr,"%d.%d.%d.%d",&c1,&c2,&c3,&c4);
			cookie = ((c1+c2+c3+c4)*13)%1111;
			printf("%d %s %s from %s:%d\n",cookie,id,name,claddr,clport);
			fflush(stdout);
			memset(client_msg,0,MAX_BUFFER); /*reset buffer for writing*/
			sprintf(client_msg,"%s STATUS %d %s:%d\n",MAGIC_STRING,cookie,claddr,clport);
			write(cfd, client_msg,strlen(client_msg));
			memset(client_msg,0,MAX_BUFFER); /*reset buffer for client_bye*/
			}
		else if(prev ==1 && stat){
			memset(client_msg,0,MAX_BUFFER); /*reset buffer for writing*/
			sprintf(client_msg,"%s SERVER_BYE\n",MAGIC_STRING);
			write(cfd, client_msg,strlen(client_msg));
			break;
			}
		else{
			clientError(claddr,clport);
			break;
		}
	}
	close(cfd);
}

int main(int argc, char *argv[]){
	struct sockaddr_in serverAddr, clientAddr;	/*server and client addr*/
	int serverport; 				/*server port*/
	int serverfd,clientfd;
	unsigned int addrLength;
	
	if (argc>2){
		fprintf(stderr,"Usage: %s <server port>\n",argv[0]);
		exit(SUCCESS);
	}
	if(argv[1] == NULL)
		serverport = SERVER_PORT;
	else serverport = atoi(argv[1]);

	serverfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(serverfd < 0){
		serverErr("Socket() creation failed!");
	}
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(serverport);
	
	if(bind(serverfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		serverErr("Failed to bind()");
	if(listen(serverfd, MAXPENDING) < 0)
		serverErr("Failed to listen()");

	while(1){
		addrLength = sizeof(clientAddr);
		if((clientfd = accept(serverfd, (struct sockaddr *)&clientAddr,&addrLength)) < 0)
			serverErr("Failed to accept()");

		serveClient(clientfd, (struct sockaddr_in *) &clientAddr);
	}
//return 0; no exit => server runs forever
}
