#include "serial.h"
#include "packet.h"

/*******************************************************************  
* 名称：                  serial_open  
* 功能：                打开串口并返回串口设备文件描述  
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1)  
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int serial_open(char* port)    
{    
    int fd;
    fd = open( port, O_RDWR|O_NOCTTY);
    // fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);
    if (FALSE == fd)    
    {    
        perror("Can't Open Serial Port");    
        return(FALSE);    
    }    
    //恢复串口为阻塞状态                                   
    // if(fcntl(fd, F_SETFL, 0) < 0)    
    // {    
    //     PRINT("fcntl failed!\n");    
    //     return(FALSE);    
    // }         
    // else    
    // {    
    //     // PRINT("fcntl=%d\n",fcntl(fd, F_SETFL,0));    
    // }    
    // // //测试是否为终端设备        
    // // if(0 == isatty(STDIN_FILENO))    
    // // {    
    // //     PRINT("standard input is not a terminal device\n");    
    // //     return(FALSE);    
    // // }    
    // // else    
    // // {    
    // //     // PRINT("%s is a terminal device!\n", port);    
    // // }                  
    return fd;    
}    
/*******************************************************************  
* 名称：                serial_close  
* 功能：                关闭串口并返回串口设备文件描述  
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1)  
* 出口参数：        void  
*******************************************************************/    
     
void serial_close(int fd)    
{    
    close(fd);    
}    
     
