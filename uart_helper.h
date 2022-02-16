/*
 * uart_helper.h
 *
 */

#ifndef UART_HELPER_H_
#define UART_HELPER_H_

void UART_init(void);
void UART_initInterrupt(void);
void UART_transmitString(char* string);
void UART_transmitPlain(char* string);


#endif /* UART_HELPER_H_ */
