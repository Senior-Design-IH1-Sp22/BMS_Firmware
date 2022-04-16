//#############################################################################
//
// FILE:   i2c_ex6_eeprom_interrupt.c
//
// TITLE:  I2C EEPROM Write / Read using interrupt
//
//! \addtogroup driver_example_list
//! <h1>I2C EEPROM</h1>
//!
//! This program will shows how to perform different EEPROM write and read commands using I2C interrupts
//! EEPROM used for this example is AT24C256
//!
//! \b External \b Connections \n
//!  - Connect external I2C EEPROM at address 0x50
//!  --------------------------------
//!    Signal   |  I2CA   |  EEPROM
//!  --------------------------------
//!     SCL     | GPIO37 |  SCL
//!     SDA     | GPIO35 |  SDA
//!     Make sure to connect GND pins if EEPROM and C2000 device are in different board.
//!  --------------------------------
//! //Example 1: EEPROM Byte Write
//! //Example 2: EEPROM Byte Read
//! //Example 3: EEPROM word (16-bit) write
//! //Example 4: EEPROM word (16-bit) read
//! //Example 5: EEPROM Page write
//! //Example 6: EEPROM word Paged read
//!
//! \b Watch \b Variables \n
//!  - \b TX_MsgBuffer - Message buffer which stores the data to be transmitted
//!  - \b RX_MsgBuffer - Message buffer which stores the data to be received
//!
//!
//#############################################################################


// Defines
#define _LAUNCHXL_F280049C          true

// Included Files
#include "driverlib.h"
#include "device.h"
#include <i2c_helper.h>
#include <uart_helper.h>
#include <ESP.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Globals
struct I2CHandle EEPROM;
struct I2CHandle TempSensor;

struct I2CHandle *currentResponderPtr;                   // Used in interrupt

uint16_t passCount = 0;
uint16_t failCount = 0;

uint16_t AvailableI2C_slaves[10];

uint16_t TX_MsgBuffer[MAX_BUFFER_SIZE];
uint16_t RX_MsgBuffer[MAX_BUFFER_SIZE];
uint32_t ControlAddr;
uint16_t status=0;

//I2C-UART 16b data buffer
uint16_t Data16b_Buf[MAX_BUFFER_SIZE];

// Function Prototypes
interrupt void i2cFIFO_isr(void);
interrupt void i2c_isr(void);

void fail(void);
void pass(void);
void initI2CFIFO(void);
void verifyEEPROMRead(void);

void I2C_GPIO_init(void);
void I2Cinit(void);

#define doExample1 0
#define doExample2 0
#define doExample3 0
#define doExample4 0
#define doExample5 0
#define doExample6 0
#define doAlarmEnable 0
#define doReadVoltageUART 0
#define doReadVoltage 0
#define readAllVoltages 1
#define doDeviceNum 0



void main(void)
{
    // Initialize device clock and peripherals
    Device_init();

    // Disable pin locks and enable internal pullups.
    Device_initGPIO();

    // Initialize PIE and clear PIE registers. Disable CPU interrupts.
    Interrupt_initModule();

    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    Interrupt_initVectorTable();

    UART_Init();

    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    EINT;
    ERTM;
    while(1) {
//        UART_TransmitCOM("hello\n");
        UART_TransmitCOM("\r\nSending a string\n");
        ESP_WifiSendString("aabcdefg", 8);
        DEVICE_DELAY_US(500000);
//        uint16_t receivedChar = SCI_readCharBlockingFIFO(SCIB_BASE);
//        SCI_writeCharBlockingFIFO(SCIA_BASE, receivedChar);
//        while (SCI_getRxStatus(SCIB_BASE) & SCI_RXSTATUS_READY) {
//            receivedChar = SCI_readCharBlockingFIFO(SCIB_BASE);
//            SCI_writeCharBlockingFIFO(SCIA_BASE, receivedChar);
//        }
//        UART_PrintRxBuffer();
//        UART_ResetRxBuffer();
        DEVICE_DELAY_US(500000);
    }
}

