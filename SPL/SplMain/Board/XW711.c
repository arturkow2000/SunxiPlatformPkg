#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/SunxiGpioLib.h>

#include "../Sun4i/Ccm.h"

#define UART_BASE FixedPcdGet32(UartBase)

VOID SplXW711InitGpio(VOID)
{
  // On XW711 PG03 and PG04 are used for TX and RX respectively
  // Disable PE10 and PE11 to avoid conflict
  SunxiGpioMuxSetFunction(SUNXI_GPIO_PIN_PE(10), L"gpio_in");
  SunxiGpioMuxSetFunction(SUNXI_GPIO_PIN_PE(11), L"gpio_in");
  // Enable PG03 and PG04
  SunxiGpioPullSet(SUNXI_GPIO_PIN_PG(03), SUNXI_GPIO_PULL_UP);
  SunxiGpioPullSet(SUNXI_GPIO_PIN_PG(04), SUNXI_GPIO_PULL_UP);
  SunxiGpioMuxSetFunction(SUNXI_GPIO_PIN_PG(03), L"uart1");
  SunxiGpioMuxSetFunction(SUNXI_GPIO_PIN_PG(04), L"uart1");
}

// TODO: should move this into SunxiUartLib
VOID SplXW711InitUart()
{
  // Set uart to 115200n8

  // Disable all interrupts
  MmioWrite32(UART_BASE + 4, 0);

  // Enable DLAB
  MmioOr32(UART_BASE + 12, 0x80);

  // Divisor = Input / (16 * baud)
  // Input clock is 24Mhz
  // Divisor = 24000000 / (16 * 115200)
  UINT16 Divisor = 13;
  MmioWrite32(UART_BASE + 0, Divisor & 0xff);
  MmioWrite32(UART_BASE + 4, (Divisor >> 8) & 0xff);

  // 8 data bits, 1 stop bit, parity off, DLAB off
  MmioWrite32(UART_BASE + 12, 3);
  // Disable FIFO
  MmioWrite32(UART_BASE + 8, 0);
  // Disable flow control
  MmioWrite32(UART_BASE + 16, 0);
}

VOID SplHWInitBoard(VOID)
{
  // Enable UART1 clock
  MmioOr32(CCM_BASE + CCM_OFFSET_APB1_GATING, 1 << CCM_APB1_GATE_UART1);
  SplXW711InitGpio();
  SplXW711InitUart();
}