//#############################################################################
//
// FILE:   sci_ex2_interrupts.c
//
// TITLE:  SCI interrupt echoback example.
//
//! \addtogroup driver_example_list
//! <h1>SCI Interrupt Echoback</h1>
//!
//!  This test receives and echo-backs data through the SCI-A port
//!  via interrupts.
//!
//!  A terminal such as 'putty' can be used to view the data from
//!  the SCI and to send information to the SCI. Characters received
//!  by the SCI port are sent back to the host.
//!
//!  Running the Application
//!  Open a COM port with the following settings using a terminal:
//!  -  Find correct COM port
//!  -  Bits per second = 9600
//!  -  Data Bits = 8
//!  -  Parity = None
//!  -  Stop Bits = 1
//!  -  Hardware Control = None
//!
//!  The program will print out a greeting and then ask you to
//!  enter a character which it will echo back to the terminal.
//!
//!  Watch Variables
//!  - counter - the number of characters sent
//!
//! External Connections
//!  Connect the SCI-A port to a PC via a transceiver and cable.
//!  - GPIO28 is SCI_A-RXD (Connect to Pin3, PC-TX, of serial DB9 cable)
//!  - GPIO29 is SCI_A-TXD (Connect to Pin2, PC-RX, of serial DB9 cable)
//
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include <string.h>
#include <stdint.h>
#include "uart_helper.h"

#ifdef _FLASH
// These are defined by the linker (see device linker command file)
extern uint16_t RamfuncsLoadStart;
extern uint16_t RamfuncsLoadSize;
extern uint16_t RamfuncsRunStart;
#endif

//
// Defines
//
// Define AUTOBAUD to use the autobaud lock feature
//#define AUTOBAUD
#define ESP_RX_FIFO_LENGTH  12
#define ESP_RX_FIFO_WIDTH   48
#define ESP_RX_BUFFER_LENGTH 1000

//
// Globals
//
uint16_t counter = 0;
unsigned char *msg;

//
// Function Prototypes
//
__interrupt void scibTxISR(void);
__interrupt void scibRxISR(void);

char UART_TxBuffer[100];
uint16_t UART_RxBuffer[100];
uint16_t UART_RxIndex = 0;

volatile char ESP_RX_BUFFER[ESP_RX_BUFFER_LENGTH];

volatile char CONNECTION_ID;

volatile char ESP_RX_FIFO[ESP_RX_FIFO_LENGTH][ESP_RX_FIFO_WIDTH];
volatile int ESP_RX_FIFO_PutI = 0, ESP_RX_FIFO_GetI = 0;

void ESP_EnableRXInts(void) {
    while(SCI_getRxStatus(SCIB_BASE) & SCI_RXSTATUS_READY) {
        SCI_readCharNonBlocking(SCIB_BASE);
    }
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXRDY_BRKDT);
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_RXRDY_BRKDT);
}

void ESP_DisableRXInts(void) {
    while(SCI_getRxStatus(SCIB_BASE) & SCI_RXSTATUS_READY) {
        SCI_readCharNonBlocking(SCIB_BASE);
    }
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXRDY_BRKDT);
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXRDY_BRKDT);
}

void UART_resetRxBuffer(void)
{
    UART_RxIndex = 0;
    int i = 0;
    for (; i < 100; i++) {
        UART_RxBuffer[i] = '\0';
    }
}

void UART_printRxBuffer(void)
{
    int i = 0;
    for (; (i < 100) && (UART_RxBuffer[i] != '\0'); i++) {
        SCI_writeCharBlockingFIFO(SCIA_BASE, UART_RxBuffer[i]);
    }
}

void UART_transmitString(char* string)
{
    strcpy(UART_TxBuffer, string);
    strcat(UART_TxBuffer, "\r\n");
    SCI_writeCharArray(SCIA_BASE, (uint16_t*)UART_TxBuffer, strlen(UART_TxBuffer)+1);
}



void ESP_SendCommand(char* command) {
    ESP_OutString(command);
    SCI_readCharBlockingFIFO(SCIB_BASE);
    while (SCI_getRxStatus(SCIB_BASE) & SCI_RXSTATUS_READY) {
        SCI_readCharNonBlocking(SCIB_BASE);
    }
    DEVICE_DELAY_US(2000000);
}

void ESP_OutString(char* str) {
    int i=0;
    while(str[i]) {
        SCI_writeCharBlockingNonFIFO(SCIB_BASE, str[i++]);
    }
}

void ESP_OutCharArr(char* str, int len) {
    int i = 0;
    for (; i<len; i++) {
        SCI_writeCharBlockingNonFIFO(SCIB_BASE, str[i++]);
    }
}

void UART_sendCommand(char* string)
{
    strcpy(UART_TxBuffer, string);
    strcat(UART_TxBuffer, "\r\n");
    SCI_writeCharArray(SCIB_BASE, (uint16_t*)UART_TxBuffer, strlen(UART_TxBuffer));
}

void UART_transmitPlain(char* string)
{
    strcpy(UART_TxBuffer, string);
    SCI_writeCharArray(SCIA_BASE, (uint16_t*)UART_TxBuffer, strlen(UART_TxBuffer)+1);
}