/*
void main(void)
{
    // Initialize device clock and peripherals
    Device_init();

    // Disable pin locks and enable internal pullups.
    Device_initGPIO();

    // Initialize I2C pins
    I2C_GPIO_init();

    // Initialize PIE and clear PIE registers. Disable CPU interrupts.
    Interrupt_initModule();

    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    Interrupt_initVectorTable();

    I2Cinit();

    //UART_Init();

    // Interrupts that are used in this example are re-mapped to ISR functions
    // found within this file.
    Interrupt_register(INT_I2CA_FIFO, &i2cFIFO_isr);

    Interrupt_enable(INT_I2CA_FIFO);
    Interrupt_register(INT_I2CA, &i2c_isr);
    Interrupt_enable(INT_I2CA);

    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    EINT;
    ERTM;

    //I2Cs connected to I2CA will be found in AvailableI2C_slaves buffer after you run I2CBusScan function.
    //scans for I2C bus slaves (look for ACK back)
    uint16_t *pAvailableI2C_slaves = AvailableI2C_slaves;
    status = I2CBusScan(I2CA_BASE, pAvailableI2C_slaves);

    currentResponderPtr = &EEPROM;

    EEPROM.currentHandlePtr     = &EEPROM;
    EEPROM.SlaveAddr            = 0x8;
    EEPROM.WriteCycleTime_in_us = 3000;    //6ms for EEPROM this code was tested
    EEPROM.base                 = I2CA_BASE;
    EEPROM.pControlAddr         = &ControlAddr;
    EEPROM.NumOfAddrBytes       = 1;

    while(1){
        if (doReadVoltage) {
                ControlAddr = 0x14;
                EEPROM.pControlAddr   = &ControlAddr;
                EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
                EEPROM.NumOfDataBytes = 2;
                status = I2C_MasterReceiver(&EEPROM);
                while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);

                ControlAddr = 0x15;
                EEPROM.pControlAddr   = &ControlAddr;
                EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
                EEPROM.NumOfDataBytes = 2;
                status = I2C_MasterReceiver(&EEPROM);
                while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);

                ControlAddr = 0x16;
                EEPROM.pControlAddr   = &ControlAddr;
                EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
                EEPROM.NumOfDataBytes = 2;
                status = I2C_MasterReceiver(&EEPROM);
                while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);
        }
        if (doAlarmEnable) {
                ControlAddr = 0x66;
                EEPROM.NumOfDataBytes  = 2;
                TX_MsgBuffer[0]        = 0x82;
                TX_MsgBuffer[1]        = 0xF0;
                EEPROM.pTX_MsgBuffer   = TX_MsgBuffer;
                status = I2C_MasterTransmitter(&EEPROM);
                DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);
        }
        if(readAllVoltages){
            uint32_t j = 0;
            ControlAddr = 0x14;
            //uint32_t AddrBase;
            uint32_t Addr[32];
            //use timer interrupt instead of loop
            while(ControlAddr < 0x32){
                Addr[(ControlAddr-0x14)] = ControlAddr;
                //loop through the voltage registers
                EEPROM.pControlAddr   = &ControlAddr;
                EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
                EEPROM.NumOfDataBytes = 2;
                status = I2C_MasterReceiver(&EEPROM);
                while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);
                //read I2C FIFO buffer
                Data16b_Buf[(ControlAddr-0x14)] = RX_MsgBuffer[j];
                j=j^1;
                ControlAddr = ControlAddr+1;
            }
            //while(1);
        }
        if (doDeviceNum) {
            ControlAddr = 0x3E;
            EEPROM.NumOfDataBytes  = 2;
            TX_MsgBuffer[0]        = 0x1;
            TX_MsgBuffer[1]        = 0x0;
            EEPROM.pTX_MsgBuffer   = TX_MsgBuffer;
            status = I2C_MasterTransmitter(&EEPROM);
            DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);

            ControlAddr = 0x40;
            EEPROM.pControlAddr   = &ControlAddr;
            EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
            EEPROM.NumOfDataBytes = 2;
            status = I2C_MasterReceiver(&EEPROM);
            while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);

            UART_transmitString("Device number: ");
            char device_str[10];
            itoa(RX_MsgBuffer[0], device_str, 10);
            UART_transmitPlain(device_str);
            UART_transmitString("");
            while(1);
        }
    }
}
*/
interrupt void i2c_isr(void)
{
    uint16_t MasterSlave = HWREGH(currentResponderPtr->base + I2C_O_MDR);

    handleI2C_ErrorCondition(currentResponderPtr);

    if(MasterSlave & I2C_MDR_MST)
    {
        I2C_enableInterrupt(currentResponderPtr->base, I2C_INT_RXFF);
        I2C_clearInterruptStatus(currentResponderPtr->base,(I2C_INT_RXFF));
    }
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

interrupt void i2cFIFO_isr(void)
{
    Write_Read_TX_RX_FIFO(currentResponderPtr);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

// End of File

