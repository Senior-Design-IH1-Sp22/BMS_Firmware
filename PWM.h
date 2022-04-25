/*
 * PWM.h
 *
 *  Created on: Apr 24, 2022
 *      Author: xchen
 */

#ifndef PWM_H_
#define PWM_H_

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// Globals
//
EPWM_SignalParams pwmSignal =
            {60, 0.5f, 0.5f, true, DEVICE_SYSCLK_FREQ,
            EPWM_COUNTER_MODE_UP_DOWN, EPWM_CLOCK_DIVIDER_4,
            EPWM_HSCLOCK_DIVIDER_4};
EPWM_SignalParams pwmSignal2 =
            {60, 0.5f, 0.5f, true, DEVICE_SYSCLK_FREQ,
            EPWM_COUNTER_MODE_UP_DOWN, EPWM_CLOCK_DIVIDER_4,
            EPWM_HSCLOCK_DIVIDER_4};
EPWM_SignalParams pwmSignal3 =
            {60, 0.5f, 0.5f, true, DEVICE_SYSCLK_FREQ,
            EPWM_COUNTER_MODE_UP_DOWN, EPWM_CLOCK_DIVIDER_4,
            EPWM_HSCLOCK_DIVIDER_4};

// hash defines
#define myEPWM1_BASE EPWM1_BASE
#define myEPWM7_BASE EPWM7_BASE
#define myEPWM8_BASE EPWM8_BASE

// declerations
void    Board_init1();
void    EPWM_init1();
void    SYNC_init1();
void    PinMux_init1();
void    configurePhase(uint32_t base, uint32_t masterBase, uint16_t phaseVal);

// functions
void Board_init1()
{
    EALLOW;

    PinMux_init1();
    SYNC_init1();
    EPWM_init1();

    EDIS;
}

void PinMux_init1()
{
    // EPWM1
    GPIO_setPinConfig(GPIO_0_EPWM1_A);
    GPIO_setPinConfig(GPIO_1_EPWM1_B);
    // PWM8
    GPIO_setPinConfig(GPIO_14_EPWM8_A);
    GPIO_setPinConfig(GPIO_15_EPWM8_B);
    // PWM7
    GPIO_setPinConfig(GPIO_28_EPWM7_A);
    GPIO_setPinConfig(GPIO_29_EPWM7_B);
}

