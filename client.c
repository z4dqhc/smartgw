#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

extern int net_int(char *client_ip,unsigned short portvalue);
extern int set_opt(int,int,int,char,int);
extern int uart_check(char *uart,char *uart_buffer);

int main(int argc, char **argv)
{
	int cfd;
	int recbyte,nByte;
	char buffer[1024] = {0};
	pid_t pid;
	unsigned short portnum = 0x8888;
	int fd,wr_static,i=10;
	char *uart3 = "/dev/ttySAC3";
	char *uart_buffer = "hello world!\r\n";
	
	printf("Hello,welcome to client!\r\n");

	if(argc == 3){
		portnum = atoi(argv[2]);
		printf("portnum is 0x%x !\n",portnum);
	}
	
	if(argc < 2){
		printf("usage: echo ip\n");
		return -1;
	}
	if((cfd = net_int(argv[1],portnum))==0){
		return -1;
	}
	if((fd = uart_check(uart3,uart_buffer))==-1){
		return -1;
	}
	
	pid = fork();
	if(pid == -1){
		printf("fork failed\n");
		return 1;
	}
	else if(pid){
		while(1){
			if(-1 == (recbyte = read(cfd, buffer, 1024))){
				printf("read data fail !\r\n");
				return -1;
			}
			buffer[recbyte]='\0';
			printf("%s\r\n",buffer);
			
			if(strlen(buffer)>0){
				wr_static = write(fd,buffer, strlen(buffer));
				memset(buffer, 0, 1024);
			}
			usleep(50);
		}
	}
	else{
		while(1){
			while((nByte = read(fd, buffer, 512))>0){
				write(fd,buffer,nByte);
				printf("read uart1 data %s!\n",buffer);
				
				buffer[nByte]='\0';
				printf("%s\r\n",buffer);
			
				send(cfd, buffer, nByte, 0);
				nByte = 0;	
			}
			usleep(50);
		}
	}
	
	close(cfd);
	close(fd);
	
	return 0;
}