/*******************************************************************  
* 名称：                serial_set  
* 功能：                设置串口数据位，停止位和效验位  
* 入口参数：        fd        串口文件描述符  
*                              speed     串口速度  
*                              flow_ctrl   数据流控制  
*                           databits   数据位   取值为 7 或者8  
*                           stopbits   停止位   取值为 1 或者2  
*                           parity     效验类型 取值为N,E,O,,S  
*出口参数：          正确返回为1，错误返回为0  
*******************************************************************/    
int serial_set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)    
{    
       
    int   i;    
    //int   status;    
    int   speed_arr[] = { B115200, B38400, B19200, B9600, B4800, B2400, B1200, B1000000,B614400};    
    //int   name_arr[] = {115200, 38400, 19200,  9600,  4800,  2400,  1200,  300};  
    // int   name_arr[] = {4,  3, 2,  1,  0,  2400,  1200,  300};   
    int   name_arr[] = {115200, 38400, 19200,  9600,  4800,  2400,  1200,  1000000,614400};
             
    struct termios options;

    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.  
    */    
    if( tcgetattr( fd,&options)  !=  0)    
    {    
        perror("SetupSerial 1");        
        return(FALSE);     
    }    
      
    //设置串口输入波特率和输出波特率    
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)    
    {    
        if  (speed == name_arr[i])    
        {                 
            cfsetispeed(&options, speed_arr[i]);   
            printf("speed is %d----------------------------\n",speed_arr[i]);  
            cfsetospeed(&options, speed_arr[i]);      
        }    
    }         
       
    //修改控制模式，保证程序不会占用串口    
    options.c_cflag |= CLOCAL;    
    //修改控制模式，使得能够从串口中读取输入数据    
    options.c_cflag |= CREAD;
    
    //清bit位 关闭字符映射 0x0a 0x0d
    options.c_iflag &= ~(INLCR|ICRNL);
    //清bit位 关闭流控字符 0x11 0x13
    options.c_iflag &= ~(IXON);
    options.c_cflag |= CBAUDEX; //设置特定波特率的标志位.
      
    //设置数据流控制    
    switch(flow_ctrl)    
    {    
          
        case 0 ://不使用流控制    
              options.c_cflag &= ~CRTSCTS;    
              break;       
          
        case 1 ://使用硬件流控制    
              options.c_cflag |= CRTSCTS;    
              break;
        case 2 ://使用硬件流控制DTR/DSR
              options.c_cflag &= ~CRTSCTS;
              break;
        case 3 ://使用软件流控制    
              options.c_cflag |= IXON | IXOFF | IXANY;    
              break;    
    }    
    //设置数据位    
    //屏蔽其他标志位    
    options.c_cflag &= ~CSIZE;    
    switch (databits)    
    {      
        case 5    :    
                     options.c_cflag |= CS5;    
                     break;    
        case 6    :    
                     options.c_cflag |= CS6;    
                     break;    
        case 7    :        
                 options.c_cflag |= CS7;    
                 break;    
        case 8:        
                 options.c_cflag |= CS8;    
                 break;      
        default:       
                 fprintf(stderr,"Unsupported data size\n");    
                 return (FALSE);     
    }    
    //设置校验位    
    switch (parity)    
    {      
        case 0:    
        case 'N': //无奇偶校验位。    
                 options.c_cflag &= ~PARENB;     
                 options.c_iflag &= ~INPCK;        
                 break;     
        case 1:      
        case 'O'://设置为奇校验        
                 options.c_cflag |= (PARODD | PARENB);     
                 options.c_iflag |= INPCK;                 
                 break;     
        case 2:     
        case 'E'://设置为偶校验      
                 options.c_cflag |= PARENB;           
                 options.c_cflag &= ~PARODD;           
                 options.c_iflag |= INPCK;          
                 break;    
        case 's':    
        case 'S': //设置为空格     
                 options.c_cflag &= ~PARENB;    
                 options.c_cflag &= ~CSTOPB;    
                 break;     
        default:      
                 fprintf(stderr,"Unsupported parity\n");        
                 return (FALSE);     
    }     
    // 设置停止位     
    switch (stopbits)    
    {      
        case 1:       
                 options.c_cflag &= ~CSTOPB; break;     
        case 2:       
                 options.c_cflag |= CSTOPB; break;    
        default:       
                       fprintf(stderr,"Unsupported stop bits\n");     
                       return (FALSE);    
    }    
       
    //修改输出模式，原始数据输出    
    options.c_oflag &= ~OPOST;    
      
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);    
    //options.c_lflag &= ~(ISIG | ICANON);    
       
    //设置等待时间和最小接收字符    
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */      
    options.c_cc[VMIN] = 32; /* 读取字符的最少个数为1 */    
       
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读    
    tcflush(fd,TCIFLUSH);    
       
    //激活配置 (将修改后的termios数据设置到串口中）    
    if (tcsetattr(fd,TCSANOW,&options) != 0)      
    {    
        perror("com set error!\n");      
        return (FALSE);     
    }    
    return (TRUE);     
}    
/*******************************************************************  
* 名称：                serial_init()  
* 功能：                串口初始化  
* 入口参数：        fd       :  文件描述符     
*               speed  :  串口速度  
*               flow_ctrl  数据流控制  
*               databits   数据位   取值为 7 或者8  
*               stopbits   停止位   取值为 1 或者2  
*               parity     效验类型 取值为N,E,O,,S  
*                        
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int serial_init(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)    
{       
    //设置串口数据帧格式    
    //if (serial_set(fd,19200,0,8,1,'N') == FALSE)
    if((speed < 0) || (speed > 10000000))
        return 1;
    if (serial_set(fd,speed,flow_ctrl,databits,stopbits,parity) == FALSE)
    {
        return FALSE;
    }
    else    
    {    
        return  TRUE;    
    }    
}    
     
/*******************************************************************  
* 名称：                  serial_recv  
* 功能：                接收串口数据  
* 入口参数：            fd       :  文件描述符     
*                    rcv_buf   :接收串口中数据存入rcv_buf缓冲区中  
*                    data_len   :数据的长度 
*                               
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int serial_recv(int fd, char *rcv_buf,int data_len)  
{  
    int len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;  
     
    FD_ZERO(&fs_read);  
    FD_SET(fd,&fs_read);  
     
    time.tv_sec = 10;  
    time.tv_usec = 0;  
     
	len = read(fd,rcv_buf,data_len);  
        // printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  
    return len;  
    //使用select实现串口的多路通信  
	/* 入口参数：
      ①：ndfs：select() 中监视的文件句柄，一般设为要监视的文件中的最大文件号加一。
      ②：rdfds：select（）监视的可读文件句柄集合，当rdfds映象的文件句柄状态变成可读时系统告诉select函数返回。这个集合中有一个文件可读，select就会返回一个大于0的值，表示有文件可读，如果没有可读的文件，则根据timeout参数再判断是否超时，若超出timeout的时间，select返回0，若发生错误返回负值，可以传入NULL值，表示不关心任何文件的读变化；
      ③：wtfds: select（）监视的可写文件句柄集合，当wtfds映象的文件句柄状态变成可写时系统告诉select函数返回。
        如果这个集合中有一个文件可写，select就会返回一个大于0的值，表示有文件可写，
        如果没有可写的文件，则根据timeout参数再判断是否超时，
        若超出timeout的时间，select返回0，若发生错误返回负值，
        可以传入NULL值，表示不关心任何文件的写变化。
      ④：exfds：select（）监视的异常文件句柄集合，当exfds映象的文件句柄上有特殊情况发生时系统会告诉select函数返回。
      ⑤：timeout：select（）的超时结束时间 */
	  /* FD_ZERO(fd_set *fdset)：清空fdset与所有文件句柄的联系。
         FD_SET(int fd, fd_set *fdset)：建立文件句柄fd与fdset的联系。
         FD_CLR(int fd, fd_set *fdset)：清除文件句柄fd与fdset的联系。
         FD_ISSET(int fd, fdset *fdset)：检查fdset联系的文件句柄fd是否可读写，>0表示可读写。
 */
    // fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);  
    // // printf("fs_sel = %d\n",fs_sel);  
    // if(fs_sel)  
    // {  
    //     len = read(fd,rcv_buf,data_len);  
    //     // printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  
    //     return len;  
    // }  
    // else  
    // {  
    //     // printf("Sorry,I am wrong!");  
    //     return FALSE;  
    // }       
}  
/********************************************************************  
* 名称：                  serial_send  
* 功能：                发送数据  
* 入口参数：        fd                  :文件描述符      
*                              send_buf    :存放串口发送数据  
*                              data_len    :一帧数据的个数  
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int serial_send(int fd, char *send_buf,int data_len)    
{    
    int len = 0;    
       
    len = write(fd,send_buf,data_len);    
    if (len == data_len )    
    {    
        PRINT("send data is %s\n",send_buf);  
        return len;    
    }         
    else       
    {    
                   
        tcflush(fd,TCOFLUSH);    
        return FALSE;    
    }    
       
}
