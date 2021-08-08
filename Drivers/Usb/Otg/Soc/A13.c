#include "../Common/Driver.h"

STATIC CONST USB_EP_FIFO_CONFIG mEpFifoConfig[] = {
  { .TxMaxPacketSizeLog2 = 6, .RxMaxPacketSizeLog2 = 6, .Flags = USB_FIFO_TX | USB_FIFO_RX | USB_FIFO_SHARED }, /* EP0 */
  { .TxMaxPacketSizeLog2 = 9, .RxMaxPacketSizeLog2 = 9, .Flags = USB_FIFO_TX | USB_FIFO_RX }, /* EP1 */
  { .TxMaxPacketSizeLog2 = 9, .RxMaxPacketSizeLog2 = 9, .Flags = USB_FIFO_TX | USB_FIFO_RX }, /* EP2 */
  { .TxMaxPacketSizeLog2 = 9, .RxMaxPacketSizeLog2 = 9, .Flags = USB_FIFO_TX | USB_FIFO_RX }, /* EP3 */
  { .TxMaxPacketSizeLog2 = 9, .RxMaxPacketSizeLog2 = 9, .Flags = USB_FIFO_TX | USB_FIFO_RX }, /* EP4 */
  { .TxMaxPacketSizeLog2 = 9, .RxMaxPacketSizeLog2 = 9, .Flags = USB_FIFO_TX | USB_FIFO_RX }, /* EP5 */
};

SUNXI_SOC_CONFIG gSunxiSocConfig = {
  .Flags = 0,
  .NumEndpoints = ARRAY_SIZE(mEpFifoConfig),
  .EpFifoConfig = mEpFifoConfig
};