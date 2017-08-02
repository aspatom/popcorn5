#include "stm32f4_discovery.h"
#include "UART_Perlib.h"

 
 
/*
 * USART DMA setting on STM32F4
 * USART1 Tx --> DMA2 Stream7 Channel4
 * USART1 Rx --> DMA2 Stream2 Channel4		/		DMA2 Stream5 Channel4
 * USART2 Tx --> DMA1 Stream6 Channel4
 * USART2 Rx --> DMA1 Stream5 Channel4
 * USART3 Tx --> DMA1 Stream3 Channel4		/		DMA1 Stream4 Channel7
 * USART3 Rx --> DMA1 Stream1 Channel4
 *
 * UART4 Tx   --> DMA1 Stream4 Channel4
 * UART4 Rx   --> DMA1 Stream2 Channel4
 * UART5 Tx   --> DMA1 Stream7 Channel4
 * UART5 Rx   --> DMA1 Stream0 Channel4
 
 
 */

									
/* ==============================================================================================
	// Local Variable
============================================================================================== */


uint16_t USART2_RxBuffer[USART2_RX_BUFFER_SIZE];
uint16_t USART2_RxGetIndex;
				
uint16_t USART2_TxBuffer[USART2_TX_BUFFER_SIZE];
uint16_t USART2_TxPutIndex;
uint16_t USART2_TxGetIndex;	

uint16_t USART3_RxBuffer[USART3_RX_BUFFER_SIZE];
uint16_t USART3_RxGetIndex;
				
uint16_t USART3_TxBuffer[USART3_TX_BUFFER_SIZE];
uint16_t USART3_TxPutIndex;
uint16_t USART3_TxGetIndex;	



uint16_t Buffer_Array[10];
int IsReceive = 0;
int exit_calib = 0;
int Rx_GetIndex = 0;
int Buffer_Size = 0;		


						  		
/* ==============================================================================================
	// Configuration
============================================================================================== */
void USART2_Configuration(void)
{				
	NVIC_InitTypeDef NVIC_InitStructure;		  	
	DMA_InitTypeDef DMA_InitStructure;		  							  
	USART_InitTypeDef USART_InitStructure;	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	uint16_t i;
				   
	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);	
						   
 	//* USART2 Rx   --> DMA1 Stream5 Channel4
	/* DMA1 Clock Enable */									   
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
					
	/* USART2 Rx DMA1 Stream5 (triggered by USART2 Rx event) Config */
	DMA_DeInit(DMA1_Stream5);  
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART2_RxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = USART2_RX_BUFFER_SIZE;		   
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
					  
	/* Enable USART2 Clock */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	
	/* USART Setting : Baud(9600)/8/N/1/N */														
	USART_InitStructure.USART_BaudRate = 115200;		//9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		
	USART_Init(USART2, &USART_InitStructure);  	
					   		
	/* Configure PA2 PA3 USART2 */			  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
			
	for (i = 0; i < USART2_TX_BUFFER_SIZE; i++)	  
		USART2_TxBuffer[i] = 0;

	for (i = 0; i < USART2_RX_BUFFER_SIZE; i++)
		USART2_RxBuffer[i] = 0;		
			
	USART2_TxPutIndex = 0;
	USART2_TxGetIndex = 0;	
	
	/* Enable USART2 DMA Rx request */
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);		
							  
	/* Enable USART2 Rx DMA1 Stream5 */ 
	DMA_Cmd(DMA1_Stream5, ENABLE);

	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE); 	 

}
void USART3_Configuration(void)
{				
//	NVIC_InitTypeDef NVIC_InitStructure;		  	
//	DMA_InitTypeDef DMA_InitStructure;		  							  
//	USART_InitTypeDef USART_InitStructure;	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	uint16_t i;
//				   
//	/* Enable the USART3 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  	NVIC_Init(&NVIC_InitStructure);	
//						   
// 	//* USART3 Rx   --> DMA1 Stream1 Channel4
//	/* DMA1 Clock Enable */									   
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//					
//	/* USART3 Rx DMA1 Stream2 (triggered by USART3 Rx event) Config */
//	DMA_DeInit(DMA1_Stream1);  
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART3_RxBuffer;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_BufferSize = USART3_RX_BUFFER_SIZE;		   
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA1_Stream1, &DMA_InitStructure);
//					  
//	/* Enable USART3 Clock */
//  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
//	
//	/* USART Setting : Baud(115200)/8/N/1/N */														
//	USART_InitStructure.USART_BaudRate = 115200;		//115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		
//	USART_Init(USART3, &USART_InitStructure);  	
//					   		
//	/* Configure PA0 PA1 USART3 */			  
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);  
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);				   
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
//			
//	for (i = 0; i < USART3_TX_BUFFER_SIZE; i++)	  
//		USART3_TxBuffer[i] = 0;