void EPWM_init1(){
    EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_2);
    EPWM_setTimeBasePeriod(myEPWM1_BASE, 0);
    EPWM_setTimeBaseCounter(myEPWM1_BASE, 0);
    EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_disablePhaseShiftLoad(myEPWM1_BASE);
    EPWM_setPhaseShift(myEPWM1_BASE, 0);
    EPWM_setCounterCompareValue(myEPWM1_BASE, EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareShadowLoadMode(myEPWM1_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(myEPWM1_BASE, EPWM_COUNTER_COMPARE_B, 0);
    EPWM_setCounterCompareShadowLoadMode(myEPWM1_BASE, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);

    EPWM_setClockPrescaler(myEPWM7_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_2);
    EPWM_setTimeBasePeriod(myEPWM7_BASE, 0);
    EPWM_setTimeBaseCounter(myEPWM7_BASE, 0);
    EPWM_setTimeBaseCounterMode(myEPWM7_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_disablePhaseShiftLoad(myEPWM7_BASE);
    EPWM_setPhaseShift(myEPWM7_BASE, 0);
    EPWM_setCounterCompareValue(myEPWM7_BASE, EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareShadowLoadMode(myEPWM7_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(myEPWM7_BASE, EPWM_COUNTER_COMPARE_B, 0);
    EPWM_setCounterCompareShadowLoadMode(myEPWM7_BASE, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(myEPWM7_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);

    EPWM_setDeadBandDelayPolarity(myEPWM7_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
    EPWM_setDeadBandDelayMode(myEPWM7_BASE, EPWM_DB_RED, true);
    EPWM_setRisingEdgeDelayCount(myEPWM7_BASE, 1000);
    EPWM_setDeadBandDelayMode(myEPWM7_BASE, EPWM_DB_FED, true);
    EPWM_setFallingEdgeDelayCount(myEPWM7_BASE, 1000);

    EPWM_setClockPrescaler(myEPWM8_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_2);
    EPWM_setTimeBasePeriod(myEPWM8_BASE, 0);
    EPWM_setTimeBaseCounter(myEPWM8_BASE, 0);
    EPWM_setTimeBaseCounterMode(myEPWM8_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_disablePhaseShiftLoad(myEPWM8_BASE);
    EPWM_setPhaseShift(myEPWM8_BASE, 0);
    EPWM_setCounterCompareValue(myEPWM8_BASE, EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareShadowLoadMode(myEPWM8_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(myEPWM8_BASE, EPWM_COUNTER_COMPARE_B, 0);
    EPWM_setCounterCompareShadowLoadMode(myEPWM8_BASE, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(myEPWM8_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);

    EPWM_setDeadBandDelayPolarity(myEPWM8_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
    EPWM_setDeadBandDelayMode(myEPWM8_BASE, EPWM_DB_RED, true);
    EPWM_setRisingEdgeDelayCount(myEPWM8_BASE, 1000);
    EPWM_setDeadBandDelayMode(myEPWM8_BASE, EPWM_DB_FED, true);
    EPWM_setFallingEdgeDelayCount(myEPWM8_BASE, 1000);
}

void SYNC_init1(){
    SysCtl_setSyncOutputConfig(SYSCTL_SYNC_OUT_SRC_EPWM1SYNCOUT);
    // For EPWM1, the sync input is: SYSCTL_SYNC_IN_SRC_EXTSYNCIN1
    SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
    SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM7, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
    SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_ECAP1, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
    SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_ECAP4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
    SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_ECAP6, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
    // SOCA
    SysCtl_enableExtADCSOCSource(0);
    // SOCB
    SysCtl_enableExtADCSOCSource(0);
}

void configurePhase(uint32_t base, uint32_t masterBase, uint16_t phaseVal)
{
    uint32_t readPrdVal, phaseRegVal;

    // Read Period value to calculate value for Phase Register
    readPrdVal = EPWM_getTimeBasePeriod(masterBase);

    // Caluclate phase register values based on Time Base counter mode
    if((HWREGH(base + EPWM_O_TBCTL) & 0x3U) == EPWM_COUNTER_MODE_UP_DOWN)
    {
        phaseRegVal = (2U * readPrdVal * phaseVal) / 360U;
    }
    else if((HWREGH(base + EPWM_O_TBCTL) & 0x3U) < EPWM_COUNTER_MODE_UP_DOWN)
    {
        phaseRegVal = (readPrdVal * phaseVal) / 360U;
    }

    EPWM_selectPeriodLoadEvent(base, EPWM_SHADOW_LOAD_MODE_SYNC);
    EPWM_setPhaseShift(base, phaseRegVal);
    EPWM_setTimeBaseCounter(base, phaseRegVal);
}

// Main
//void main(void)
//{
//    // Initialize device clock and peripherals
//    Device_init();
//
//    // Disable pin locks and enable internal pull-ups.
//    Device_initGPIO();
//
//    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
//    Interrupt_initModule();
//
//    // Initialize the PIE vector table with pointers to the shell Interrupt
//    // Service Routines (ISR).
//    Interrupt_initVectorTable();
//
//    // For this case just init GPIO pins for ePWM1, ePWM8, ePWM7
//    Board_init1();
//
//    // Disable sync(Freeze clock to PWM as well)
//    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
//
//    // Configuring ePWM module for desired frequency and duty
//    EPWM_configureSignal(myEPWM1_BASE, &pwmSignal);
//    EPWM_configureSignal(myEPWM7_BASE, &pwmSignal2);
//    EPWM_configureSignal(myEPWM8_BASE, &pwmSignal3);
//
//    // Configure phase between PWM1, PWM8 & PWM7.
//    // PWM1 is configured as master and ePWM8 & 7
//    // are configured as slaves.
//    EPWM_disablePhaseShiftLoad(myEPWM1_BASE);
//    EPWM_setPhaseShift(myEPWM1_BASE, 0U);
//
//    // ePWM1 SYNCO is generated on CTR=0
//    EPWM_setSyncOutPulseMode(myEPWM1_BASE, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);
//
//    // Configure phase shift for EPWM2 & 3
//    configurePhase(myEPWM7_BASE, myEPWM1_BASE, 0);
//    configurePhase(myEPWM8_BASE, myEPWM1_BASE, 180);
//
//    EPWM_enablePhaseShiftLoad(myEPWM7_BASE);
//    EPWM_enablePhaseShiftLoad(myEPWM8_BASE);
//
//    // Enable sync and clock to PWM
//    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
//
//    // Enable interrupts required for this example
//    Interrupt_enable(INT_EPWM1);
//
//    // Enable global Interrupts and higher priority real-time debug events:
//    EINT;  // Enable Global interrupt INTM
//    ERTM;  // Enable Global realtime interrupt DBGM
//
//    // IDLE loop. Just sit and loop forever (optional):
//    for(;;)
//    {
//        asm ("  NOP");
//    }
//}





#endif /* PWM_H_ */
