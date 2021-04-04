#include "packet.h"
#include "serial.h"
#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<unistd.h>     /*Unix 标准函数定义*/  
#include<sys/types.h>   
#include<sys/stat.h>     
#include<fcntl.h>      /*文件控制定义*/ 
#define __USE_MISC
#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#include <sys/mman.h>
#include <poll.h>







#define FALSE  -1  
#define TRUE   0  
#define UART_NUM 12

void Uart_Out32(unsigned int * Addr, unsigned int Value)
{
	volatile unsigned int *LocalAddr = (volatile unsigned int *)Addr;
	*LocalAddr = Value;
}
 unsigned int * Uart_In32(unsigned int * Addr)
{
	return *(volatile unsigned int *) Addr;
}

uint8_t *uartdev[UART_NUM] = {
	"/dev/ttyS0",	"/dev/ttyS1",	"/dev/ttyS2",	"/dev/ttyS3",	
	"/dev/ttyS4",	"/dev/ttyS5",	"/dev/ttyS6",	"/dev/ttyS7",	
	"/dev/ttyS8",	"/dev/ttyS9",	"/dev/ttyS10",	"/dev/ttyS11",	
	
};


int main(int argc, char **argv)  
{  
    int fd;                            //文件描述符  
    int err;                           //返回调用函数的状态  
    int len;                          
    int i;  
    char rcv_buf[0x100];         
    
    char send_buf[20]="HelloWorld";
	
      fd = serial_open(uartdev[0]); //打开串口，返回文件描述符  
      err = serial_init(fd,614400,0,8,1,'N');  
        printf("Set Port Exactly!\n");  
		
        for(i = 0;i < 3;i++)  
        {  
             len = serial_send(fd,send_buf,10);  
            if(len > 0)  
                printf(" %d time send %d data successful\n",i,len);  
            else  
                printf("send data failed!\n");  
                            
            sleep(2);  
        }  
        // serial_close(fd);               
     
                                            
        while (1) //循环读取数据  
        {    
			memset(rcv_buf,0,256);
            len = serial_recv(fd, rcv_buf,100);  
              if(len > 0)  
            {  
                // rcv_buf[len] = '\0';  
				for(i = 0;i < len;i++)
                printf("receive data is 0x%x\n",rcv_buf[i]);  
                printf("len = %d\n",len);  
            }  
            else  
            {  
                printf("cannot receive data\n");  
            }  
            // sleep(2);  
        }              
        // serial_close(fd);   
      
} 