//	for (i = 0; i < USART3_RX_BUFFER_SIZE; i++)
//		USART3_RxBuffer[i] = 0;		
//			
//	USART3_TxPutIndex = 0;
//	USART3_TxGetIndex = 0;	
//	
//	/* Enable USART3 DMA Rx request */
//	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);		
//							  
//	/* Enable USART3 Rx DMA1 Stream2 */ 
//	DMA_Cmd(DMA1_Stream1, ENABLE);

//	/* Enable the USART3 */
//	USART_Cmd(USART3, ENABLE); 	 

	GPIO_InitTypeDef     GPIO_InitStructureGPIO;
	USART_InitTypeDef   USART_InitStructure;								  
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	

	GPIO_InitStructureGPIO.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructureGPIO.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructureGPIO.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructureGPIO.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructureGPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructureGPIO);
	

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		 // we want to configure the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// this sets the priority group of the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		 // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART3 interrupts are globally enabled

	NVIC_Init(&NVIC_InitStructure);	
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt 

	/* Enable the USART */

	USART_Cmd(USART3, ENABLE);

	/* Configure USART */


	
}

	
/* ==============================================================================================
	// Method - Rx
============================================================================================== */
					
uint16_t USART2_GetRxBufferCount(void)
{
	return ((uint16_t)(DMA1_Stream5->NDTR) + USART2_RX_BUFFER_SIZE - USART2_RxGetIndex) % USART2_RX_BUFFER_SIZE;
}
	
uint16_t USART2_SniffRxBuffer(uint16_t index)
{
 	if (USART2_RxGetIndex != (uint16_t)(DMA1_Stream5->NDTR))
		return USART2_RxBuffer[(USART2_RxGetIndex + index) % USART2_RX_BUFFER_SIZE];
	else
		return 0;
}
				  
void USART2_ClearRxBuffer(void)
{
	USART2_RxGetIndex = (uint16_t)(DMA1_Stream5->NDTR);
}

uint8_t USART2_GetChar(uint8_t *c)
{	
	uint16_t putIndex = (uint16_t)USART2_RX_BUFFER_SIZE - (uint16_t)(DMA1_Stream5->NDTR);
	
 	if (USART2_RxGetIndex != putIndex)
	{
		*c = USART2_RxBuffer[USART2_RxGetIndex];
		USART2_RxGetIndex = (USART2_RxGetIndex + 1) % USART2_RX_BUFFER_SIZE;
		return 1;
	}
	else
		return 0;
}
	
uint16_t USART2_ReadExisting(uint8_t *str)
{
	uint16_t cnt = 0;
	uint16_t putIndex = (uint16_t)(DMA1_Stream5->NDTR);

	while (USART2_RxGetIndex != putIndex)
	{
		*(str + cnt) = USART2_RxBuffer[USART2_RxGetIndex];
		cnt++;
		USART2_RxGetIndex = (USART2_RxGetIndex + 1) % USART2_RX_BUFFER_SIZE;
	};		 

	*(str + cnt) = 0;

	return cnt;
}
////////////////////////////////////////////					
uint16_t USART3_GetRxBufferCount(void)
{
	return ((uint16_t)(DMA1_Stream1->NDTR) + USART3_RX_BUFFER_SIZE - USART3_RxGetIndex) % USART3_RX_BUFFER_SIZE;
}
	
uint16_t USART3_SniffRxBuffer(uint16_t index)
{
 	if (USART3_RxGetIndex != (uint16_t)(DMA1_Stream1->NDTR))
		return USART3_RxBuffer[(USART3_RxGetIndex + index) % USART3_RX_BUFFER_SIZE];
	else
		return 0;
}
				  
void USART3_ClearRxBuffer(void)
{
	USART3_RxGetIndex = (uint16_t)(DMA1_Stream1->NDTR);
}

