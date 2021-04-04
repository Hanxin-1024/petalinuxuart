#ifndef _SERIAL_H_
#define _SERIAL_H_

#include<stdio.h>      /*标准输入输出定义*/    
#include<stdlib.h>     /*标准函数库定义*/    
#include<unistd.h>     /*Unix 标准函数定义*/    
#include<sys/types.h>     
#include<sys/stat.h>       
#include<fcntl.h>      /*文件控制定义*/    
#include<termios.h>    /*PPSIX 终端控制定义*/    

#include<errno.h>      /*错误号定义*/    
#include<string.h>
#include<sys/time.h>

//宏定义    
#define FALSE  -1    
#define TRUE   0  

#define   B76800  0010020
#define   B153600 0010021
#define   B307200 0010022
#define   B614400 0010023
//串口相关函数的头文件
int serial_open(char* port);
int serial_init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);
int serial_recv(int fd, char *rcv_buf,int data_len);
int serial_send(int fd, char *send_buf,int data_len);
void serial_close(int fd);

#endif