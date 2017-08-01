#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char *buf);
int who_win(int msg1, int msg2);

int clnt_num = 0; // 클라이언트 수
int clnt[10] = { 0 };

struct login_info
{
	char id[50];
	char pw[50];
	int islogin;
	int logined_descripter;
	int ban[10];

}list[10];

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	//클라이언트에게 전송할 메세지 버퍼
	char message[100]; 
	char message2[100];
	char message3[100];
	char message4[100];
	char message5[100];
	char message6[100];

	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i, j;
	char buf[BUF_SIZE];
	char temp[BUF_SIZE];

	int clnt_id;
	int id_flag = 0;
	int login_flag = 0;
	int wisper_flag = 0;
	int game_flag = 0;

	int ban=0;
	int result = 0;
	int win_cnt[2];
	int num = 0;
	int clnt_cnt = 0;
	int Gclnt_sock[2] = {0,0};
	int clnt_msg[2];
	int clnt_flag;
	double rate1 = 0.0;
	double rate2 = 0.0;

	char *token = NULL;
	char str[] = " ,\n";
	char temp2[BUF_SIZE];
	char temp3[BUF_SIZE];
	char temp4[BUF_SIZE];
	char temp5[BUF_SIZE];
	char temp6[BUF_SIZE];
	
	for(i=0;i<10;i++) {
		for(j=0;j<10;j++)
			list[i].ban[j]=0;
	}

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	for(i=0; i<10; i++)
		list[i].islogin = 0;

	while (1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		memset(buf, '\0', BUF_SIZE);

		if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;

		if (fd_num == 0)
			continue;

		for (i = 0; i<fd_max + 1; i++)
		{
			//memset(buf, '\0', BUF_SIZE);

			if (FD_ISSET(i, &cpy_reads))
			{
				if (i == serv_sock)     // connection request!
				{
					adr_sz = sizeof(clnt_adr);
					clnt_sock =
						accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if (fd_max<clnt_sock)
						fd_max = clnt_sock;

					for (j = 4; j <= 8; j++) //디스크립터 4부터 클라이언트 소켓이 들어오기 때문에
					{
						if (clnt[j] == 0) { //빈 곳 중에 가장 작은 곳에 클라이언트를 넣기 위해서, j번째 클라이언트가 접속하면 값을 1로 바꿈
							clnt_id = j;
							clnt[j] = 1;
							break;
						}
					}

					clnt_num++;
					printf("New client is connected - Number of total client is %d \n", clnt_num);

					sprintf(message, "Connection Success, Your ID is User%d\n", clnt_id - 3);
					write(clnt_sock, message, strlen(message));

				}
				else    // read message!
				{
				
				//	str_len = read(i, buf, BUF_SIZE);
					if(game_flag ==0)
						str_len = read(i,buf, BUF_SIZE);
					else if(game_flag ==1)
						read(i,buf,2);

					if (str_len == 0)    // close request!
					{   //클라이언트가 나가면  clnt_num--;
						FD_CLR(i, &reads);
						close(i);
						clnt[i] = 0;
						clnt_num--;
						printf("User%d is disconnected - Number of total client is %d \n", i - 3, clnt_num);
						list[i].islogin = 0;
					}

					else 
					{
						strcpy(temp, buf);
						token = strtok(temp, str);
						
						if( strcmp(token, "/game") == 0){
						//	printf("Game will be made...\n");
							
							if( Gclnt_sock[0] == 0){
								Gclnt_sock[0] = i;
								win_cnt[0] = 0;
								clnt_msg[0] = 0;
							}
							else if( Gclnt_sock[1] == 0){
								Gclnt_sock[1] = i;
								win_cnt[1] = 0;
								clnt_msg[1] = 0;

								printf("New game start\n"); game_flag = 1;
								write( Gclnt_sock[0], "Input your decision\n", sizeof("Input your decision\n"));
								write( Gclnt_sock[1], "Input your decision\n", sizeof("Input your decision\n"));
							}
							else {
								write(i, "You cannot play the game.\n", strlen("You cannot play the game.\n"));
							}
						}
						else if(strcmp(token, "/register") == 0) //첫 토큰이 '/register' 이면 회원가입과정 실행
						{
							token = strtok(NULL, str);
							strcpy(temp2, token); //id토큰

							token = strtok(NULL, str);
							strcpy(temp3, token); //pw토큰
							temp3[strlen(temp3)-1] = '\0';
							for (j = 4; j <= 8; j++)
							{
								if (strcmp(list[j].id, temp2) == 0) //저장된 id_list에 회원가입하려는 아이디가 존재하면
								{
									sprintf(message2, "Incorect registration\n");
									printf("Incorrect registration\n");
									write(i, message2, strlen(message2));
									id_flag = 1; //회원 가입에 실패하면 flag 1, 성공했으면 0이 될것
									break;
								}
							}

							if (id_flag == 0) // 회원가입 성공 시에
							{
								strcpy(list[i].id, temp2);
								strcpy(list[i].pw, temp3);
								sprintf(message3, "Registration success\n");
								printf("%s is registered\n", list[i].id);
								write(i, message3, strlen(message3));
							}
							id_flag = 0;
							//fflush(stdin);
						}

						else if (strcmp(token, "/login") == 0) //첫 토큰이 '/login' 이면 회원가입과정 실행
						{
							token = strtok(NULL, str);
							strcpy(temp2, token); //id토큰

							token = strtok(NULL, str);
							strcpy(temp3, token); //pw토큰
							temp3[strlen(temp3)-1] = '\0';

							for (j = 4; j <= 8; j++)
							{
								if (strcmp(list[j].id, temp2) == 0 && strcmp(list[j].pw, temp3) == 0 && list[j].islogin == 0 ) //목록중에 id/pw일치하는게 있으면
								{
									sprintf(message2, "Login Success\n");
									write(i, message2, strlen(message2));
									login_flag = 1; // 성공시에 flag 1, 실패시에 flag 0
									list[j].islogin = 1;
									list[j].logined_descripter = i;
								}
							}

							if (login_flag == 0) {
								sprintf(message3, "Login fail\n");
								write(i, message3, strlen(message3));
							}

							login_flag = 0;
							//fflush(stdin);

						}

						else if(strcmp(token, "/w")==0) // 첫 토큰이 '/w'이면 귓속말 실행
						{
							token = strtok(NULL, str);
							strcpy(temp4, token); //id 토큰

							 token = strtok(NULL, str);
							// token = temp4;
						//	token = temp + strlen(temp4) - 1;
							strcpy(message4, token); //메세지 토큰
							//message4[strlen(message4)] = '\0';
							for(j=4; j<=8; j++)
							{
								if(strcmp(list[j].id, temp4)==0 && list[j].islogin == 1)
								{
									wisper_flag = 1;

									if(list[i].ban[j] > 0)
										continue;

								
									sprintf(message5, "%s) %s\n",list[i].id, message4);
									write(list[j].logined_descripter, message5, strlen(message5));
									
								}
							}

							if(wisper_flag ==0){
								sprintf(message4,"Wisper fail, no user\n");
								write(i, message4, strlen(message4));
							}

							wisper_flag = 0;
						}

						else if(strcmp(token, "/b") ==0) //첫 토큰이 '/b'이면 특정 유저 차단 실행
						{
							token = strtok(NULL, str);
							strcpy(temp5, token); //차단 id 토큰

							for(j=4; j<=8; j++)
							{
								if(strcmp(list[j].id, temp5)==0){
									printf("ban : %s will ban %s\n", list[i].id, list[j].id);
									list[j].ban[i] = 1;//j가 i에게 보내지 못하는것!!!!!
									// list[i].ban = list[j].logined_descripter; //int ban
								}
							}
						}

						else {
							//printf("echo!\n");
							//printf("read message: %s\n", buf);
							//buf[str_len-1] = '\0';
							if( i == Gclnt_sock[0] || i == Gclnt_sock[1] && game_flag == 1){
								//read(i, buf, 2);
								if( strcmp(token, "/p") == 0){
									rate1 = win_cnt[0] * 100.0 / (double)num;
									rate2 = win_cnt[1] * 100.0 / (double)num;
									printf("%.2lf %.2lf\n", rate1, rate2);
									write( Gclnt_sock[0], "Game finished\n", sizeof("Game finished\n"));
									write( Gclnt_sock[1], "Game finished\n", sizeof("Game finished\n"));
									read(i,buf,BUF_SIZE);//
									Gclnt_sock[0] = 0; Gclnt_sock[1] = 0; num = 0;
									win_cnt[0] = 0; win_cnt[1] = 0; game_flag = 0;
								
								}
								else{
									// buf[1] = 0;
									if( i == Gclnt_sock[0]) clnt_flag = 0;
									else clnt_flag = 1;
									

									//clnt_msg[ clnt_flag] = buf[0] - '0';
									
									buf[1] = 0;
									clnt_msg[clnt_flag] = atoi(buf);
									if( clnt_msg[ clnt_flag] != 1 && clnt_msg[ clnt_flag] != 2 && clnt_msg[ clnt_flag] != 3){
										write( Gclnt_sock[ clnt_flag], "Incorrect input, try again\n", sizeof("Incorrect input, try again\n"));
										clnt_msg[ clnt_flag] = 0;
									}
	
									if( clnt_msg[ clnt_flag] > 0 && clnt_msg[ 1- clnt_flag] > 0) { // 둘다 무언가를 낸 상황
										result = who_win(clnt_msg[0], clnt_msg[1]);
										//printf("clnt1: %d clnt2: %d, winner: %d\n", clnt_msg[0], clnt_msg[1], result);
										if( result == -1){
											write(Gclnt_sock[0], "Draw\n", sizeof("Draw\n"));
											write(Gclnt_sock[1], "Draw\n", sizeof("Draw\n"));
										}
										else {
											write(Gclnt_sock[ result], "You Win\n", sizeof("You Win\n"));
											write(Gclnt_sock[ 1- result], "You Lose\n", sizeof("You Lose\n"));
											win_cnt[result]++;
										}
										clnt_msg[0] = 0; clnt_msg[1] = 0;
										num++;
									}
								}
							}
							else {
								if(list[i].islogin == 0)
									write(i, buf, str_len);
	
								else if(list[i].islogin == 1 && strlen(buf)>=2)
								{
									sprintf(message4, "%s: %s", list[i].id, buf);
									buf[strlen(buf)-1] = '\0';
									for(j=4; j<=8; j++)
									{
										if(list[i].ban[j]>0)
												continue;
	
										if(list[j].islogin == 1 && i!=j)
											write(j, message4, strlen(message4));
									}
								}
							}
						}

					}

							
				}

			}
		}
	}
	
	close(serv_sock);
	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}

int who_win(int msg1, int msg2){
	if( msg1 == msg2) return -1;

	if( msg1 > msg2 && msg1 - msg2 == 1) return 0;
	else if( msg1 == 1 && msg2 == 3) return 0;
	else return 1;
}
