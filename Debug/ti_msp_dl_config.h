/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for PID_Timer */
#define PID_Timer_INST                                                  (TIMG12)
#define PID_Timer_INST_IRQHandler                              TIMG12_IRQHandler
#define PID_Timer_INST_INT_IRQN                                (TIMG12_INT_IRQn)
#define PID_Timer_INST_LOAD_VALUE                                      (319999U)



/* Defines for Debug_UART */
#define Debug_UART_INST                                                    UART0
#define Debug_UART_INST_FREQUENCY                                       32000000
#define Debug_UART_INST_IRQHandler                              UART0_IRQHandler
#define Debug_UART_INST_INT_IRQN                                  UART0_INT_IRQn
#define GPIO_Debug_UART_RX_PORT                                            GPIOA
#define GPIO_Debug_UART_TX_PORT                                            GPIOA
#define GPIO_Debug_UART_RX_PIN                                     DL_GPIO_PIN_1
#define GPIO_Debug_UART_TX_PIN                                     DL_GPIO_PIN_0
#define GPIO_Debug_UART_IOMUX_RX                                  (IOMUX_PINCM2)
#define GPIO_Debug_UART_IOMUX_TX                                  (IOMUX_PINCM1)
#define GPIO_Debug_UART_IOMUX_RX_FUNC                   IOMUX_PINCM2_PF_UART0_RX
#define GPIO_Debug_UART_IOMUX_TX_FUNC                   IOMUX_PINCM1_PF_UART0_TX
#define Debug_UART_BAUD_RATE                                            (115200)
#define Debug_UART_IBRD_32_MHZ_115200_BAUD                                  (17)
#define Debug_UART_FBRD_32_MHZ_115200_BAUD                                  (23)





/* Defines for Grayscale_ADC */
#define Grayscale_ADC_INST                                                  ADC1
#define Grayscale_ADC_INST_IRQHandler                            ADC1_IRQHandler
#define Grayscale_ADC_INST_INT_IRQN                              (ADC1_INT_IRQn)
#define Grayscale_ADC_ADCMEM_0                                DL_ADC12_MEM_IDX_0
#define Grayscale_ADC_ADCMEM_0_REF               DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define Grayscale_ADC_ADCMEM_0_REF_VOLTAGE_V                                     3.3
#define Grayscale_ADC_ADCMEM_1                                DL_ADC12_MEM_IDX_1
#define Grayscale_ADC_ADCMEM_1_REF               DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define Grayscale_ADC_ADCMEM_1_REF_VOLTAGE_V                                     3.3
#define Grayscale_ADC_ADCMEM_2                                DL_ADC12_MEM_IDX_2
#define Grayscale_ADC_ADCMEM_2_REF               DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define Grayscale_ADC_ADCMEM_2_REF_VOLTAGE_V                                     3.3
#define GPIO_Grayscale_ADC_C5_PORT                                         GPIOB
#define GPIO_Grayscale_ADC_C5_PIN                                 DL_GPIO_PIN_18
#define GPIO_Grayscale_ADC_IOMUX_C5                              (IOMUX_PINCM44)
#define GPIO_Grayscale_ADC_IOMUX_C5_FUNC          (IOMUX_PINCM44_PF_UNCONNECTED)
#define GPIO_Grayscale_ADC_C2_PORT                                         GPIOA
#define GPIO_Grayscale_ADC_C2_PIN                                 DL_GPIO_PIN_17
#define GPIO_Grayscale_ADC_IOMUX_C2                              (IOMUX_PINCM39)
#define GPIO_Grayscale_ADC_IOMUX_C2_FUNC          (IOMUX_PINCM39_PF_UNCONNECTED)
#define GPIO_Grayscale_ADC_C6_PORT                                         GPIOB
#define GPIO_Grayscale_ADC_C6_PIN                                 DL_GPIO_PIN_19
#define GPIO_Grayscale_ADC_IOMUX_C6                              (IOMUX_PINCM45)
#define GPIO_Grayscale_ADC_IOMUX_C6_FUNC          (IOMUX_PINCM45_PF_UNCONNECTED)



/* Port definition for Pin Group Motor_A_Dir */
#define Motor_A_Dir_PORT                                                 (GPIOA)

