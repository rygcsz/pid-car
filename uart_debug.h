/*
 * uart_debug.h - UART debug output
 * MSPM0G3507 PID Line-Following Car
 *
 * Uses UART0 (PA10-TX/PA11-RX) via XDS110 back-channel
 * SysConfig uses PA0/PA1 as placeholder; IOMUX remapped in uart_debug_init()
 */

#ifndef __UART_DEBUG_H__
#define __UART_DEBUG_H__

#include <stdint.h>

void uart_debug_init(void);
void uart_debug_printf(const char *fmt, ...);
void uart_debug_send_char(char c);
void uart_debug_send_string(const char *str);

#endif /* __UART_DEBUG_H__ */
