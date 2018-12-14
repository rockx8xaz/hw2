#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
char username[100];
void *recvmg(void *sock)
{
	int their_sock = *((int *)sock);
	char msg[500];
	int len;
	while((len = recv(their_sock,msg,500,0)) > 0) 
	{
		char *a,*b;
		char sname[100];
		msg[len] = '\0';
		if(a=strstr(msg,"%%"))
		{
			memset(sname,'\0',sizeof(sname));
			b=strstr(a+2,"%%");
			strncpy(sname,a+2,(b-a)-2);
			if(strcmp(username,sname)==0)
			{
				char newmsg[500],fromwho[100];
				memset(newmsg,'\0',sizeof(newmsg));
				memset(fromwho,'\0',sizeof(fromwho));
				strcpy(newmsg,"(serect from)");
				strncpy(fromwho,msg,a-msg);
				strcat(newmsg,fromwho);
				strcat(newmsg,b+2);
				fputs(newmsg,stdout);
				memset(msg,'\0',sizeof(msg));
				memset(newmsg,'\0',sizeof(newmsg));
			}	
		}
		else
		{
			fputs(msg,stdout);
		}
		memset(msg,'\0',sizeof(msg));
	}
}
int main(int argc, char *argv[])
{
	struct sockaddr_in their_addr;
	int my_sock;
	int their_sock;
	int their_addr_size;
	int portno;
	pthread_t sendt,recvt;
	char msg[500];
	char tmpmsg[500];
	char res[600];
	char ip[INET_ADDRSTRLEN];
	char towho[100];
	char *nameindex,*msgindex;
	int len;
	memset(username,'\0',sizeof(username));
	if(argc > 3) 
	{
		printf("too many arguments");
		exit(1);
	}
	portno = atoi(argv[2]);
	strcpy(username,argv[1]);
	my_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(their_addr.sin_zero,'\0',sizeof(their_addr.sin_zero));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(portno);
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(my_sock,(struct sockaddr *)&their_addr,sizeof(their_addr)) < 0) 
	{
		perror("connection not esatablished");
		exit(1);
	}
	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
	printf("connected to %s, start chatting\n",ip);
	send(my_sock,username,(strlen(username)),0);
	pthread_create(&recvt,NULL,recvmg,&my_sock);
	while(fgets(msg,500,stdin) > 0) 
	{
		strcpy(res,username);
		strcat(res,":");
		if(nameindex=strstr(msg,"to:"))
		{
			memset(towho,'\0',sizeof(towho));
			memset(tmpmsg,'\0',sizeof(tmpmsg));
			nameindex=nameindex+3;
			msgindex=strstr(msg," ");
			strncpy(towho,nameindex,msgindex-nameindex);
			strcat(towho,"\0");
			strcpy(msg,msgindex+1);
			strcpy(tmpmsg,"%%");
			strcat(tmpmsg,towho);
			strcat(tmpmsg,"%%");
			strcat(tmpmsg,msg);
			strcpy(msg,tmpmsg);
			strcat(res,msg);
		}	
		else if(strcmp(msg,"username\n")==0)
		{
			strcat(res,"%%username");
		}	
		else
			strcat(res,msg);
		len = write(my_sock,res,strlen(res));
		
		memset(msg,'\0',sizeof(msg));
		memset(res,'\0',sizeof(res));
	}
	pthread_join(recvt,NULL);
	close(my_sock);

}
