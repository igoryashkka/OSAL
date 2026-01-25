/**
 * Platform_UART_Read Implementation Guide
 * 
 * This file shows how to implement Platform_UART_Read() for different STM32 variants.
 * The Platform_API provides weak defaults; you override them in platform/stm32/impl/
 */

/* ============================================================================
 * STM32F1 (STM32F103) Implementation - Using LL Driver
 * Location: platform/stm32/impl/stm32f1/STM32F103x_Uart_API.c
 * ============================================================================ */

#if 0

#include "platform_uart.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx.h"

/* Simple circular buffer for UART RX (optional, for robustness) */
#define UART_RX_BUFFER_SIZE 256
typedef struct {
    uint8_t buffer[UART_RX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} uart_rx_buffer_t;

static uart_rx_buffer_t uart_rx_buffer = {0};

/**
 * USART2 RX Interrupt Handler
 * Assumed to be called from HAL/LL interrupt handler
 */
void UART_RxISR_Handler(void) {
    if (LL_USART_IsActiveFlag_RXNE(USART2)) {
        uint8_t byte = LL_USART_ReceiveData8(USART2);
        
        uint16_t next_head = (uart_rx_buffer.head + 1) % UART_RX_BUFFER_SIZE;
        if (next_head != uart_rx_buffer.tail) {
            uart_rx_buffer.buffer[uart_rx_buffer.head] = byte;
            uart_rx_buffer.head = next_head;
        }
        /* else: buffer full, drop byte */
    }
}

/**
 * Initialize UART (USART2 on PA2/PA3 for STM32F103)
 */
void Platform_UART_Init(uint32_t baud) {
    /* Enable GPIOA and USART2 clocks */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    
    /* GPIO Config: PA2 (TX), PA3 (RX) */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USART Config */
    LL_USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.BaudRate = baud;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.Direction = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    LL_USART_Init(USART2, &USART_InitStruct);
    
    /* Enable RX interrupt */
    LL_USART_EnableIT_RXNE(USART2);
    NVIC_EnableIRQ(USART2_IRQn);
    
    LL_USART_Enable(USART2);
}

/**
 * Write data to UART (blocking)
 */
int Platform_UART_Write(const uint8_t* data, int len) {
    for (int i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART2));
        LL_USART_TransmitData8(USART2, data[i]);
    }
    return len;
}

/**
 * Read data from RX buffer (non-blocking)
 */
int Platform_UART_Read(uint8_t* buffer, int max_len) {
    int count = 0;
    
    while (count < max_len && uart_rx_buffer.tail != uart_rx_buffer.head) {
        buffer[count++] = uart_rx_buffer.buffer[uart_rx_buffer.tail];
        uart_rx_buffer.tail = (uart_rx_buffer.tail + 1) % UART_RX_BUFFER_SIZE;
    }
    
    return count;
}

#endif

/* ============================================================================
 * STM32H7 (STM32H750) Implementation - Using HAL Driver
 * Location: platform/stm32/impl/stm32h7/STM32H750x_Uart_API.c
 * ============================================================================ */

#if 0

#include "platform_uart.h"
#include "stm32h7xx_hal.h"

extern UART_HandleTypeDef huart3;  /* Assuming UART3 is used for GPS */

#define GPS_RX_BUFFER_SIZE 256
typedef struct {
    uint8_t buffer[GPS_RX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} uart_rx_buffer_t;

static uart_rx_buffer_t uart_rx_buffer = {0};

/**
 * UART RX Complete Callback (from HAL)
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == UART3) {
        uint8_t byte = huart->pRxBuffPtr[0];
        
        uint16_t next_head = (uart_rx_buffer.head + 1) % GPS_RX_BUFFER_SIZE;
        if (next_head != uart_rx_buffer.tail) {
            uart_rx_buffer.buffer[uart_rx_buffer.head] = byte;
            uart_rx_buffer.head = next_head;
        }
    }
}

/**
 * Initialize UART (UART3 on PB10/PB11 for STM32H750)
 */
void Platform_UART_Init(uint32_t baud) {
    /* Configure UART3 */
    huart3.Instance = UART3;
    huart3.Init.BaudRate = baud;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_UART_Init(&huart3);
    
    /* Enable RX with interrupt */
    HAL_UART_Receive_IT(&huart3, (uint8_t*)&uart_rx_buffer.buffer[0], 1);
}

/**
 * Write data to UART (blocking)
 */
int Platform_UART_Write(const uint8_t* data, int len) {
    HAL_UART_Transmit(&huart3, (uint8_t*)data, len, HAL_MAX_DELAY);
    return len;
}

/**
 * Read data from RX buffer (non-blocking)
 */
int Platform_UART_Read(uint8_t* buffer, int max_len) {
    int count = 0;
    
    while (count < max_len && uart_rx_buffer.tail != uart_rx_buffer.head) {
        buffer[count++] = uart_rx_buffer.buffer[uart_rx_buffer.tail];
        uart_rx_buffer.tail = (uart_rx_buffer.tail + 1) % GPS_RX_BUFFER_SIZE;
    }
    
    return count;
}

#endif

/* ============================================================================
 * Simple Polling Implementation (No Buffering)
 * Simplest option, lowest memory overhead
 * ============================================================================ */

#if 0

#include "platform_uart.h"
#include "stm32f1xx_ll_usart.h"

void Platform_UART_Init(uint32_t baud) {
    /* Configure GPIO and USART for your specific STM32 */
    /* See LL examples in STM32 HAL documentation */
}

int Platform_UART_Write(const uint8_t* data, int len) {
    for (int i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART2));
        LL_USART_TransmitData8(USART2, data[i]);
    }
    return len;
}

/**
 * Directly read from UART without buffering
 * Note: Will only return bytes currently available; may lose data in fast traffic
 */
int Platform_UART_Read(uint8_t* buffer, int max_len) {
    int count = 0;
    while (count < max_len && LL_USART_IsActiveFlag_RXNE(USART2)) {
        buffer[count++] = LL_USART_ReceiveData8(USART2);
    }
    return count;
}

#endif

/* ============================================================================
 * Recommended Implementation Notes
 * ============================================================================ */

/*
 * OPTION 1: Buffered + Interrupt (RECOMMENDED for GPS)
 * - Pros: Reliable, no data loss, handles slow polling
 * - Cons: More RAM, more complex
 * - Best for: High-frequency or variable-latency applications
 * - Memory: ~256 bytes RX buffer + small overhead
 *
 * OPTION 2: Direct Polling (SIMPLE)
 * - Pros: Minimal code, no interrupts, no buffering
 * - Cons: May lose data if polling interval is too long
 * - Best for: Tight main loop that reads constantly
 * - Memory: Minimal
 *
 * OPTION 3: DMA + Circular Buffer (HIGH PERFORMANCE)
 * - Pros: No CPU overhead, very reliable
 * - Cons: More complex DMA setup
 * - Best for: Other time-critical serial communication
 *
 * GPS Notes:
 * - NEO-6M sends ~180 bytes/second at 1Hz update rate (9600 baud)
 * - Buffering is recommended to avoid data loss
 * - Interrupt-driven approach is ideal for battery life
 * 
 * Testing:
 * 1. Connect GPS module with UART
 * 2. Call GPS_Init(9600)
 * 3. Read loop: GPS_Read() -> GPS_ProcessByte() for each byte
 * 4. Check GPS_GetStats() for debug info
 * 5. Verify GPS_GetPosition() returns valid coordinates
 */
