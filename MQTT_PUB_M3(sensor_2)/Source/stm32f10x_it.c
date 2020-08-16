/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */

#include "device_driver.h"

void NMI_Handler(void)
{
//  while (1) {}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1){}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
//  while (1) {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
//  while (1) {}
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
//  while (1) {}
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
//  while (1) {}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */

void WWDG_IRQHandler(void){}
void PVD_IRQHandler(void){}
void TAMPER_IRQHandler(void){}



void RTC_IRQHandler(void)
{
	
	
}
void FLASH_IRQHandler(void){}
void RCC_IRQHandler(void){}

void EXTI0_IRQHandler(void)
{

}
void EXTI1_IRQHandler(void)
{

}
void EXTI2_IRQHandler(void){}
void EXTI3_IRQHandler(void){}
void EXTI4_IRQHandler(void){}
void DMA1_Channel1_IRQHandler(void){}
void DMA1_Channel2_IRQHandler(void){}
void DMA1_Channel3_IRQHandler(void){}
void DMA1_Channel4_IRQHandler(void){}
void DMA1_Channel5_IRQHandler(void){}
void DMA1_Channel6_IRQHandler(void){}
void DMA1_Channel7_IRQHandler(void){}
void ADC1_2_IRQHandler(void){}
void USB_HP_CAN1_TX_IRQHandler(void){}
void USB_LP_CAN1_RX0_IRQHandler(void){}
void CAN1_RX1_IRQHandler(void){}
void CAN1_SCE_IRQHandler(void){}
void EXTI9_5_IRQHandler(void){}
void TIM1_BRK_IRQHandler(void){}
void TIM1_UP_IRQHandler(void){}
void TIM1_TRG_COM_IRQHandler(void){}
void TIM1_CC_IRQHandler(void){}

extern void Timer2_ISR(void);
void TIM2_IRQHandler(void)
{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		Timer2_ISR();
	}
}
void TIM3_IRQHandler(void){}



void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);		
	}
}
void I2C1_EV_IRQHandler(void){}
void I2C1_ER_IRQHandler(void){}
void I2C2_EV_IRQHandler(void){}
void I2C2_ER_IRQHandler(void){}
void SPI1_IRQHandler(void){}
void SPI2_IRQHandler(void){}

void USART1_IRQHandler(void)
{
	
}
void USART2_IRQHandler(void)
{

}
void USART3_IRQHandler(void){}

volatile int Key_Value = 0;
volatile int interlock = 0; // (추가)
volatile int Sensor_num = 0;

volatile int sensor[100];

void EXTI15_10_IRQHandler(void)
{
  Key_Value = Macro_Extract_Area(EXTI->PR, 0x7, 13);
  Sensor_num = Key_Value;       // (추가) main함수의 배열에서 사용
  
  if (sensor[Key_Value] == 0)   sensor[Key_Value] = 1;
  else if (sensor[Key_Value] == 1)      sensor[Key_Value] = 0;  // (이부분 코드 실행속도가 느리면 다른 인터럽트 인식못할수도..)
  
  EXTI->PR = 0x7<<13;
  NVIC_ClearPendingIRQ((IRQn_Type)40);
}
void RTCAlarm_IRQHandler(void){}
void USBWakeUp_IRQHandler(void){}






/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
