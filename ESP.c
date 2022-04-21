/*
 * ESP.c
 */

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include <string.h>
#include <stdint.h>
#include "uart_helper.h"
#include "ESP.h"


void ESP_Init(void) {
    ESP_DisableRXInts();
    ESP_SendCommand(AT_Test);
    ESP_SendCommand(AT_Disable_Echo);       // Disable UART Echo
    ESP_SendCommand(AT_SET_WIFI_AP);        // Config as Access Point
    ESP_SendCommand(AT_AP_CONFIG);          // Set SSID and Password
//    DEVICE_DELAY_US(20000);
    ESP_SendCommand(AT_AP_QUERY);
    ESP_SendCommand(AT_TRANSFER_NORMAL);    // Enable Transparent Transmission AT_TRANSFER_NORMAL
    ESP_SendCommand(AT_DHCP_EN);            // Enable DHCP Server
    ESP_SendCommand(AT_DHCP_QUERY);
    ESP_SendCommand(AT_SAP_IP_SET);         // Set Server IP to 192.168.4.1
    ESP_SendCommand(AT_MULTI_EN);           // Enable Multiple Connections
    ESP_SendCommand(AT_MULTI_QUERY);
    ESP_SendCommand(AT_SERVER_CONFIG);      // Enable Server on Port 333
    ESP_SendCommand(AT_SERVER_QUERY);
    ESP_SendCommand(AT_LONG_TIMEOUT);       // 2 hour timeout so it doesn't DC.

//    // Configure ESP
//    ESP_SendCommand(AT_Disable_Echo);       // Disable UART Echo
//    ESP_SendCommand(AT_SET_WIFI_AP);        // Config as Access Point
//    ESP_SendCommand(AT_AP_CONFIG);          // Set SSID and Password
//    ESP_SendCommand(AT_DHCP_EN);            // Enable DHCP Server
//    ESP_SendCommand(AT_SAP_IP_SET);         // Set Server IP to 192.168.4.1
//    ESP_SendCommand(AT_MULTI_EN);           // Enable Multiple Connections
//    ESP_SendCommand(AT_SERVER_CONFIG);      // Enable Server on Port 333
//    ESP_SendCommand(AT_TRANSFER_NORMAL);    // Enable Transparent Transmission
//    ESP_SendCommand(AT_LONG_TIMEOUT);       // 2 hour timeout so it doesn't DC.


    ESP_EnableRXInts();
}

void ESP_EnableRXInts(void) {
    while(SCI_getRxFIFOStatus(UART_ESP_BASE)) {
        SCI_readCharBlockingFIFO(UART_ESP_BASE);
    }
    SCI_clearInterruptStatus(UART_ESP_BASE, SCI_INT_RXRDY_BRKDT);
    SCI_enableInterrupt(UART_ESP_BASE, SCI_INT_RXRDY_BRKDT);
}

void ESP_DisableRXInts(void) {
    while(SCI_getRxFIFOStatus(UART_ESP_BASE)) {
        SCI_readCharBlockingFIFO(UART_ESP_BASE);
    }
    SCI_clearInterruptStatus(UART_ESP_BASE, SCI_INT_RXRDY_BRKDT);
    SCI_disableInterrupt(UART_ESP_BASE, SCI_INT_RXRDY_BRKDT);
}

void ESP_SendCommand(char* command) {
    UART_TransmitESP(command);
    DEVICE_DELAY_US(10000);
    SCI_readCharBlockingFIFO(UART_ESP_BASE);
    while (SCI_getRxFIFOStatus(UART_ESP_BASE)) {
        SCI_readCharBlockingFIFO(UART_ESP_BASE);
        DEVICE_DELAY_US(100);
    }
}

void ESP_WifiSendChar(char c) {
    char str[2] = "\0";
    str[0] = c;
    UART_TransmitESP(AT_CIPSEND_1);
    DEVICE_DELAY_US(10000);
    SCI_readCharBlockingFIFO(UART_ESP_BASE);
    while (SCI_getRxFIFOStatus(UART_ESP_BASE)) {
        SCI_readCharBlockingFIFO(UART_ESP_BASE);
        DEVICE_DELAY_US(100);
    }
    UART_TransmitESP(str);
}

void ESP_WifiSendOneAtaTime(char* str, int len) {
    int i;
    for (i = 0; i < len; i++) {
        char msg[2] = "\0";
        msg[0] = str[i];
        UART_TransmitESP(AT_CIPSEND_1);
        DEVICE_DELAY_US(10000);
        SCI_readCharBlockingFIFO(UART_ESP_BASE);
        while (SCI_getRxFIFOStatus(UART_ESP_BASE)) {
            SCI_readCharBlockingFIFO(UART_ESP_BASE);
            DEVICE_DELAY_US(100);
        }
        UART_TransmitESP(msg);
        DEVICE_DELAY_US(2000);
    }
}

void ESP_WifiSendString(char* str, int len) {
    // If /r or /n is in str, send one at a time
//    int i;
//    for (i = 0; i < len; i++) {
//        if (str[i] == '\r' || str[i] == '\n') {
//            ESP_WifiSendOneAtaTime(str, len);
//            return;
//        }
//    }
    // Otherwise send multiple characters using one CIPSEND command
    char cmd[] = "AT+CIPSEND=0,__\r\n";
    cmd[13] = len/10 + '0';
    cmd[14] = len%10 + '0';
    UART_TransmitESP(cmd);
    DEVICE_DELAY_US(10000);
    SCI_readCharBlockingFIFO(UART_ESP_BASE);
    while (SCI_getRxFIFOStatus(UART_ESP_BASE)) {
        SCI_readCharBlockingFIFO(UART_ESP_BASE);
        DEVICE_DELAY_US(100);
    }
    UART_TransmitESP(str);
}
