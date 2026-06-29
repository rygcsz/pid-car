/*
 * uart_debug.c - UART debug output
 * MSPM0G3507 PID Line-Following Car
 *
 * Simple printf-like debug output over UART0 (XDS110 back-channel)
 *
 * NOTE: SysConfig assigns UART0 to PA0/PA1 (SDK v2.10 removed PA10/PA11
 * from the pin dropdown). We manually remap IOMUX here so that UART0
 * actually routes through PA10(TX)/PA11(RX), matching the LaunchPad
 * XDS110 back-channel hardware wiring.
 */

#include "uart_debug.h"
#include "ti_msp_dl_config.h"
#include "sys_config.h"
#include <stdio.h>
#include <stdarg.h>

void uart_debug_init(void)
{
    /*
     * Remap UART0 from PA0/PA1 (SysConfig placeholder) to PA10/PA11
     * so the XDS110 back-channel on the LaunchPad works correctly.
     *
     * PA10 = PINCM21 → UART0_TX
     * PA11 = PINCM22 → UART0_RX
     */
    /* Step 1: Release PA0/PA1 from UART function (set to GPIO input) */
    DL_GPIO_initDigitalInput(IOMUX_PINCM1);
    DL_GPIO_initDigitalInput(IOMUX_PINCM2);

    /* Step 2: Route UART0 to PA10/PA11 */
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM21,
                                         IOMUX_PINCM21_PF_UART0_TX);
    DL_GPIO_initPeripheralInputFunction(IOMUX_PINCM22,
                                        IOMUX_PINCM22_PF_UART0_RX);
}

void uart_debug_send_char(char c)
{
    DL_UART_Main_transmitDataBlocking(DEBUG_UART_INST, (uint8_t)c);
}

void uart_debug_send_string(const char *str)
{
    while (*str) {
        DL_UART_Main_transmitDataBlocking(DEBUG_UART_INST, (uint8_t)*str);
        str++;
    }
}

/*
 * Minimal printf implementation for debug output
 * Supports: %d, %f, %s, %c, %u, %x, %%
 */
void uart_debug_printf(const char *fmt, ...)
{
    char buf[64];
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    int val = va_arg(args, int);
                    snprintf(buf, sizeof(buf), "%d", val);
                    uart_debug_send_string(buf);
                    break;
                }
                case 'u': {
                    unsigned int val = va_arg(args, unsigned int);
                    snprintf(buf, sizeof(buf), "%u", val);
                    uart_debug_send_string(buf);
                    break;
                }
                case 'f': {
                    double val = va_arg(args, double);
                    snprintf(buf, sizeof(buf), "%.2f", val);
                    uart_debug_send_string(buf);
                    break;
                }
                case 's': {
                    char *str = va_arg(args, char*);
                    uart_debug_send_string(str);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    uart_debug_send_char(c);
                    break;
                }
                case 'x': {
                    unsigned int val = va_arg(args, unsigned int);
                    snprintf(buf, sizeof(buf), "%x", val);
                    uart_debug_send_string(buf);
                    break;
                }
                case '%':
                    uart_debug_send_char('%');
                    break;
                default:
                    uart_debug_send_char('%');
                    uart_debug_send_char(*fmt);
                    break;
            }
        } else {
            uart_debug_send_char(*fmt);
        }
        fmt++;
    }

    va_end(args);
}
