#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message1[BUF_SIZE], message2[BUF_SIZE];
	int str_len, status;
	struct sockaddr_in serv_adr;
	pid_t pid, pidE;
//	fd_set reads, temps;
//	int fd;
//	int re;
//	char buf[BUF_SIZE];
//	struct timeval timeout;

	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	/*
	if(write(sock,message,strlen(message)) < 0){
		perror("write error :");
		exit(1);
	}
	
	FD_ZERO(&read_fds);
	FD_SET(sock,&read_fds);
	FD_SET(0,&read_fds);

	fd = sock;

	while(1){
		tmp_fds = read_fds;
		
		//변화감지
		if(select(fd+1,&tmp_fds,0,0,0)<0){
			perror("select error : ");
			exit(1);
		}

		//클라이언트 소켓에 변화가 있다는 것을 확인
		if(FD_ISSET(sock, &tmp_fds)){
			memset(buf,0,BUF_SIZE);
			re = read(sock, buf,BUF_SIZE);
			if(re<0){
				perror("read error : ");
				exit(1);
			}
			//클라이언트 서버가 접속이 끊겼을때
			else if(re==0){
				exit(1);
			}
			printf("%s\n",buf);
		}

		//입력에 대한 변화가 있다는 것을 확인
		else if(FD_ISSET(0,&tmp_fds)){
			memset(message,0,strlen(message));
			memset(buf,0,BUF_SIZE);

			//보낼 메세지 입력
			fgets(buf,BUF_SIZE,stdin);
			buf[strlen(buf)-1] = '\0';
			sprintf(message,buf);
			if(write(sock,message,strlen(message)<0)){
				perror("write error");
				exit(1);
			}
		}
	}

	*/

	str_len = read(sock, message1, BUF_SIZE-1);
	if( str_len == 0){
		printf("Server does not response.\n");
		exit(0);
	}
	message1[str_len]=0;
	printf("%s", message1);
	
	pid = fork();
	
	if( pid == 0){ // child - read
		
		while(1){
			fgets(message1, BUF_SIZE, stdin);
		
			write(sock, message1, strlen(message1));
		}
	}
	else { // parent - write
		while(1){
			str_len = read(sock, message2, BUF_SIZE-1);
			message2[str_len]=0;
			printf("%s", message2);
			pidE = waitpid(-1, &status, WNOHANG);
			if( pidE == pid) break;
		}
	}
/*
	while(1) 
	{
//		temps = reads;
//		timeout.tv_sec = 5;
//		timeout.tv_usec = 0;

//		re = select(1,&temps,0,0,&timeout);

		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;
		
//		if(FD_ISSET(sock,temps)){
//			str_len = read(sock, message, BUF_SIZE-1);
//			message[str_len] = 0;
//			printf("%s\n",message);
//		}

		
		write(sock, message, strlen(message));
		str_len=read(sock, message, BUF_SIZE-1);
		message[str_len]=0;
		printf("Message from server: %s", message);


	}


*/



	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
