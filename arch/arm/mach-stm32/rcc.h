#ifndef __STM32_RCC_H__
#define __STM32_RCC_H__

#define STM32_RCC_RST       0x10
#define STM32_RCC_CLK       0x30
#define STM32_RCC_LPCLK     0x50

#define STM32_RCC_AHB1      0x0
#define STM32_RCC_AHB2      0x4
#define STM32_RCC_AHB3      0x8
#define STM32_RCC_APB1      0x10
#define STM32_RCC_APB2      0x14

#define RCC_PERIPHERAL(operation, bus, device) \
            (operation << 16 | bus << 8 | device)

/* Reset */
#define STM32_RST_GPIOA     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 0)
#define STM32_RST_GPIOB     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 1)
#define STM32_RST_GPIOC     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 2)
#define STM32_RST_GPIOD     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 3)
#define STM32_RST_GPIOE     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 4)
#define STM32_RST_GPIOF     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 5)
#define STM32_RST_GPIOG     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 6)
#define STM32_RST_GPIOH     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 7)
#define STM32_RST_GPIOI     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 8)
#define STM32_RST_CRC       RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 12)
#define STM32_RST_DMA1      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 21)
#define STM32_RST_DMA2      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 22)
#define STM32_RST_ETHMAC    RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 25)
#define STM32_RST_OTGHS     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB1, 29)

#define STM32_RST_DCMI      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB2, 0)
#define STM32_RST_CRYP      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB2, 4)
#define STM32_RST_HASH      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB2, 5)
#define STM32_RST_RNG       RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB2, 6)
#define STM32_RST_OTGF      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB2, 7)

#define STM32_RST_FSMC      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_AHB3, 0)

#define STM32_RST_TIM2      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 0)
#define STM32_RST_TIM3      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 1)
#define STM32_RST_TIM4      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 2)
#define STM32_RST_TIM5      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 3)
#define STM32_RST_TIM6      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 4)
#define STM32_RST_TIM7      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 5)
#define STM32_RST_TIM12     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 6)
#define STM32_RST_TIM13     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 7)
#define STM32_RST_TIM14     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 8)
#define STM32_RST_WWDG      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 11)
#define STM32_RST_SPI2      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 14)
#define STM32_RST_SPI3      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 15)
#define STM32_RST_UART2     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 17)
#define STM32_RST_UART3     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 18)
#define STM32_RST_UART4     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 19)
#define STM32_RST_UART5     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 20)
#define STM32_RST_I2C1      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 21)
#define STM32_RST_I2C2      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 22)
#define STM32_RST_I2C3      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 23)
#define STM32_RST_CAN1      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 25)
#define STM32_RST_CAN2      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 26)
#define STM32_RST_PWR       RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 28)
#define STM32_RST_DAC       RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB1, 29)

#define STM32_RST_TIM1      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 0)
#define STM32_RST_TIM8      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 1)
#define STM32_RST_USART1    RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 4)
#define STM32_RST_USART6    RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 5)
#define STM32_RST_ADC       RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 8)
#define STM32_RST_SDIO      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 11)
#define STM32_RST_SPI1      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 12)
#define STM32_RST_SYSCFG    RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 14)
#define STM32_RST_TIM9      RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 16)
#define STM32_RST_TIM10     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 17)
#define STM32_RST_TIM11     RCC_PERIPHERAL(STM32_RCC_RST, STM32_RCC_APB2, 18)

/* Clock */
#define STM32_CLK_GPIOA     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 0)
#define STM32_CLK_GPIOB     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 1)
#define STM32_CLK_GPIOC     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 2)
#define STM32_CLK_GPIOD     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 3)
#define STM32_CLK_GPIOE     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 4)
#define STM32_CLK_GPIOF     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 5)
#define STM32_CLK_GPIOG     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 6)
#define STM32_CLK_GPIOH     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 7)
#define STM32_CLK_GPIOI     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 8)
#define STM32_CLK_CRC       RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 12)
#define STM32_CLK_DMA1      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 21)
#define STM32_CLK_DMA2      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 22)
#define STM32_CLK_ETHMAC    RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 25)
#define STM32_CLK_OTGHS     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB1, 29)

#define STM32_CLK_DCMI      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB2, 0)
#define STM32_CLK_CRYP      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB2, 4)
#define STM32_CLK_HASH      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB2, 5)
#define STM32_CLK_RNG       RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB2, 6)
#define STM32_CLK_OTGF      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB2, 7)

#define STM32_CLK_FSMC      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_AHB3, 0)

#define STM32_CLK_TIM2      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 0)
#define STM32_CLK_TIM3      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 1)
#define STM32_CLK_TIM4      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 2)
#define STM32_CLK_TIM5      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 3)
#define STM32_CLK_TIM6      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 4)
#define STM32_CLK_TIM7      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 5)
#define STM32_CLK_TIM12     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 6)
#define STM32_CLK_TIM13     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 7)
#define STM32_CLK_TIM14     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 8)
#define STM32_CLK_WWDG      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 11)
#define STM32_CLK_SPI2      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 14)
#define STM32_CLK_SPI3      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 15)
#define STM32_CLK_UART2     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 17)
#define STM32_CLK_UART3     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 18)
#define STM32_CLK_UART4     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 19)
#define STM32_CLK_UART5     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 20)
#define STM32_CLK_I2C1      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 21)
#define STM32_CLK_I2C2      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 22)
#define STM32_CLK_I2C3      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 23)
#define STM32_CLK_CAN1      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 25)
#define STM32_CLK_CAN2      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 26)
#define STM32_CLK_PWR       RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 28)
#define STM32_CLK_DAC       RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB1, 29)

#define STM32_CLK_TIM1      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 0)
#define STM32_CLK_TIM8      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 1)
#define STM32_CLK_USART1    RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 4)
#define STM32_CLK_USART6    RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 5)
#define STM32_CLK_ADC       RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 8)
#define STM32_CLK_SDIO      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 11)
#define STM32_CLK_SPI1      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 12)
#define STM32_CLK_SYSCFG    RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 14)
#define STM32_CLK_TIM9      RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 16)
#define STM32_CLK_TIM10     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 17)
#define STM32_CLK_TIM11     RCC_PERIPHERAL(STM32_RCC_CLK, STM32_RCC_APB2, 18)

void stm32_clk_enable(unsigned clock);
void stm32_clk_disable(unsigned clock);
void stm32_reset(unsigned rst);

#endif /* __STM32_RCC_H__ */

