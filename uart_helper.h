/*
 * uart_helper.h
 *
 */

#ifndef UART_HELPER_H_
#define UART_HELPER_H_

#define AT_RESET        "AT+RST\r\n"
#define AT_Test         "AT\r\n"
#define AT_Version      "AT+GMR\r\n"
#define AT_Disable_Echo "ATE0\r\n"
#define AT_SET_WIFI_AP  "AT+CWMODE=2\r\n"
#define AT_AP_CONFIG    "AT+CWSAP=\"SSTWifi\",\"needfellowship!\",5,0\r\n"
#define AT_DHCP_EN      "AT+CWDHCP=0,0\r\n"
#define AT_MULTI_EN     "AT+CIPMUX=1\r\n"
#define AT_SERVER_CONFIG "AT+CIPSERVER=1\r\n"
#define AT_SAP_IP_GET   "AT+CIPAP?\r\n"
#define AT_SAP_IP_SET   "AT+CIPAP=\"192.168.4.1\"\r\n"
#define AT_TRANSFER_NORMAL "AT+CIPMODE=0\r\n"
#define AT_LIST_CLIENTS "AT+CWLIF\r\n"
#define AT_LONG_TIMEOUT "AT+CIPSTO=0\r\n"

void ESP_EnableRXInts(void);
void ESP_DisableRXInts(void);
void UART_init(void);
void UART_initInterrupt(void);
void UART_resetRxBuffer(void);
void UART_printRxBuffer(void);
void ESP_SendCommand(char* str);
void ESP_OutCharArr(char* str, int len);
void ESP_OutString(char* str);
void UART_transmitString(char* string);
void UART_transmitPlain(char* string);
void UART_sendCommand(char* string);


#endif /* UART_HELPER_H_ */