uint8_t USART3_GetChar(uint8_t *c)
{	
	uint16_t putIndex = (uint16_t)USART3_RX_BUFFER_SIZE - (uint16_t)(DMA1_Stream1->NDTR);
	
 	if (USART3_RxGetIndex != putIndex)
	{
		*c = USART3_RxBuffer[USART3_RxGetIndex];
		USART3_RxGetIndex = (USART3_RxGetIndex + 1) % USART3_RX_BUFFER_SIZE;

		return 1;
	}
	else
		return 0;
}
	
uint16_t USART3_ReadExisting(uint8_t *str)
{
	uint16_t cnt = 0;
	uint16_t putIndex = (uint16_t)(DMA1_Stream1->NDTR);

	while (USART3_RxGetIndex != putIndex)
	{
		*(str + cnt) = USART3_RxBuffer[USART3_RxGetIndex];
		cnt++;
		USART3_RxGetIndex = (USART3_RxGetIndex + 1) % USART3_RX_BUFFER_SIZE;
	};		 

	*(str + cnt) = 0;

	return cnt;
}	  





uint16_t USART_ReceiveChar(void)
{
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE));
	return USART3->DR;	
}

void USART_ReceiveString(void)     // Save the Data to Buffer_Array and Return Buffer Size
{
	uint16_t Receive_Data = 0;
	
	
	Receive_Data = USART_ReceiveChar();
	Buffer_Array[Rx_GetIndex] = Receive_Data;
	Rx_GetIndex++;
	if(Receive_Data == 'c')
	{ 
		IsReceive = 1;
		Buffer_Size = Rx_GetIndex;	//Test
		Rx_GetIndex = 0;		
	}
	if(Receive_Data == 'q')
	{ 
		exit_calib = 1;
		Buffer_Size = Rx_GetIndex;	//Test
		Rx_GetIndex = 0;		
	}
	
	return;	
}



/* ==============================================================================================
	// Method - Tx
============================================================================================== */
void USART2_PutChar(uint8_t c)
{			  
	USART2_TxBuffer[USART2_TxPutIndex] = c;
	USART2_TxPutIndex = (USART2_TxPutIndex + 1) % USART2_TX_BUFFER_SIZE;

	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void USART2_PutString(uint8_t *str)
{
	while (*str)
		USART2_PutChar(*(str++));
}	
   	
void USART2_PutInt(int n)
{
	int r = 1;

	if (n == 0)
	{
		USART2_PutChar('0');
		return;
	}

	if (n < 0)
	{
		USART2_PutChar('-');
		n = -n;
	}

	while (n / r)
		r *= 10;

	while (r != 1)
	{
		r /= 10;
		USART2_PutChar((n % (r * 10)) / r + '0');
	};
}

void USART2_PutUINT8(uint8_t n) 	{ USART2_PutInt((int)n); }
void USART2_PutINT8(int8_t n) 		{ USART2_PutInt((int)n); }	
void USART2_PutUINT16(uint16_t n) 	{ USART2_PutInt((int)n); }
void USART2_PutINT16(int16_t n) 	{ USART2_PutInt((int)n); }
void USART2_PutUINT32(uint32_t n) 	{ USART2_PutInt((int)n); }
void USART2_PutINT32(int32_t n) 	{ USART2_PutInt((int)n); }
	   		
void USART2_PutFloat(float n, int precision)
{
	int n1, i;

	if (n == 0)
	{
		USART2_PutChar('0');
		return;
	}
		
	if (n < 0)
	{
		USART2_PutChar('-');
		n = -n;
	}

	n1 = (int)n;  
	n -= (float)n1;

	USART2_PutInt(n1);		

	if (n == 0)
		return;
   	
	USART2_PutChar('.');

	for (i = 0; i < precision; i++)
	{			  
		n *= 10;
		if (n == 0)
			return;
		USART2_PutChar((int)n + '0');	
		n -= (int)n;
	};
}

void USART2_PutDouble(double n, int precision)
{
	int n1, i;

	if (n == 0)
	{
		USART2_PutChar('0');
		return;
	}
		
	if (n < 0)
	{
		USART2_PutChar('-');
		n = -n;
	}

	n1 = (int)n;  
	n -= (double)n1;

	USART2_PutInt(n1);		

	if (n == 0)
		return;
   	
	USART2_PutChar('.');

	for (i = 0; i < precision; i++)
	{			  
		n *= 10;
		if (n == 0)
			return;
		USART2_PutChar((int)n + '0');	
		n -= (int)n;
	};
}
//////////////////////////////////////////////


void USART3_PutChar(uint8_t c)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, c);

}

void USART3_PutString(uint8_t *str)
{							             
	while(*str)
	{
		USART3_PutChar(*str);
		*str++;
	}
}






