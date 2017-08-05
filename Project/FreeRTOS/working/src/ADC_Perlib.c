


#include "lwip/sys.h"
#include "lwip/api.h"
#include "ADC_Perlib.h"
#include "util_Perlib.h"
#include "string.h"    
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

                        
#define ADC_THREAD_PRIO  ( tskIDLE_PRIORITY)    
#define emacBLOCK_TIME_WAITING_FOR_INPUT	( ( portTickType ) 100 )

volatile uint32_t ADC_ConversionValue[3]= {0};

extern uint16_t Audio_Data;
extern uint16_t Freq_Data;

extern uint16_t AUDIO_SAMPLE[];
void (*ADC_EOC_Event)(void) = 0;


xSemaphoreHandle s_xSemaphore_ADC = NULL;
uint8_t flag_ADC_EOC = 0;

char str[20];


uint32_t adcValue=0;
uint32_t adcVoltage=0;



//void ADC_Configuration(void)
//{				
//	GPIO_InitTypeDef GPIO_InitStructure;   
//	DMA_InitTypeDef DMA_InitStructure;	   	 
//	ADC_CommonInitTypeDef ADC_CommonInitStructure;
//	ADC_InitTypeDef ADC_InitStructure;					 
//	NVIC_InitTypeDef NVIC_InitStructure;

//						 
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
// 	
//	DMA_DeInit(DMA2_Stream0);
//	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_ConversionValue;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_BufferSize = 0xFFF;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
//	DMA_Cmd(DMA2_Stream0, ENABLE);


//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	

//	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
//	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
//	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
//	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_15Cycles;
//	ADC_CommonInit(&ADC_CommonInitStructure);

//	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
//	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_NbrOfConversion = 1;
//	ADC_Init(ADC1, &ADC_InitStructure);

///*
//	1	PA4		ADC1_IN4
//	2	PA5		ADC1_IN5
//	3	PA6		ADC1_IN6	
//*/

//	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_15Cycles);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_15Cycles);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_15Cycles);



//	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
//	
//	/* Enable ADC1 DMA */
//	ADC_DMACmd(ADC1, ENABLE);
//	
//	/* Enable ADC1 */
//	ADC_Cmd(ADC1, ENABLE);


//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 
//	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
//	NVIC_Init(&NVIC_InitStructure);	

//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

//}


void ADC_Configuration(void)
{
  
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	

	
       	//Enable ADC1 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//Enable the ADC1 GPIO Clock	
      	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 

	//Initializing GPIO for ADC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//Initializing GPIO 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//ADC Common Init

	//Use Independent mode
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	//ADC1 Init
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	//Enable ADC1
	ADC_Cmd(ADC1, ENABLE);
}


void ADC_Set_EOC_Event(void (*func)(void))
{
	ADC_EOC_Event = func;
}

uint16_t ADC_GetRawValue(uint8_t ch)
{
	return ADC_ConversionValue[ch];
}




//Task to handle ADC conversion
static void AdcTask(void *pvParameters)
{
	uint32_t cnt;
	//ADC1 regular channel16 configuration
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_15Cycles);

	cnt = 0;
	while(1)
	{
        cnt++;
		if(cnt > 2000000)
		{
			//Start ADC1 Conversion
			ADC_SoftwareStartConv(ADC1);

			//NOTE :: Make sure that decimals value is being multipied with adcValue
			adcValue= ADC_GetConversionValue(ADC1);
			 sprintf(str, "    %d" , adcValue);
			LCD_LINE_Control(Line4, str);
			cnt = 0;
		}

	}

}


//static void ADC_thread(void *arg)
//{
//	
//	ADC_Configuration();
//	if (s_xSemaphore_ADC == NULL)
//	{
//		s_xSemaphore_ADC= xSemaphoreCreateCounting(20,0);
//	}
//	while(1)
//	{   
//		if(flag_ADC_EOC)
//		{			
//			sprintf(str, "%d  %d  %d", ADC_ConversionValue[0],ADC_ConversionValue[1],ADC_ConversionValue[2]);
//			LCD_LINE_Control(Line4, str);
//			flag_ADC_EOC = 0;
//		}
//	
//	}
//}




void ADC_Task_init(void)
{
  sys_thread_new("ADC_thread", AdcTask, NULL, DEFAULT_THREAD_STACKSIZE, ADC_THREAD_PRIO);
}


			
void ADC_IRQHandler(void)
{	
//	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;


//  /* Frame received */
//	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
//	{
//		xSemaphoreGiveFromISR(s_xSemaphore_ADC, &xHigherPriorityTaskWoken );
//	}
//	
//	sprintf(str, "%d  %d  %d", ADC_ConversionValue[0],ADC_ConversionValue[1],ADC_ConversionValue[2]);
//	LCD_LINE_Control(Line4, str);
//	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
//	
//	              
//	/* Switch tasks if necessary. */
//	if ( xHigherPriorityTaskWoken != pdFALSE ) 
//	{
//		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
//	}
	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
	{
		flag_ADC_EOC = 1;
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}