void ESP_Init(void)
{
    ESP_DisableRXInts();

    // Configure ESP
//    ESP_SendCommand("AT\r\n");        // Config as Access Point
//    ESP_SendCommand(AT_Disable_Echo);       // Disable UART Echo
    ESP_SendCommand(AT_SET_WIFI_AP);        // Config as Access Point
    ESP_SendCommand(AT_AP_CONFIG);          // Set SSID and Password
    ESP_SendCommand(AT_DHCP_EN);            // Enable DHCP Server
    ESP_SendCommand(AT_SAP_IP_SET);         // Set Server IP to 192.168.4.1
    ESP_SendCommand(AT_MULTI_EN);           // Enable Multiple Connections
    ESP_SendCommand(AT_SERVER_CONFIG);      // Enable Server on Port 333
    ESP_SendCommand(AT_TRANSFER_NORMAL);    // Enable Transparent Transmission
    ESP_SendCommand(AT_LONG_TIMEOUT);       // 2 hour timeout so it doesn't DC.

    ESP_EnableRXInts();
}

void UART_init(void)
{
    // GPIO28 is the SCIA Rx pin.
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO29 is the SCIA Tx pin.
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

    // GPIO13 is the SCIB Rx pin.
    GPIO_setMasterCore(13, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_13_SCIB_RX);
    GPIO_setDirectionMode(13, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(13, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(13, GPIO_QUAL_ASYNC);

    // GPIO40 is the SCIB Tx pin.
    GPIO_setMasterCore(40, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_40_SCIB_TX);
    GPIO_setDirectionMode(40, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(40, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(40, GPIO_QUAL_ASYNC);

    // Map the ISR to the wake interrupt.
    Interrupt_register(INT_SCIA_RX, scibRxISR);

    // Initialize SCIA, SCIB, and their FIFO.
    SCI_performSoftwareReset(SCIA_BASE);
    SCI_performSoftwareReset(SCIB_BASE);

    // Configure SCIA for COM
    SCI_setConfig(SCIA_BASE, 25000000, 9600, (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIA_BASE);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);

    // Configure SCIB for ESP
    SCI_setConfig(SCIB_BASE, 25000000, 115200, (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);
    SCI_resetTxFIFO(SCIB_BASE);
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);
    SCI_enableFIFO(SCIB_BASE);
    SCI_enableModule(SCIB_BASE);
    SCI_performSoftwareReset(SCIB_BASE);

    // Enable the RXRDY interrupt
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_RXRDY_BRKDT);

    // Send starting message.
    UART_transmitString("\n\nUART Initialized!\n");

    // Clear the SCI interrupts before enabling them.
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXRDY_BRKDT);

    // Enable the interrupts in the PIE: Group 9 interrupts 1 & 2.
    Interrupt_enable(INT_SCIB_RX);
//    Interrupt_enable(INT_SCIB_TX);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    ESP_Init();

}

// ======================================================================================
//              ESP



//
// sciaTxISR - Disable the TXRDY interrupt and print message asking
//             for a character.
//
__interrupt void
scibTxISR(void)
{
    // Disable the TXRDY interrupt.
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_TXRDY);

//    msg = "\r\nSending AT+GMR";
    UART_transmitString("\r\nSending AT+GMR");
    UART_sendCommand("AT+GMR");
//    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 22);

    // Ackowledge the PIE interrupt.
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//
// sciaRxISR - Read the character from the RXBUF and echo it back.
//
/*
 * Reads data from UART Buffer and chucks it into the Recieve FIFO.
 * First element of buffer is length
 */
__interrupt void
scibRxISR(void)
{
    int i = -1;
    char cmd[16];

    do {
        cmd[++i] = (char) SCI_readCharBlockingFIFO(SCIB_BASE);
    } while(i<16 && cmd[i] != ':' && !(i>4 && cmd[i-1]=='\r' && cmd[i]=='\n'));

    // Make sure colon exists, not sure why it wouldn't
    if (cmd[i] != ':') {
        return;
    }

    // Make sure we have a +IPD command
    // Toss all other commands, idk if they even exist lol
    if (strncmp(cmd, "\r\n+IPD,", 7) != 0) {
        return;
    }

    // Make sure epxected client is the one sending packets
    // No funny business, like Allen trying to hack Sophia's Saber
    if (cmd[7] != CONNECTION_ID) {
        return;
    }

    int len = 0;
    int j=9;
    for (; j<i; j++) {
        len = len*10 + (cmd[j] - '0');
    }

    // Check if Space in FIFO, read and toss packet if not.
    if ((ESP_RX_FIFO_PutI+1)%ESP_RX_FIFO_LENGTH == ESP_RX_FIFO_GetI) {
        while(len--) {
            SCI_readCharNonBlocking(SCIB_BASE);
        }
        return;
    }

    ESP_RX_FIFO[ESP_RX_FIFO_PutI][0] = len;

    int k = 1;
    while(k<=len) {
        ESP_RX_FIFO[ESP_RX_FIFO_PutI][k++] = SCI_readCharBlockingFIFO(SCIB_BASE);
    }

    ESP_RX_FIFO[ESP_RX_FIFO_PutI][k] = 0;

#ifdef ESP_DEBUG_MODE
    UART0_OutString("Recieving Data\r\n");
    UART0_OutString(ESP_RX_FIFO[ESP_RX_FIFO_PutI]+1);
    UART0_OutString("Done\r\n\n");
#endif

    ESP_RX_FIFO_PutI++;
    ESP_RX_FIFO_PutI %= ESP_RX_FIFO_LENGTH;

    // Acknowledge the PIE interrupt.
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}


// End of File

