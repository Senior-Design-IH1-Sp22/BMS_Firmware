/*
 * i2c_helper.h
 *
 * Reference: i2cLib_FIFO_master_interrupt.h from C28x-I2C library
 */

#ifndef I2CLIB_FIFO_INTERRUPT_H
#define I2CLIB_FIFO_INTERRUPT_H

#include "device.h"

//
// Error messages for read and write functions
//
#define ERROR_BUS_BUSY              0x1000
#define ERROR_NACK_RECEIVED         0x2000
#define ERROR_ARBITRATION_LOST      0x3000
#define ERROR_STOP_NOT_READY        0x5555
#define SUCCESS                     0x0000

#define MAX_BUFFER_SIZE             128
#define I2C_FIFO_LEVEL              16

#define MAX_7_BIT_ADDRESS 127U
#define MAX_10_BIT_ADDRESS 1023U


//
// Typedefs
//
struct I2CHandle
{
    uint32_t base;
    uint16_t SlaveAddr;                  // Slave address tied to the message.
    uint32_t *pControlAddr;
    uint16_t NumOfAddrBytes;
    uint16_t *pTX_MsgBuffer;             // Pointer to TX message buffer
    uint16_t *pRX_MsgBuffer;             // Pointer to RX message buffer
    uint16_t NumOfDataBytes;             // Number of valid bytes in message.
    struct I2CHandle *currentHandlePtr;

    uint16_t numofSixteenByte;
    uint16_t remainingBytes;

    uint16_t WriteCycleTime_in_us;      //  Slave write cycle time. Depends on slave.
                                        //  Please check slave device datasheet
};

uint16_t I2C_TransmitSlaveAddress_ControlBytes(struct I2CHandle *I2C_Params);
uint16_t I2CBusScan(uint32_t base, uint16_t *pAvailableI2C_slaves);

void I2C_GPIO_init(void);
void I2Cinit(void);

uint16_t I2C_MasterTransmitter(struct I2CHandle *I2C_Params);
uint16_t I2C_MasterReceiver(struct I2CHandle *I2C_Params);
uint16_t I2C_SlaveTransmitter(struct I2CHandle *I2C_Params);
uint16_t I2C_SlaveReceiver(struct I2CHandle *I2C_Params);

uint16_t checkBusStatus(uint32_t base);

uint16_t handleNACK(uint32_t base);

extern void handleI2C_ErrorCondition(struct I2CHandle *I2C_Params);
extern void Write_Read_TX_RX_FIFO(struct I2CHandle *I2C_Params);

extern uint16_t TX_MsgBuffer[MAX_BUFFER_SIZE];
extern uint16_t RX_MsgBuffer[MAX_BUFFER_SIZE];

extern uint16_t status;

extern struct I2CHandle *currentSlavePtr;

#endif
