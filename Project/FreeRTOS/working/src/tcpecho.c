/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#include "UART_Perlib.h"
#include "main.h"
#include "json.h"
#include "string.h"
#include "netconf.h"
#include "util_Perlib.h"

#define TCPECHO_THREAD_PRIO  ( tskIDLE_PRIORITY + 3 )

extern char post_packet[450];
int flag_recv_success = 0;
extern struct netif xnetif;
static void tcp_sendData_thread(void *arg);
void tcp_SendData_init(void);

int recv_serial;
int flag_Client;


/**
 * Get the data pointer and length of the data inside a netbuf.
 *
 * @param buf netbuf to get the data from
 * @param dataptr pointer to a void pointer where to store the data pointer
 * @param len pointer to an u16_t where the length of the data is stored
 * @return ERR_OK if the information was retreived,
 *         ERR_BUF on error.
 */
err_t buf_data(char *buf, void **dataptr, u16_t *length)
{
  LWIP_ERROR("netbuf_data: invalid buf", (buf != NULL), return ERR_ARG;);
  LWIP_ERROR("netbuf_data: invalid dataptr", (dataptr != NULL), return ERR_ARG;);
	
	*length = strlen((char *)buf);
  LWIP_ERROR("netbuf_data: invalid len", (length != NULL), return ERR_ARG;);

  if (buf == NULL) {
    return ERR_BUF;
  }
  *dataptr = buf;
  return ERR_OK;
}


/*-----------------------------------------------------------------------------------*/
static void tcpecho_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err, errWrite;
	char str[15];
	
  LWIP_UNUSED_ARG(arg);

  /* Create a new connection identifier. */
  conn = netconn_new(NETCONN_TCP);
	
	if (conn!=NULL) 
	{  
		/* Bind connection to well known port number 25262. */
		err = netconn_bind(conn, NULL, RECV_PORT);
		if (err == ERR_OK) 
		{
			/* Tell connection to go into listening mode. */
			netconn_listen(conn);
			
      while (1) 
			{
        /* Grab new connection. */
        newconn = netconn_accept(conn);
        /* Process the new connection. */
        if(newconn) 
				{
          struct netbuf *buf;
          void *data;
          u16_t len;
					errWrite = ERR_OK;
					LCD_LINE_Control(Line3, "Server Connected");
					
          while((buf = netconn_recv(newconn)) != NULL) 
					{
							do 
							{
								netbuf_data(buf, &data, &len);
								errWrite = netconn_write(newconn, data, len, NETCONN_COPY);
								recv_serial  = str2int((uint8_t*)data);
								if(recv_serial >= 0)
								{
									flag_recv_success = 1;
									sprintf(str, "%d", recv_serial);
									flag_Client = 1;
									tcp_SendData_init();
									LCD_LINE_Control(Line2, str);
								}
							} while (netbuf_next(buf) >= 0);
							netbuf_delete(buf);
          }
					/* Close connection and discard connection identifier. */
					netconn_close(newconn);
					netconn_delete(newconn);
        }
				LCD_LINE_Control(Line3, "Server Disconnected");
      }
    } else {
      printf(" can not bind TCP netconn");
    }
  } else {
    printf("can not create TCP netconn");
  }
}



/*-----------------------------------------------------------------------------------*/
static void tcp_sendData_thread(void *arg)
{
	struct netconn *sendConn;
  err_t err, errSender, errWrite;
  struct ip_addr recvIPaddr;
	void *sendData;
	u16_t sendLen;
	struct netbuf *buf;
	char *str;
	char dummyStr[10] = {0,};
  LWIP_UNUSED_ARG(arg);
	
	flag_Client = 0;
	sendConn = netconn_new(NETCONN_TCP);
	IP4_ADDR( &recvIPaddr, RECV_IP_ADDR0, RECV_IP_ADDR1, RECV_IP_ADDR2, RECV_IP_ADDR3);
							

  /* Create a new connection identifier. */
	while(1)
	{
		if(sendConn == NULL)
		{
			while((sendConn = netconn_new(NETCONN_TCP)) == NULL);
		}
		if(flag_Client == 1)
		{
			if(sendConn != NULL)
			{
				//errSender = netconn_connect(sendConn,&recvIPaddr,TRANS_PORT);
				while((errSender = netconn_connect(sendConn,&recvIPaddr,TRANS_PORT)) != ERR_OK);
				errWrite = ERR_OK;
				while(errWrite == ERR_OK)
				{
					if(errSender == ERR_OK)
					{
							//LCD_LINE_Control(Line5, "Client Connected");
							if(flag_recv_success)
							{
								//sprintf(str, "Send OK %d", recv_serial);
								//LCD_LINE_Control(Line0, str);
						
								sendLen = strlen((char *)post_packet);
								json_Packet_Gen();
								buf_data(post_packet, &sendData, &sendLen);
								errWrite = netconn_write(sendConn, sendData, sendLen, NETCONN_COPY);
								flag_Client = 0;
								flag_recv_success = 0;
							}
//							else
//							{
//								sendLen = strlen((char *)dummyStr);
//								buf_data(dummyStr, &sendData, &sendLen);
//								errWrite = netconn_write(sendConn, sendData, sendLen, NETCONN_COPY);
//								LCD_LINE_Control(Line6, "Dummy Send ");
//								
//								if(errWrite != ERR_OK)
//								{
//										break;
//								}
//								//LCD_LINE_Control(Line6, "Send Massage Fail");
//							}
					}
					else
					{
							while(netconn_close(sendConn) != ERR_OK);
							//netconn_delete(sendConn);
							LCD_LINE_Control(Line5, "Client Disconnected");
					}
				}
				LCD_LINE_Control(Line5, "Client Disconnected");
				while(netconn_close(sendConn) != ERR_OK);
				//netconn_delete(sendConn);
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*/

void tcpecho_init(void)
{
  sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPECHO_THREAD_PRIO);
}


void tcp_SendData_init(void)
{
  sys_thread_new("tcp_SendData_thread", tcp_sendData_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPECHO_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
