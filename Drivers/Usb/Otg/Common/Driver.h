#pragma once

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

#define MUSB_EP0_STAGE_IDLE 0      /* idle, waiting for SETUP */
#define MUSB_EP0_STAGE_SETUP 1     /* received SETUP */
#define MUSB_EP0_STAGE_TX 2        /* IN data */
#define MUSB_EP0_STAGE_RX 3        /* OUT data */
#define MUSB_EP0_STAGE_STATUSIN 4  /* (after OUT data) */
#define MUSB_EP0_STAGE_STATUSOUT 5 /* (after IN data) */
#define MUSB_EP0_STAGE_ACKWAIT 6   /* after zlp, before statusin */

// Newer allwinner SoCs don't have MUSB_CONFIGDATA register
#define SUNXI_USB_FIXED_CONFIG_DATA (1 << 0)

#define USB_FIFO_TX (1 << 0)
#define USB_FIFO_RX (1 << 1)
#define USB_FIFO_SHARED (1 << 2)

typedef struct _USB_EP_FIFO_CONFIG {
  UINT8 TxMaxPacketSizeLog2;
  UINT8 RxMaxPacketSizeLog2;
  UINT8 Flags;
} USB_EP_FIFO_CONFIG;

typedef struct _SUNXI_SOC_CONFIG {
  UINT32 Flags;
  // Number of endpoints including EP0
  UINT32 NumEndpoints;
  CONST USB_EP_FIFO_CONFIG *EpFifoConfig;
} SUNXI_SOC_CONFIG;

extern SUNXI_SOC_CONFIG gSunxiSocConfig;

typedef struct _USB_DRIVER {
  UINT32 Base;
  BOOLEAN Enabled;
  UINT8 Ep0State;
  UINT16 AckPending;
  BOOLEAN SetAddress;
  UINT8 Address;
} USB_DRIVER;

EFI_STATUS UsbInit(USB_DRIVER *Driver);
VOID UsbEnable(USB_DRIVER *Driver);
VOID UsbDisable(USB_DRIVER *Driver);
VOID UsbCoreInit(USB_DRIVER *Driver);
VOID UsbSetupFifo(USB_DRIVER *Driver);
VOID UsbHandleInterrupt(USB_DRIVER *Driver);
VOID UsbEp0HandleIrq(USB_DRIVER *Driver);

// FIFO access functions
VOID UsbReadFifo(USB_DRIVER *Driver, UINT8 Endpoint, UINT16 Length, UINT8* OutData);

/// FIXME: move PHY stuff into PHY driver
VOID USBC_EnableIdPullUp(USB_DRIVER *Driver);
VOID USBC_EnableDpDmPullUp(USB_DRIVER *Driver);
VOID USBC_ForceIdToLow(USB_DRIVER *Driver);
VOID USBC_ForceIdToHigh(USB_DRIVER *Driver);
VOID USBC_ForceVbusValidToLow(USB_DRIVER *Driver);
VOID USBC_ForceVbusValidToHigh(USB_DRIVER *Driver);
VOID USBC_ConfigFIFO_Base(VOID);