//void USART3_PutChar(uint8_t c)
//{
//	USART3_TxBuffer[USART3_TxPutIndex] = c;
//	USART3_TxPutIndex = (USART3_TxPutIndex + 1) % USART3_TX_BUFFER_SIZE;

//	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
//}

//void USART3_PutString(uint8_t *str)
//{
//	while (*str)
//		USART3_PutChar(*(str++));
//}	
   	
void USART3_PutInt(int n)
{
	int r = 1;

	if (n == 0)
	{
		USART3_PutChar('0');
		return;
	}

	if (n < 0)
	{
		USART3_PutChar('-');
		n = -n;
	}

	while (n / r)
		r *= 10;

	while (r != 1)
	{
		r /= 10;
		USART3_PutChar((n % (r * 10)) / r + '0');
	};
}

void USART3_PutUINT8(uint8_t n) 	{ USART3_PutInt((int)n); }
void USART3_PutINT8(int8_t n) 		{ USART3_PutInt((int)n); }	
void USART3_PutUINT16(uint16_t n) 	{ USART3_PutInt((int)n); }
void USART3_PutINT16(int16_t n) 	{ USART3_PutInt((int)n); }
void USART3_PutUINT32(uint32_t n) 	{ USART3_PutInt((int)n); }
void USART3_PutINT32(int32_t n) 	{ USART3_PutInt((int)n); }
	   		
void USART3_PutFloat(float n, int precision)
{
	int n1, i;

	if (n == 0)
	{
		USART3_PutChar('0');
		return;
	}
		
	if (n < 0)
	{
		USART3_PutChar('-');
		n = -n;
	}

	n1 = (int)n;  
	n -= (float)n1;

	USART3_PutInt(n1);		

	if (n == 0)
		return;
   	
	USART3_PutChar('.');

	for (i = 0; i < precision; i++)
	{			  
		n *= 10;
		if (n == 0)
			return;
		USART3_PutChar((int)n + '0');	
		n -= (int)n;
	};
}

void USART3_PutDouble(double n, int precision)
{
	int n1, i;

	if (n == 0)
	{
		USART3_PutChar('0');
		return;
	}
		
	if (n < 0)
	{
		USART3_PutChar('-');
		n = -n;
	}

	n1 = (int)n;  
	n -= (double)n1;

	USART3_PutInt(n1);		

	if (n == 0)
		return;
   	
	USART3_PutChar('.');

	for (i = 0; i < precision; i++)
	{			  
		n *= 10;
		if (n == 0)
			return;
		USART3_PutChar((int)n + '0');	
		n -= (int)n;
	};
}


/* ==============================================================================================
	// Event Handler
============================================================================================== */
void USART2_IRQHandler(void)
{	   	   
	USART_ClearITPendingBit(USART2, USART_IT_RXNE | USART_IT_TXE);

	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{		
		if (USART2_TxGetIndex != USART2_TxPutIndex)
		{
			USART2->DR = USART2_TxBuffer[USART2_TxGetIndex];			//USART_SendData(USART1, Usart1_TxBuffer[Usart1_TxGetIndex]);
			USART2_TxGetIndex = (USART2_TxGetIndex + 1) % USART2_TX_BUFFER_SIZE;
		}	
		else
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);	  
	}				 						 
}
//////////////////////////////////////////////
void USART3_IRQHandler(void)
{	   	   
	
//		char ch;
//	
//	if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
//	{	
//		USART_ClearITPendingBit(USART3, USART_IT_TXE);
//		if (USART3_TxGetIndex != USART3_TxPutIndex)
//		{
//			USART3->DR = USART3_TxBuffer[USART3_TxGetIndex];			//USART_SendData(USART1, Usart1_TxBuffer[Usart1_TxGetIndex]);
//			USART3_TxGetIndex = (USART3_TxGetIndex + 1) % USART3_TX_BUFFER_SIZE;
//		}	
//		else
//			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);	  
//	}				 						 

//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
//    {
//		USART_ClearITPendingBit(USART3, USART_IT_RXNE);					   
//		USART_ReceiveString();
//	}
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);			
		USART_ReceiveString();		   
		GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
	}
	
	
	
}


void Clear_screen(void)
{
	USART3_PutChar(ESC); // ESC
	USART3_PutString(CLRSCR); // clear screen
	USART3_PutChar(ESC); // ESC
	USART3_PutString(CSR00); // cursor to home
}



