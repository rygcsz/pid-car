/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PID_Timer_init();
    SYSCFG_DL_Debug_UART_init();
    SYSCFG_DL_Grayscale_ADC_init();
}



SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerG_reset(PID_Timer_INST);
    DL_UART_Main_reset(Debug_UART_INST);
    DL_ADC12_reset(Grayscale_ADC_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerG_enablePower(PID_Timer_INST);
    DL_UART_Main_enablePower(Debug_UART_INST);
    DL_ADC12_enablePower(Grayscale_ADC_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_Debug_UART_IOMUX_TX, GPIO_Debug_UART_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_Debug_UART_IOMUX_RX, GPIO_Debug_UART_IOMUX_RX_FUNC);

    DL_GPIO_initDigitalOutput(Motor_A_Dir_AIN1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_A_Dir_AIN2_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_B_Dir_BIN1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_B_Dir_BIN2_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_PWM_PWMA_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_PWM_PWMB_IOMUX);

    DL_GPIO_initDigitalInputFeatures(Encoder_GPIO_LEFT_A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Encoder_GPIO_LEFT_B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Encoder_GPIO_RIGHT_A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Encoder_GPIO_RIGHT_B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_clearPins(GPIOA, Motor_A_Dir_AIN1_PIN |
		Motor_A_Dir_AIN2_PIN |
		Motor_B_Dir_BIN1_PIN |
		Motor_PWM_PWMA_PIN);
    DL_GPIO_enableOutput(GPIOA, Motor_A_Dir_AIN1_PIN |
		Motor_A_Dir_AIN2_PIN |
		Motor_B_Dir_BIN1_PIN |
		Motor_PWM_PWMA_PIN);
    DL_GPIO_clearPins(GPIOB, Motor_B_Dir_BIN2_PIN |
		Motor_PWM_PWMB_PIN);
    DL_GPIO_enableOutput(GPIOB, Motor_B_Dir_BIN2_PIN |
		Motor_PWM_PWMB_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOB, DL_GPIO_PIN_6_EDGE_RISE_FALL |
		DL_GPIO_PIN_7_EDGE_RISE_FALL |
		DL_GPIO_PIN_2_EDGE_RISE_FALL |
		DL_GPIO_PIN_3_EDGE_RISE_FALL);
    DL_GPIO_clearInterruptStatus(GPIOB, Encoder_GPIO_LEFT_A_PIN |
		Encoder_GPIO_LEFT_B_PIN |
		Encoder_GPIO_RIGHT_A_PIN |
		Encoder_GPIO_RIGHT_B_PIN);
    DL_GPIO_enableInterrupt(GPIOB, Encoder_GPIO_LEFT_A_PIN |
		Encoder_GPIO_LEFT_B_PIN |
		Encoder_GPIO_RIGHT_A_PIN |
		Encoder_GPIO_RIGHT_B_PIN);

}


SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);

}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32000000 Hz = 32000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gPID_TimerClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * PID_Timer_INST_LOAD_VALUE = (10ms * 32000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gPID_TimerTimerConfig = {
    .period     = PID_Timer_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC_UP,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_PID_Timer_init(void) {

    DL_TimerG_setClockConfig(PID_Timer_INST,
        (DL_TimerG_ClockConfig *) &gPID_TimerClockConfig);

    DL_TimerG_initTimerMode(PID_Timer_INST,
        (DL_TimerG_TimerConfig *) &gPID_TimerTimerConfig);
    DL_TimerG_enableInterrupt(PID_Timer_INST , DL_TIMERG_INTERRUPT_LOAD_EVENT);
	NVIC_SetPriority(PID_Timer_INST_INT_IRQN, 2);
    DL_TimerG_enableClock(PID_Timer_INST);





}


static const DL_UART_Main_ClockConfig gDebug_UARTClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gDebug_UARTConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_Debug_UART_init(void)
{
    DL_UART_Main_setClockConfig(Debug_UART_INST, (DL_UART_Main_ClockConfig *) &gDebug_UARTClockConfig);

    DL_UART_Main_init(Debug_UART_INST, (DL_UART_Main_Config *) &gDebug_UARTConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115211.52
     */
    DL_UART_Main_setOversampling(Debug_UART_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(Debug_UART_INST, Debug_UART_IBRD_32_MHZ_115200_BAUD, Debug_UART_FBRD_32_MHZ_115200_BAUD);



    DL_UART_Main_enable(Debug_UART_INST);
}

/* Grayscale_ADC Initialization */
static const DL_ADC12_ClockConfig gGrayscale_ADCClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_ULPCLK,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_8,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
SYSCONFIG_WEAK void SYSCFG_DL_Grayscale_ADC_init(void)
{
    DL_ADC12_setClockConfig(Grayscale_ADC_INST, (DL_ADC12_ClockConfig *) &gGrayscale_ADCClockConfig);

    DL_ADC12_initSeqSample(Grayscale_ADC_INST,
        DL_ADC12_REPEAT_MODE_DISABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, DL_ADC12_TRIG_SRC_SOFTWARE,
        DL_ADC12_SEQ_START_ADDR_00, DL_ADC12_SEQ_END_ADDR_02, DL_ADC12_SAMP_CONV_RES_12_BIT,
        DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
    DL_ADC12_configConversionMem(Grayscale_ADC_INST, Grayscale_ADC_ADCMEM_0,
        DL_ADC12_INPUT_CHAN_5, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_configConversionMem(Grayscale_ADC_INST, Grayscale_ADC_ADCMEM_1,
        DL_ADC12_INPUT_CHAN_2, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_configConversionMem(Grayscale_ADC_INST, Grayscale_ADC_ADCMEM_2,
        DL_ADC12_INPUT_CHAN_6, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_setPowerDownMode(Grayscale_ADC_INST,DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(Grayscale_ADC_INST,80);
    DL_ADC12_enableConversions(Grayscale_ADC_INST);
}

