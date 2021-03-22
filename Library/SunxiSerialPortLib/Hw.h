#pragma once

#include <Library/PcdLib.h>

#define UART_BASE FixedPcdGet32(UartBase)

#define UART_RBR (UART_BASE + 0)  /* Receiver */
#define UART_THR (UART_BASE + 0)  /* Transmitter */
#define UART_DLL (UART_BASE + 0)  /* Divisor low 8 bits */
#define UART_DLH (UART_BASE + 4)  /* Divisor high 8 bits */
#define UART_IER (UART_BASE + 4)  /* Interrupt enable */
#define UART_IIR (UART_BASE + 8)  /* Interrupt ID register */
#define UART_FCR (UART_BASE + 8)  /* FIFO control register */
#define UART_LCR (UART_BASE + 12) /* Line control */
#define UART_MCR (UART_BASE + 16) /* Modem control */
#define UART_LSR (UART_BASE + 20) /* Line status */
#define UART_MSR (UART_BASE + 24) /* Modem status */

#define UART_LSR_DR (1)        /* Data ready */
#define UART_LSR_OE (1 << 1)   /* Overrun error */
#define UART_LSR_PE (1 << 2)   /* Parity error */
#define UART_LSR_FE (1 << 3)   /* Framing error */
#define UART_LSR_BI (1 << 4)   /* Break interrupt */
#define UART_LSR_THRE (1 << 5) /* THR empty */
#define UART_LSR_TEMT (1 << 6) /* Transmitter empty */
#define UART_LSR_FERR (1 << 7) /* FIFO data error */

#define UART_FCR_ENABLE_FIFO (1 << 0)