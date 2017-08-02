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
#include "lwip/sys.h"
#include "lwip/api.h"
#include "UART_Perlib.h"
#include "main.h"
#include "json.h"
#include "string.h"
#include "netconf.h"
#include "util_Perlib.h"


static char LCD_String_Line[10][20];
int flag_LCD_Line_Update = 0;
int clear_line;



#define LCDCONTROL_THREAD_PRIO  ( tskIDLE_PRIORITY + 3 )



/*-----------------------------------------------------------------------------------*/
static void LCD_Control_thread(void *arg)
{
	int i;
  LWIP_UNUSED_ARG(arg);
	
	while(1)
	{
			if(flag_LCD_Line_Update == 1)
			{
				/* Clear the LCD */
				LCD_ClearLine(clear_line);
				for(i = 0 ; i < 10 ; i++)
				{
					LCD_DisplayStringLine(LINE(i), (uint8_t*)LCD_String_Line[i]);
				}
				flag_LCD_Line_Update = 0;
			}
	}
}

void LCD_LINE_Control(int line, char* str)
{
		LCD_ClearLine(line);
		sprintf(LCD_String_Line[line], "%s", str);
		LCD_DisplayStringLine(line, (uint8_t*)LCD_String_Line[line]);
	//clear_line = line;
	//flag_LCD_Line_Update = 1;
}







void LCD_Control_init(void)
{
  sys_thread_new("LCD_Control_thread", LCD_Control_thread, NULL, DEFAULT_THREAD_STACKSIZE, LCDCONTROL_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/



