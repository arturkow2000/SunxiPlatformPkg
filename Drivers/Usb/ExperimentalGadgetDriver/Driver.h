#pragma once

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/NetLib.h>

#include <Library/UsbGadgetLib.h>
#include <Ppi/UsbPpi.h>

#include <IndustryStandard/UsbEx.h>
#include <IndustryStandard/CdcAcm.h>
#include <Protocol/SerialIo.h>
#include <Protocol/SimpleNetwork.h>

#include "Buffer.h"
#include "Rndis.h"

enum {
  // Due to bug in rndis6 driver RNDIS must be the first interface (interface 0
  // and 1). We also keep interfaces in the same order in configuration
  // descriptor to keep interface number and interface indices the same because
  // MOS descriptors refer to interface by its index instead of number.
  RNDIS_CONTROL_INTERFACE,
  RNDIS_DATA_INTERFACE,
  CUSTOM_INTERFACE,
  CDC_CONTROL_INTERFACE,
  CDC_DATA_INTEFACE,
  NUM_INTERFACES
};

#if FixedPcdGet32(UsbHighSpeedSupport)
#define CDC_DATA_MAX_PACKET 512
#else
#define CDC_DATA_MAX_PACKET 64
#endif

#define GADGET_TO_DRIVER(Record)        \
  BASE_CR(Record, GADGET_DRIVER, Gadget)

#define SERIAL_TO_DRIVER(Record)        \
  BASE_CR(Record, GADGET_DRIVER, SerialProtocol)

#define SNP_TO_DRIVER(Record)           \
  BASE_CR(Record, SNP_DRIVER, Protocol)

#define SNP_TO_GADGET_DRIVER(Record)    \
  BASE_CR(Record, GADGET_DRIVER, Snp)

//#define RNDIS_RX_HEADER 0
//#define RNDIS_RX_DATA 1

typedef struct {
  USB_REQUEST_BLOCK *DataInUrb;
  USB_REQUEST_BLOCK *DataInUrb2;
  USB_REQUEST_BLOCK *DataOutUrb;
  USB_REQUEST_BLOCK *InterruptUrb;
  SIMPLE_BUFFER TxBuffer;
  SIMPLE_BUFFER RxBuffer;
  UINT8 *RxBufferTemp;
  UINT8 *EncapsulatedMessage;
  UINT32 EncTxBufferSize;
  UINT32 RxProcessThr;
  BOOLEAN TxPending;
} RNDIS_DRIVER;

typedef struct {
  EFI_SIMPLE_NETWORK_PROTOCOL Protocol;
  EFI_SIMPLE_NETWORK_MODE Mode;
  EFI_HANDLE Handle;
  SIMPLE_BUFFER RxBuffer;
  struct {
    BOOLEAN Rx;
  } Interrupt;
} SNP_DRIVER;

typedef struct {
  USB_CDC_LINE_CODING PendingLineCoding;
  USB_CDC_LINE_CODING LineCoding;
} CDC_STATE;

typedef struct {
  VENDOR_DEVICE_PATH        Guid;
  UART_DEVICE_PATH          Uart;
  EFI_DEVICE_PATH_PROTOCOL  End;
} SERIAL_DEVICE_PATH;

typedef struct {
  VENDOR_DEVICE_PATH        Guid;
  VENDOR_DEVICE_PATH        Network;
  EFI_DEVICE_PATH_PROTOCOL  End;
} NETWORK_DEVICE_PATH;

typedef struct _GADGET_DRIVER {
  USB_GADGET Gadget;

  USB_REQUEST_BLOCK *CdcDataInUrb;
  USB_REQUEST_BLOCK *CdcDataOutUrb;

  volatile BOOLEAN CdcTxPending;
  SIMPLE_BUFFER CdcTxBuffer;
  SIMPLE_BUFFER CdcRxBuffer;
  UINT8 *CdcRxBufferTemp;

  RNDIS_DRIVER Rndis;
  SNP_DRIVER Snp;

  EFI_HANDLE SerialHandle;

  EFI_SERIAL_IO_PROTOCOL SerialProtocol;
  SERIAL_DEVICE_PATH SerialDevicePath;

  CDC_STATE CdcState;
  USB_CONFIG_DESCRIPTOR *ConfigDescriptor;

  UINT8 ActiveConfig;
} GADGET_DRIVER;

