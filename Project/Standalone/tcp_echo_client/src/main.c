/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; Portions COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */
/**
  ******************************************************************************
  * <h2><center>&copy; Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
  * @file    main.c
  * @author  CMP Team
  * @version V1.0.0
  * @date    28-December-2012
  * @brief   Main program body      
  *          Modified to support the STM32F4DISCOVERY, STM32F4DIS-BB and
  *          STM32F4DIS-LCD modules. 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main.h"
#include "tcp_echoclient.h"
#include "jsmn.h"
#include "TIM_Perlib.h"
#include "UART_Perlib.h"
#include "json.h"
#include "tcp_server.h"
#include "string.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/*--------------- LCD Messages ---------------*/
#define MESSAGE1   "     STM32F4x7      "
#define MESSAGE2   "  STM32F-4 Series   "
#define MESSAGE3   " TCP echoclient Demo"
#define MESSAGE4   "                    "

#ifdef __GNUC__
 #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
 #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
uint32_t Button_TimerBack;
__IO uint32_t Button_Flag;
 
 
 
static struct tcp_pcb *tcp_pcb;
 
int recv_serial;
int flag_err_recv_serial; 
int flag_server_accept;
int flag_recv_serial;
 

extern uint32_t count_uS;	
extern uint32_t count_mS;	 
extern uint32_t countT;
extern uint32_t countTime;
extern char post_packet[300];

/* Private function prototypes -----------------------------------------------*/
void LCD_LED_BUTTON_Init(void);
uint8_t Button_State(void);

void EVAL_LEDInit(void);
void print_IP(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured to 
       168 MHz, this is done through SystemInit() function which is called from
       startup file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
	
	/*Variables for the JSON Protocol*/ 
  
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	int r;
	struct netconn *conn, *newconn;
	 err_t err, accept_err;
  struct netbuf *buf;
  void *data;
  u16_t len;
	
	
  /*Initialize LCD and Leds */ 
  LCD_LED_BUTTON_Init();
	EVAL_LEDInit();
	TIM2_IT_Config();
	USART3_Configuration();
  
  /* Configure ethernet (GPIOs, clocks, MAC, DMA) */ 
  ETH_BSP_Config();
    
  /* Initilaize the LwIP stack */
  LwIP_Init();
	
	/* server init */
//	tcp_server_init();
	
  /* Initilaize the jsmn */
	jsmn_init(&p);
	STM_EVAL_LEDOff(LED4);
	STM_EVAL_LEDOff(LED3);
	STM_EVAL_LEDOff(LED5);
	STM_EVAL_LEDOff(LED6);
	
	
	/* Initilaize the flags */
	flag_err_recv_serial = 0;
	flag_server_accept = 0;
	flag_recv_serial = 0;
	
	printf("HelloWorld\r\n");
	json_Packet_Gen();
	printf("%s\r\n",post_packet);
  /* Infinite loop */
  
//			tcp_server_init();
	
	while (1)
  {
			//tcp_server_init();
		
		
		while(1)
			{
				/* check if any packet received */
				if (ETH_CheckFrameReceived()) 
				{ 
					/* process received ethernet packet */
					LwIP_Pkt_Handle();
					tcp_server_recvData();
				}
				/* handle periodic timers for LwIP */
				LwIP_Periodic_Handle(LocalTime);
			}
			
		
			while(!flag_err_recv_serial)
			{	
				/* check if any packet received */
				if (ETH_CheckFrameReceived()) { 
					/* process received ethernet packet */
					LwIP_Pkt_Handle();
					
				}
				while(!flag_recv_serial)
				{
					tcp_server_recvData();
				}
				USART3_PutString("system >> received Data\r\n");
			
				
				STM_EVAL_LEDToggle(LED4);
				tcp_echoclient_connect();   
				flag_recv_serial = 0;
//				if(count_mS >= 500)
//				{
//						USART3_PutString("TIME OUT \r\n");
//						print_IP();
//						
//					
//						count_mS = 0;
//						tcp_echoclient_connect_sustained();
//						STM_EVAL_LEDToggle(LED5);
//				}			
//				
//				
//				
//		//	  tcp_echoclient_connect_sustained();
//				if (Button_State()) {
//					USART3_PutString("Event >> Button Pressed \r\n");
//					/*connect to tcp server */ 
//					STM_EVAL_LEDToggle(LED4);
//					tcp_echoclient_connect();   
//			 }
		 }
			flag_err_recv_serial = 0;
			flag_server_accept = 0;
		 flag_recv_serial = 0;
		
  }   
}

void print_IP(void)
{
	unsigned char bytes[4];

	bytes[0] = netif.ip_addr.addr & 0xFF;
	bytes[1] = (netif.ip_addr.addr >> 8) & 0xFF;
	bytes[2] = (netif.ip_addr.addr >> 16) & 0xFF;
	bytes[3] = (netif.ip_addr.addr >> 24) & 0xFF;   
	printf("Connected IP : %d.%d.%d.%d\r\n", bytes[0], bytes[1], bytes[2], bytes[3]);	
}


/**
  * @brief  Button state.
  * @param  None.
  * @retval 1: button is pressed.
  *         0: button is unpressed.
  */
uint8_t Button_State(void)
{
  uint8_t state = GPIO_ReadInputDataBit(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN);
  
  switch(Button_Flag) {
    case 0: 
      if (state) {
        Button_Flag = 1;
        Button_TimerBack = LocalTime;
      }
      state = 0;
      break;
    case 1:
      if ((state) && ((LocalTime - Button_TimerBack) >= 40)) {
        Button_Flag = 2;        
      } else {
        state = 0;
      }
      break;
    default:
      if (state == 0) {
        Button_Flag = 0;  
      }
      state = 0;
      break;
  }
  return state;
}
/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  

  /* wait until the desired delay finish */  
  while (timingdelay > LocalTime);
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}


/**
  * @brief  Eval Board Output Pin Initialize Function.
  * @param  None
  * @retval None
  */				  
void EVAL_LEDInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
												
	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}



PUTCHAR_PROTOTYPE
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3,ch);
	return ch;
}


/**
  * @brief  Initializes the STM324xG-EVAL's LCD, LEDs and push-buttons resources.
  * @param  None
  * @retval None
  */
void LCD_LED_BUTTON_Init(void)
{
#ifdef USE_LCD
  /* Initialize the STM324xG-EVAL's LCD */
  STM32f4_Discovery_LCD_Init();
#endif
 
#ifdef USE_LCD
  /* Clear the LCD */
  LCD_Clear(Black);

  /* Set the LCD Back Color */
  LCD_SetBackColor(Black);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  /* Display message on the LCD*/
  LCD_DisplayStringLine(Line0, (uint8_t*)MESSAGE1);
  LCD_DisplayStringLine(Line1, (uint8_t*)MESSAGE2);
  LCD_DisplayStringLine(Line2, (uint8_t*)MESSAGE3);
  LCD_DisplayStringLine(Line3, (uint8_t*)MESSAGE4);  
#endif

  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/
