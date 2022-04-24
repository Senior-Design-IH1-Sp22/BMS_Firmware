/*
 * uart_helper.c
 */

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
#define ESP_RX_BUFFER_LENGTH 100
#define ESP_RX_FIFO_LENGTH  16
#define ESP_RX_FIFO_WIDTH   20

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


void UART_ResetRxBuffer(void) {
    UART_RxIndex = 0;
    int i = 0;
    for (; i < 100; i++) {
        UART_RxBuffer[i] = '\0';
    }
}

void UART_PrintRxBuffer(void) {
    int i = 0;
    for (; (i < 100) && (UART_RxBuffer[i] != '\0'); i++) {
        SCI_writeCharBlockingFIFO(SCIA_BASE, UART_RxBuffer[i]);
    }
}

void UART_TransmitCOM(char* string) {
    SCI_writeCharArray(SCIA_BASE, (uint16_t*)string, strlen(string));
}

void UART_TransmitESP(char* string, int len) {
    SCI_writeCharArray(UART_ESP_BASE, (uint16_t*)string, len);
}

void UART_Init(void) {
    // GPIO9 is the SCIA Rx pin.
    GPIO_setMasterCore(9, GPIO_CORE_CPU1);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCIRXDA);
    GPIO_setDirectionMode(9, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(9, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(9, GPIO_QUAL_ASYNC);

    // GPIO8 is the SCIA Tx pin.
    GPIO_setMasterCore(8, GPIO_CORE_CPU1);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCITXDA);
    GPIO_setDirectionMode(8, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(8, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(8, GPIO_QUAL_ASYNC);

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

#if UART_ESP_BASE == SCIB_BASE
    // Configure SCIA for COM
    SCI_setConfig(SCIA_BASE, 25000000, 9600, (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);
#endif

    // Configure SCIB for ESP
    SCI_setConfig(UART_ESP_BASE, 25000000, 115200, (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(UART_ESP_BASE);
    SCI_resetRxFIFO(UART_ESP_BASE);
    SCI_resetTxFIFO(UART_ESP_BASE);
    SCI_clearInterruptStatus(UART_ESP_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);
    SCI_enableFIFO(UART_ESP_BASE);
    SCI_enableModule(UART_ESP_BASE);
    SCI_performSoftwareReset(UART_ESP_BASE);
}

// ======================================================================================
//              ESP

//
// sciaTxISR - Disable the TXRDY interrupt and print message asking
//             for a character.
//
__interrupt void
scibTxISR(void) {
    // Disable the TXRDY interrupt.
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_TXRDY);

//    msg = "\r\nSending AT+GMR";
    UART_TransmitCOM("\r\nSending AT+GMR\n");
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
scibRxISR(void) {
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
            SCI_readCharBlockingFIFO(SCIB_BASE);
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

