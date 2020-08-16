#include "device_driver.h"

void LED_Init(void)
{
  Peri_Set_Bit(RCC->APB2ENR, 3);
  Macro_Write_Block(GPIOB->CRH, 0xf, 0x6, 0);
  GPIO_Lock_Multi(B, 0x1<<8);
  GPIO_Set_Area(B, 0x1, 8);
}

void LED_Display(unsigned int num)
{
  GPIO_Write_Block(B, 0x1, (~num & 1), 8);
}

void LED_On(void)
{
  GPIO_Clear_Area(B, 0x1, 8);
}

void LED_Off(void)
{
  GPIO_Set_Area(B, 0x1, 8);
}
