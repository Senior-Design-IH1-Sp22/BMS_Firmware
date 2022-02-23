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

// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);
    return buffer;
}

// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32)
        return buffer;
    // consider the absolute value of the number
    int n = abs(value);
    int i = 0;
    while (n) {
        int r = n % base;
        if (r >= 10)
            buffer[i++] = 65 + (r - 10);
        else
            buffer[i++] = 48 + r;
        n = n / base;
    }
    // if the number is 0
    if (i == 0)
        buffer[i++] = '0';
    buffer[i] = '\0'; // null terminate string
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

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

    UART_init();

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
//        if (doReadVoltage) {
//                ControlAddr = 0x14;
//                EEPROM.pControlAddr   = &ControlAddr;
//                EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
//                EEPROM.NumOfDataBytes = 2;
//                status = I2C_MasterReceiver(&EEPROM);
//                while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);
//        }
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
        //debugging purposes only, for checking I2C with UART output
        if (doReadVoltageUART) {
            ControlAddr = 0x14;
            EEPROM.pControlAddr   = &ControlAddr;
            EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
            EEPROM.NumOfDataBytes = 2;
            status = I2C_MasterReceiver(&EEPROM);
            while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);

            UART_transmitString("Voltage at cell 0: ");
            char voltage_str[10];
            itoa(RX_MsgBuffer[1] >> 1, voltage_str, 10);
            UART_transmitPlain(voltage_str);
            itoa(RX_MsgBuffer[0] >> 1, voltage_str, 10);
            if (strlen(voltage_str) < 2)
                UART_transmitPlain("0");
            UART_transmitPlain(voltage_str);
            UART_transmitString("");
            while(1);
        }
        if(readAllVoltages){
            uint32_t j = 0;
            ControlAddr = 0x14;
            uint32_t AddrBase;
            //use timer interrupt instead of loop
            for(j = 0; j<15; j++){
                AddrBase = ControlAddr+1*j;
                //loop through the voltage registers
                EEPROM.pControlAddr   = &(AddrBase);
                EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
                EEPROM.NumOfDataBytes = 2;
                status = I2C_MasterReceiver(&EEPROM);
                while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);
            }
            while(1);
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

//REFERENCES
//    //Example 1: EEPROM Byte Write
//    if (doExample1) {
//        //Write 11 to EEPROM address 0x0
//        ControlAddr = 0x0;     //EEPROM address to write
//        EEPROM.NumOfDataBytes       = 1;
//        TX_MsgBuffer[0]             = 11;
//        EEPROM.pTX_MsgBuffer        = TX_MsgBuffer;
//        status = I2C_MasterTransmitter(&EEPROM);
//        DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);
//    }
//    //Example 2: EEPROM Byte Read
//    if (doExample2) {
//        //Make sure 11 is written to EEPROM address 0x0
//        ControlAddr = 0;
//        EEPROM.pControlAddr   = &ControlAddr;
//        EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
//        EEPROM.NumOfDataBytes = 1;
//        status = I2C_MasterReceiver(&EEPROM);
//        while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);
//        verifyEEPROMRead();
//    }
//    //Example 3: EEPROM word (16-bit) write
//    if (doExample3) {
//        //EEPROM address 0x1 = 22 &  0x2 = 33
//        ControlAddr = 1;   //EEPROM address to write
//        EEPROM.NumOfDataBytes  = 2;
//        TX_MsgBuffer[0]        = 0x11;
//        TX_MsgBuffer[1]        = 0x22;
//        EEPROM.pTX_MsgBuffer   = TX_MsgBuffer;
//        status = I2C_MasterTransmitter(&EEPROM);
//        DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);
//    }
//    //Example 4: EEPROM word (16-bit) read
//    if (doExample4) {
//        //Make sure EEPROM address 1 has 0x11 and 2 has 0x22
//        ControlAddr = 1;
//        EEPROM.pControlAddr   = &ControlAddr;
//        EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
//        EEPROM.NumOfDataBytes = 2;
//        status = I2C_MasterReceiver(&EEPROM);
//        verifyEEPROMRead();
//    }

//
// pass - Function to be called if data written matches data read
//
void
pass(void)
{
    asm("   ESTOP0");
    for(;;);
}

//
// fail - Function to be called if data written does NOT match data read
//
void fail(void)
{
    asm("   ESTOP0");
    for(;;);
}
//may need to use interrupt for ALL registers
//use interrupt for each type of data, put received data into struct
//receive data, compare TX to RX, if not equal it stops.
void verifyEEPROMRead(void)
{
    uint16_t i;
    while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);

    for(i=0;i<EEPROM.NumOfDataBytes;i++)
    {
        if(RX_MsgBuffer[i] != TX_MsgBuffer[i])
        {
            //Transmitted data doesn't match received data
            //Fail condition. PC shouldn't reach here
            ESTOP0;
            fail();
        }
    }
}

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


void I2C_GPIO_init(void)
{
    // I2CA pins (SDAA / SCLA)
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SDAA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SDAA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setMasterCore(DEVICE_GPIO_PIN_SDAA, GPIO_CORE_CPU1);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SDAA, GPIO_QUAL_ASYNC);

    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCLA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCLA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setMasterCore(DEVICE_GPIO_PIN_SCLA, GPIO_CORE_CPU1);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCLA, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DEVICE_GPIO_CFG_SDAA);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCLA);
}

void I2Cinit(void)
{
    //myI2CA initialization
    I2C_disableModule(I2CA_BASE);
    I2C_initMaster(I2CA_BASE, DEVICE_SYSCLK_FREQ, 30000, I2C_DUTYCYCLE_50);
    I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
    I2C_setSlaveAddress(I2CA_BASE, 0x10);
    I2C_setOwnSlaveAddress(I2CA_BASE, 0x60); //I2CA address
    I2C_disableLoopback(I2CA_BASE);
    I2C_setBitCount(I2CA_BASE, I2C_BITCOUNT_8);
    I2C_setDataCount(I2CA_BASE, 2);
    I2C_setAddressMode(I2CA_BASE, I2C_ADDR_MODE_7BITS);
    I2C_enableFIFO(I2CA_BASE);
    I2C_clearInterruptStatus(I2CA_BASE, I2C_INT_ARB_LOST | I2C_INT_NO_ACK);
    I2C_setFIFOInterruptLevel(I2CA_BASE, I2C_FIFO_TXEMPTY, I2C_FIFO_RX2);
    I2C_enableInterrupt(I2CA_BASE, I2C_INT_ADDR_SLAVE | I2C_INT_ARB_LOST | I2C_INT_NO_ACK | I2C_INT_STOP_CONDITION);
    I2C_setEmulationMode(I2CA_BASE, I2C_EMULATION_FREE_RUN);
    I2C_enableModule(I2CA_BASE);
}
// End of File

