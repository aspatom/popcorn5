						  								   				   							   
#include "stm32f4xx.h"
				  	 
#ifndef __PD_USART_H
#define __PD_USART_H

#define PLUSE 		0
#define MINUS 		1
#define ENTER 		0x0D
#define ESC 		0x1B
#define CLRSCR 		"[2J"
#define CSR00 		"[H"
#define LEFT 		75
#define RIGHT 		77
#define UP 			72
#define DOWN 		80

	  		   
#define USART2_RX_BUFFER_SIZE	256	
#define USART2_TX_BUFFER_SIZE	256
#define USART3_RX_BUFFER_SIZE	256	
#define USART3_TX_BUFFER_SIZE	256	


extern int IsReceive;
extern int exit_calib;


void USART2_Configuration(void);
void USART3_Configuration(void);
			
// Rx
uint16_t USART2_GetRxBufferCount(void);
uint16_t USART2_SniffRxBuffer(uint16_t index);
void USART2_ClearRxBuffer(void);
uint8_t USART2_GetChar(uint8_t *c);
uint16_t USART2_ReadExisting(uint8_t *str);

uint16_t USART3_GetRxBufferCount(void);
uint16_t USART3_SniffRxBuffer(uint16_t index);
void USART3_ClearRxBuffer(void);
uint8_t USART3_GetChar(uint8_t *c);
uint16_t USART3_ReadExisting(uint8_t *str);

// Tx  		
void USART2_PutChar(uint8_t c);
void USART2_PutString(uint8_t *str); 
void USART2_PutString_DMA(uint8_t *str, uint16_t length);
void USART2_PutInt(int n);					
void USART2_PutUINT8(uint8_t n);
void USART2_PutINT8(int8_t n);
void USART2_PutUINT16(uint16_t n);
void USART2_PutINT16(int16_t n);
void USART2_PutUINT32(uint32_t n);
void USART2_PutINT32(int32_t n);	
void USART2_PutFloat(float n, int precision);  
void USART2_PutDouble(double n, int precision);

void USART3_PutChar(uint8_t c);
void USART3_PutString(uint8_t *str); 
void USART3_PutString_DMA(uint8_t *str, uint16_t length);
void USART3_PutInt(int n);					
void USART3_PutUINT8(uint8_t n);
void USART3_PutINT8(int8_t n);
void USART3_PutUINT16(uint16_t n);
void USART3_PutINT16(int16_t n);
void USART3_PutUINT32(uint32_t n);
void USART3_PutINT32(int32_t n);	
void USART3_PutFloat(float n, int precision);  
void USART3_PutDouble(double n, int precision);


uint16_t USART_ReceiveChar(void);
void USART_ReceiveString(void) ;


// Interrupt Service Routine	   	
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);

void Clear_screen(void);

#endif
