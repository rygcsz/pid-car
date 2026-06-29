/*
 * encoder.c - MG513 Hall encoder driver
 * MSPM0G3507 PID Line-Following Car
 *
 * Software 4x quadrature decode via GPIO interrupt.
 * Left  encoder: PB6(A), PB7(B)
 * Right encoder: PB2(A), PB3(B)
 */

#include "encoder.h"
#include "board.h"

volatile int encoder_left_count  = 0;
volatile int encoder_right_count = 0;

static int last_left_speed  = 0;
static int last_right_speed = 0;

void encoder_init(void)
{
    /* Pin config, polarity, and interrupt enable are done by SysConfig.
     * We only need to enable the NVIC here. */
    NVIC_EnableIRQ(Encoder_GPIO_INT_IRQN);
}

/*
 * Quadrature decode helper
 * Returns +1 or -1 based on A/B phase relationship
 */
static int decode_quadrature(uint32_t pin_a_val, uint32_t pin_b_val,
                              uint32_t triggered_pin, int is_rising)
{
    /*
     * Standard quadrature decode:
     *   A rising + B high => +1 (or -1 depending on wiring)
     *   A rising + B low  => -1
     *   A falling + B high => -1
     *   A falling + B low  => +1
     * Same logic applies when B triggers, with A/B swapped
     */
    if (triggered_pin == 0) {
        /* A phase triggered */
        if (is_rising) {
            return (pin_b_val) ? 1 : -1;
        } else {
            return (pin_b_val) ? -1 : 1;
        }
    } else {
        /* B phase triggered */
        if (is_rising) {
            return (pin_a_val) ? -1 : 1;
        } else {
            return (pin_a_val) ? 1 : -1;
        }
    }
}

/*
 * GPIO interrupt handler for encoder pins
 * This function name must match the SysConfig generated IRQ handler
 * or be called from it.
 */
void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
        {
            uint32_t int_status = DL_GPIO_getEnabledInterruptStatus(GPIOB,
                LEFT_ENC_A_PIN | LEFT_ENC_B_PIN |
                RIGHT_ENC_A_PIN | RIGHT_ENC_B_PIN);

            /* Read current pin states */
            uint32_t port_val = DL_GPIO_readPins(GPIOB,
                LEFT_ENC_A_PIN | LEFT_ENC_B_PIN |
                RIGHT_ENC_A_PIN | RIGHT_ENC_B_PIN);

            /* Left encoder */
            if (int_status & LEFT_ENC_A_PIN) {
                int rising = (port_val & LEFT_ENC_A_PIN) ? 1 : 0;
                encoder_left_count += decode_quadrature(
                    port_val & LEFT_ENC_A_PIN,
                    port_val & LEFT_ENC_B_PIN,
                    0, rising);
            }
            if (int_status & LEFT_ENC_B_PIN) {
                int rising = (port_val & LEFT_ENC_B_PIN) ? 1 : 0;
                encoder_left_count += decode_quadrature(
                    port_val & LEFT_ENC_A_PIN,
                    port_val & LEFT_ENC_B_PIN,
                    1, rising);
            }

            /* Right encoder */
            if (int_status & RIGHT_ENC_A_PIN) {
                int rising = (port_val & RIGHT_ENC_A_PIN) ? 1 : 0;
                encoder_right_count += decode_quadrature(
                    port_val & RIGHT_ENC_A_PIN,
                    port_val & RIGHT_ENC_B_PIN,
                    0, rising);
            }
            if (int_status & RIGHT_ENC_B_PIN) {
                int rising = (port_val & RIGHT_ENC_B_PIN) ? 1 : 0;
                encoder_right_count += decode_quadrature(
                    port_val & RIGHT_ENC_A_PIN,
                    port_val & RIGHT_ENC_B_PIN,
                    1, rising);
            }

            /* Clear interrupt flags */
            DL_GPIO_clearInterruptStatus(GPIOB,
                LEFT_ENC_A_PIN | LEFT_ENC_B_PIN |
                RIGHT_ENC_A_PIN | RIGHT_ENC_B_PIN);
        }
        break;

        default:
            break;
    }
}

int encoder_get_left_speed(void)
{
    int speed = encoder_left_count;
    encoder_left_count = 0;
    last_left_speed = speed;
    return speed;
}

int encoder_get_right_speed(void)
{
    int speed = encoder_right_count;
    encoder_right_count = 0;
    last_right_speed = speed;
    return -speed;  /* Negate: right encoder A/B phases are swapped */
}