typedef struct _DEVICE_CONFIG {
  USB_CONFIG_DESCRIPTOR Config;
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR Iad1;
  USB_INTERFACE_DESCRIPTOR RndisControl;
  USB_CDC_FUNCTIONAL_DESCRIPTOR RndisFunctionalDescriptor;
  USB_CDC_CALL_MANAGEMENT_DESCRIPTOR RndisCallManagementDescriptor;
  USB_CDC_ACM_DESCRIPTOR RndisAcmDescriptor;
  USB_CDC_UNION_DESCRIPTOR RndisUnionDescriptor;
  USB_ENDPOINT_DESCRIPTOR RndisControlEp;
  USB_INTERFACE_DESCRIPTOR RndisDataInterface;
  USB_ENDPOINT_DESCRIPTOR RndisDataEpIn;
  USB_ENDPOINT_DESCRIPTOR RndisDataEpOut;
  // Interface with custom protocol used for controlling device
  // currently dummy,
  // it will be used to transfer EFI image when booting from USB
  // and possibly for debugging purposes
  //
  // CDC will be used to emulate serial port
  USB_INTERFACE_DESCRIPTOR Custom;
  USB_ENDPOINT_DESCRIPTOR CustomEpIn;
  USB_ENDPOINT_DESCRIPTOR CustomEpOut;
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR Iad0;
  USB_INTERFACE_DESCRIPTOR CdcAt;
  USB_CDC_FUNCTIONAL_DESCRIPTOR CdcFunctional;
  USB_CDC_CALL_MANAGEMENT_DESCRIPTOR CdcCallManagement;
  USB_CDC_ACM_DESCRIPTOR CdcAcm;
  USB_CDC_UNION_DESCRIPTOR CdcUnion;
  USB_ENDPOINT_DESCRIPTOR CdcControlEp;
  USB_INTERFACE_DESCRIPTOR CdcData;
  USB_ENDPOINT_DESCRIPTOR CdcDataEpIn;
  USB_ENDPOINT_DESCRIPTOR CdcDataEpOut;
} DEVICE_CONFIG;

extern DEVICE_CONFIG gConfigDescriptorTemplate;

USB_DEVICE_DESCRIPTOR *UsbGadgetGetDeviceDescriptor(USB_GADGET *Gadget);
USB_CONFIG_DESCRIPTOR *UsbGadgetGetConfigDescriptor(USB_GADGET *Gadget);
UINT8 *UsbGadgetGetStringDescriptor(USB_GADGET *Gadget, UINT8 Id);
EFI_STATUS UsbGadgetHandleClassRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request);
EFI_STATUS UsbGadgetHandleVendorRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request);
EFI_STATUS UsbGadgetHandleSetConfig(USB_GADGET *This, UINT8 Config);

EFI_STATUS CdcHandleRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request);
EFI_STATUS CdcEnable(USB_GADGET *This);
EFI_STATUS CdcDisable(USB_GADGET *This);
BOOLEAN CdcIsEnabled(USB_GADGET *This);
EFI_STATUS CdcQueueRead(USB_GADGET *This);
EFI_STATUS CdcRead(USB_GADGET *This, IN VOID *Buffer, IN OUT UINTN *BufferSize);
EFI_STATUS CdcWrite(USB_GADGET *This, IN VOID *Buffer, IN OUT UINTN *BufferSize);
EFI_STATUS CdcFlush(USB_GADGET *This);

EFI_STATUS UsbSerialInit(USB_GADGET *This);
EFI_STATUS UsbSerialDestroy(USB_GADGET *This);

EFI_STATUS RndisInit(USB_GADGET *This);
EFI_STATUS RndisEnable(USB_GADGET *This);
EFI_STATUS RndisDisable(USB_GADGET *This);
EFI_STATUS RndisHandleRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request);
EFI_STATUS RndisSendPacket(USB_GADGET *This, VOID *Payload, UINTN PayloadLength);


EFI_STATUS NetInitSnp(SNP_DRIVER *Driver);
VOID NetProcessEthernetFrame(SNP_DRIVER *Snp, SIMPLE_BUFFER *Buffer, UINTN Length);
UINT8* NetGetMacAddress();
