#pragma once

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/SunxiUsbPhyLib.h>
#include <Library/SunxiCcmLib.h>
#include <Library/UsbGadgetLib.h>
#include <Library/PcdLib.h>

#include <IndustryStandard/UsbEx.h>

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

// These flags must match with flags in UsbPpi.h
#define USB_FLAG_ZLP (1 << 0)
#define USB_FLAG_TX (1 << 1)

typedef struct _USB_REQUEST_BLOCK USB_REQUEST_BLOCK;

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

typedef struct _USB_EPX {
  LIST_ENTRY TxQueue;
  LIST_ENTRY RxQueue;
  UINT16 TxPacketSize;
  UINT16 RxPacketSize;
  BOOLEAN Busy;
} USB_EPX;

typedef struct _USB_DRIVER {
  UINT32 Base;
  UINT32 Phy;
  BOOLEAN Enabled;
  UINT8 Ep0State;
  UINT16 AckPending;
  BOOLEAN SetAddress;
  UINT8 Address;

  LIST_ENTRY Ep0Queue;

  UINT32 EndpointsUsedForTx;
  UINT32 EndpointsUsedForRx;

  USB_EPX *Epx;
} USB_DRIVER;

EFI_STATUS UsbInit(USB_DRIVER *Driver);
VOID UsbReset(USB_DRIVER *Driver, BOOLEAN FirstReset);
VOID UsbEnable(USB_DRIVER *Driver);
VOID UsbDisable(USB_DRIVER *Driver);
VOID UsbCoreInit(USB_DRIVER *Driver);
VOID UsbSetupFifo(USB_DRIVER *Driver);
VOID UsbHandleInterrupt(USB_DRIVER *Driver);
VOID UsbEp0HandleIrq(USB_DRIVER *Driver);
VOID UsbEpxHandleRxIrq(USB_DRIVER *Driver, UINT32 EndpointNumber);
VOID UsbEpxHandleTxIrq(USB_DRIVER *Driver, UINT32 EndpointNumber);
VOID UsbEpxRestart(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb);

// Internal APIs used for forwarding data to gadget driver
INT8 UsbForwardControlRequestToGadgetDriver(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Request);
VOID UsbSignalCompletion(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb, EFI_STATUS Status);

// Internal APIs
VOID UsbEp0CompleteRequest(USB_DRIVER *Driver, USB_REQUEST_BLOCK *Urb, EFI_STATUS Status);
VOID UsbEpxCompleteRequest(USB_DRIVER *Driver, USB_REQUEST_BLOCK *Urb, EFI_STATUS Status, UINT32 Endpoint);
EFI_STATUS UsbEp0QueuePacket(USB_DRIVER *Driver, USB_REQUEST_BLOCK *Urb);
EFI_STATUS UsbEp0Halt(USB_DRIVER *Driver);
EFI_STATUS UsbEnableEndpoint(USB_DRIVER *Driver, USB_ENDPOINT_DESCRIPTOR *Descriptor);
EFI_STATUS UsbEpxQueue(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb);

// FIFO access functions
VOID UsbReadFifo(USB_DRIVER *Driver, UINT8 Endpoint, UINT16 Length, UINT8* OutData);
VOID UsbWriteFifo(USB_DRIVER *Driver, UINT8 Endpoint, UINT16 Length, CONST UINT8 *Data);

// Debugging
VOID UsbDumpSetupPacket(USB_DEVICE_REQUEST *Setup);

