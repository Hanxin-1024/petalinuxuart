#ifndef _PACKET_H
#define _PACKET_H

#include <stdint.h>

// #define DEBUG
#ifdef DEBUG
#define PRINT(fmt, ...)   printf(fmt, ##__VA_ARGS__)
#else
#define PRINT(fmt, ...)
#endif

typedef enum {
    UART_SND_REQ            = 0x11,
    UART_UPLOAD             = 0x22,
    UART_ATTR_SET_REQ       = 0x33,
    UART_ATTR_GET_REQ       = 0x44,
    UART_ATTR_GET_ACK       = 0x44,
    JTAG_CONNECT_SET_REQ    = 0x55,
    JTAG_CONNECT_GET_REQ    = 0x66,
    JTAG_CONNECT_GET_ACK    = 0x66,
    IP_ADDR_SET_REQ         = 0x77,
    IP_ADDR_GET_REQ         = 0x71,
    IP_ADDR_GET_ACK         = 0x71,
    SYSTEM_RESET            = 0x72
} MANAGE_TYPE;

typedef struct {
	uint32_t    PackageLen;
	uint8_t     ManageType;
} __attribute__((__packed__)) HEADER;

typedef struct {
	HEADER      header;
    uint8_t 	PortID;
    uint8_t     Data[];
} __attribute__((__packed__)) UART_SND;

typedef struct {
	HEADER      header;
    uint8_t 	PortID;
	uint8_t 	Baudrate;
	uint8_t	    DataBit;
    uint8_t	    StopBit;
    uint8_t	    Pirity;
    uint8_t	    FlowCtrl;
	uint8_t	    CheckSum;
} __attribute__((__packed__)) UART_SET_REQ;

typedef struct {
	HEADER      header;
    uint8_t 	PortID;
	uint8_t	    CheckSum;
} __attribute__((__packed__)) UART_GET_REQ;

#define UART_GET_ACK UART_SET_REQ
#define JTAG_SET_REQ UART_GET_REQ
#define JTAG_INQ_REQ UART_GET_REQ   //Jtag connect status inquire request
#define JTAG_INQ_ACK UART_GET_REQ   //Jtag connect status inquire answer
#define IP_INQ_REQ   UART_GET_REQ
#define COMMON_ACK   UART_GET_REQ
#define IP_INQ_ACK   IP_SET_REQ

typedef struct {
	HEADER      header;
	uint8_t		IpAddr[4];
    uint8_t		NetMask[4];
    uint8_t		GateWay[4];
    uint8_t		CheckSum;
} __attribute__((__packed__)) IP_SET_REQ;

typedef struct {
    int sock;
    char status;
    uint8_t ip[20];
} TCP_CLIENT;

#endif