/* Defines for AIN1: GPIOA.26 with pinCMx 59 on package pin 30 */
#define Motor_A_Dir_AIN1_PIN                                    (DL_GPIO_PIN_26)
#define Motor_A_Dir_AIN1_IOMUX                                   (IOMUX_PINCM59)
/* Defines for AIN2: GPIOA.27 with pinCMx 60 on package pin 31 */
#define Motor_A_Dir_AIN2_PIN                                    (DL_GPIO_PIN_27)
#define Motor_A_Dir_AIN2_IOMUX                                   (IOMUX_PINCM60)
/* Defines for BIN1: GPIOA.25 with pinCMx 55 on package pin 26 */
#define Motor_B_Dir_BIN1_PORT                                            (GPIOA)
#define Motor_B_Dir_BIN1_PIN                                    (DL_GPIO_PIN_25)
#define Motor_B_Dir_BIN1_IOMUX                                   (IOMUX_PINCM55)
/* Defines for BIN2: GPIOB.24 with pinCMx 52 on package pin 23 */
#define Motor_B_Dir_BIN2_PORT                                            (GPIOB)
#define Motor_B_Dir_BIN2_PIN                                    (DL_GPIO_PIN_24)
#define Motor_B_Dir_BIN2_IOMUX                                   (IOMUX_PINCM52)
/* Defines for PWMA: GPIOA.28 with pinCMx 3 on package pin 35 */
#define Motor_PWM_PWMA_PORT                                              (GPIOA)
#define Motor_PWM_PWMA_PIN                                      (DL_GPIO_PIN_28)
#define Motor_PWM_PWMA_IOMUX                                      (IOMUX_PINCM3)
/* Defines for PWMB: GPIOB.4 with pinCMx 17 on package pin 52 */
#define Motor_PWM_PWMB_PORT                                              (GPIOB)
#define Motor_PWM_PWMB_PIN                                       (DL_GPIO_PIN_4)
#define Motor_PWM_PWMB_IOMUX                                     (IOMUX_PINCM17)
/* Port definition for Pin Group Encoder_GPIO */
#define Encoder_GPIO_PORT                                                (GPIOB)

/* Defines for LEFT_A: GPIOB.6 with pinCMx 23 on package pin 58 */
// pins affected by this interrupt request:["LEFT_A","LEFT_B","RIGHT_A","RIGHT_B"]
#define Encoder_GPIO_INT_IRQN                                   (GPIOB_INT_IRQn)
#define Encoder_GPIO_INT_IIDX                   (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define Encoder_GPIO_LEFT_A_IIDX                             (DL_GPIO_IIDX_DIO6)
#define Encoder_GPIO_LEFT_A_PIN                                  (DL_GPIO_PIN_6)
#define Encoder_GPIO_LEFT_A_IOMUX                                (IOMUX_PINCM23)
/* Defines for LEFT_B: GPIOB.7 with pinCMx 24 on package pin 59 */
#define Encoder_GPIO_LEFT_B_IIDX                             (DL_GPIO_IIDX_DIO7)
#define Encoder_GPIO_LEFT_B_PIN                                  (DL_GPIO_PIN_7)
#define Encoder_GPIO_LEFT_B_IOMUX                                (IOMUX_PINCM24)
/* Defines for RIGHT_A: GPIOB.2 with pinCMx 15 on package pin 50 */
#define Encoder_GPIO_RIGHT_A_IIDX                            (DL_GPIO_IIDX_DIO2)
#define Encoder_GPIO_RIGHT_A_PIN                                 (DL_GPIO_PIN_2)
#define Encoder_GPIO_RIGHT_A_IOMUX                               (IOMUX_PINCM15)
/* Defines for RIGHT_B: GPIOB.3 with pinCMx 16 on package pin 51 */
#define Encoder_GPIO_RIGHT_B_IIDX                            (DL_GPIO_IIDX_DIO3)
#define Encoder_GPIO_RIGHT_B_PIN                                 (DL_GPIO_PIN_3)
#define Encoder_GPIO_RIGHT_B_IOMUX                               (IOMUX_PINCM16)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PID_Timer_init(void);
void SYSCFG_DL_Debug_UART_init(void);
void SYSCFG_DL_Grayscale_ADC_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
