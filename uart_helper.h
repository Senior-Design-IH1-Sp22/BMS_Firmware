/*
 * uart_helper.h
 */

#ifndef UART_HELPER_H_
#define UART_HELPER_H_

#define UART_ESP_BASE SCIB_BASE

void UART_Init(void);
void UART_InitInterrupt(void);
void UART_ResetRxBuffer(void);
void UART_PrintRxBuffer(void);
void UART_TransmitCOM(char* string);
void UART_TransmitESP(char* string);
void UART_SendCommand(char* string);


#endif /* UART_HELPER_H_ */
