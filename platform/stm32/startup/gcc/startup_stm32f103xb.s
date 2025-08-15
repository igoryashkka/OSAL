/* Minimal GCC startup for STM32F103xB */
.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

/* Symbols from linker script */
.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss
.extern _estack

.global Reset_Handler
.global Default_Handler

/* Vector table */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors
g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word Default_Handler /* NMI */
  .word Default_Handler /* HardFault */
  .word Default_Handler /* MemManage */
  .word Default_Handler /* BusFault */
  .word Default_Handler /* UsageFault */
  .word 0,0,0,0         /* Reserved */
  .word Default_Handler /* SVC */
  .word Default_Handler /* DebugMon */
  .word 0                /* Reserved */
  .word Default_Handler /* PendSV */
  .word Default_Handler /* SysTick */

  /* External IRQs (fill as needed) */
  .word Default_Handler /* WWDG */
  .word Default_Handler /* PVD */
  .word Default_Handler /* TAMPER */
  .word Default_Handler /* RTC */
  .word Default_Handler /* FLASH */
  .word Default_Handler /* RCC */
  .word Default_Handler /* EXTI0 */
  .word Default_Handler /* EXTI1 */
  .word Default_Handler /* EXTI2 */
  .word Default_Handler /* EXTI3 */
  .word Default_Handler /* EXTI4 */
  .word Default_Handler /* DMA1_Channel1 */
  .word Default_Handler /* DMA1_Channel2 */
  .word Default_Handler /* DMA1_Channel3 */
  .word Default_Handler /* DMA1_Channel4 */
  .word Default_Handler /* DMA1_Channel5 */
  .word Default_Handler /* DMA1_Channel6 */
  .word Default_Handler /* DMA1_Channel7 */
  .word Default_Handler /* ADC1_2 */
  .word Default_Handler /* USB_HP_CAN_TX */
  .word Default_Handler /* USB_LP_CAN_RX0 */
  .word Default_Handler /* CAN_RX1 */
  .word Default_Handler /* CAN_SCE */
  .word Default_Handler /* EXTI9_5 */
  .word Default_Handler /* TIM1_BRK */
  .word Default_Handler /* TIM1_UP */
  .word Default_Handler /* TIM1_TRG_COM */
  .word Default_Handler /* TIM1_CC */
  .word Default_Handler /* TIM2 */
  .word Default_Handler /* TIM3 */
  .word Default_Handler /* TIM4 */
  .word Default_Handler /* I2C1_EV */
  .word Default_Handler /* I2C1_ER */
  .word Default_Handler /* I2C2_EV */
  .word Default_Handler /* I2C2_ER */
  .word Default_Handler /* SPI1 */
  .word Default_Handler /* SPI2 */
  .word Default_Handler /* USART1 */
  .word Default_Handler /* USART2 */
  .word Default_Handler /* USART3 */
  .word Default_Handler /* EXTI15_10 */
  .word Default_Handler /* RTCAlarm */
  .word Default_Handler /* USBWakeUp */
  .word Default_Handler /* Reserved */
  .word Default_Handler /* Reserved */
  .word Default_Handler /* Reserved */
  .word Default_Handler /* Reserved */
  .word Default_Handler /* Reserved */
  .word Default_Handler /* Reserved */
  .word Default_Handler /* BootRAM */

.section .text.Reset_Handler,"ax",%progbits
.type Reset_Handler, %function
Reset_Handler:
  /* Copy .data from flash to RAM */
  ldr r0, =_sidata
  ldr r1, =_sdata
  ldr r2, =_edata
1:
  cmp r1, r2
  ittt lt
  ldrlt r3, [r0], #4
  strlt r3, [r1], #4
  blt 1b

  /* Zero .bss */
  ldr r0, =_sbss
  ldr r1, =_ebss
  movs r2, #0
2:
  cmp r0, r1
  itt lt
  strlt r2, [r0], #4
  blt 2b

  /* Call SystemInit and main */
  bl SystemInit
  bl main

  /* If main returns, loop */
3: b 3b

.size Reset_Handler, .-Reset_Handler

.section .text.Default_Handler,"ax",%progbits
.type Default_Handler, %function
Default_Handler:
  b .
.size Default_Handler, .-Default_Handler
