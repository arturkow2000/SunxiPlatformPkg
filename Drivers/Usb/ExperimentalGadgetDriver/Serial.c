#include "Driver.h"
#include <Guid/SerialPortLibVendor.h>

EFI_STATUS
EFIAPI
SerialReset (
  IN EFI_SERIAL_IO_PROTOCOL *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SerialSetAttributes (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN UINT64                 BaudRate,
  IN UINT32                 ReceiveFifoDepth,
  IN UINT32                 Timeout,
  IN EFI_PARITY_TYPE        Parity,
  IN UINT8                  DataBits,
  IN EFI_STOP_BITS_TYPE     StopBits
  ) {
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SerialSetControl (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN UINT32                 Control
  ) {
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
SerialGetControl (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  OUT UINT32                *Control
  ) {
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
SerialWrite (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN OUT UINTN              *BufferSize,
  IN VOID                   *Buffer
  ) {
  GADGET_DRIVER *Driver = SERIAL_TO_DRIVER(This);
  return CdcWrite(&Driver->Gadget, Buffer, BufferSize);
}

EFI_STATUS
EFIAPI
SerialRead (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN OUT UINTN              *BufferSize,
  OUT VOID                  *Buffer
  ) {
  GADGET_DRIVER *Driver = SERIAL_TO_DRIVER(This);
  return CdcRead(&Driver->Gadget, Buffer, BufferSize);
}

SERIAL_DEVICE_PATH mSerialDevicePath = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, { sizeof (VENDOR_DEVICE_PATH), 0} },
    EDKII_SERIAL_PORT_LIB_VENDOR_GUID
  },
  {
    { MESSAGING_DEVICE_PATH, MSG_UART_DP, { sizeof (UART_DEVICE_PATH), 0} },
    0,                  // Reserved
    0,                  // BaudRate
    0,                  // DataBits
    0,                  // Parity
    0                   // StopBits
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 } }
};

//
// Template used to initialize the Serial IO protocols.
//
EFI_SERIAL_IO_MODE mSerialIoMode = {
  //
  //    value  field                set in SerialDxeInitialize()?
  //---------  -------------------  -----------------------------
            0, // ControlMask
  1000 * 1000, // Timeout
            115200, // BaudRate          yes
            1, // ReceiveFifoDepth
            8, // DataBits          yes
            0, // Parity            yes
            1  // StopBits          yes
};

EFI_SERIAL_IO_PROTOCOL mSerialIoTemplate = {
  SERIAL_IO_INTERFACE_REVISION,
  SerialReset,
  SerialSetAttributes,
  SerialSetControl,
  SerialGetControl,
  SerialWrite,
  SerialRead,
  &mSerialIoMode
};

STATIC VOID UsbSerialUpdatePath(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);

  Driver->SerialDevicePath.Uart.BaudRate = 115200;
  Driver->SerialDevicePath.Uart.DataBits = 8;
  Driver->SerialDevicePath.Uart.Parity = 1;
  Driver->SerialDevicePath.Uart.StopBits = 1;
}

EFI_STATUS UsbSerialInit(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  EFI_STATUS Status;

  if (Driver->SerialHandle) {
    // FIXME: temporary workaround to avoid installing USB serial multiple times.
    // USB serial should be uninstalled as soon as USB is disconnected.
    /*Status = UsbSerialDestroy(This);
    if (EFI_ERROR(Status))
      return Status;*/

    return EFI_SUCCESS;
  }
  CopyMem(&Driver->SerialProtocol, &mSerialIoTemplate, sizeof(mSerialIoTemplate));
  CopyMem(&Driver->SerialDevicePath, &mSerialDevicePath, sizeof(mSerialDevicePath));

  UsbSerialUpdatePath(This);

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Driver->SerialHandle,
    &gEfiSerialIoProtocolGuid, &Driver->SerialProtocol,
    &gEfiDevicePathProtocolGuid, &Driver->SerialDevicePath,
    NULL
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to install USB serial\n"));
    // May fail if SerialDxe is already installed so don't assert.
    // TODO: should add better feature control to make it possible to disable CDC
    // completely
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS UsbSerialDestroy(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  EFI_STATUS Status;

  Status = gBS->UninstallMultipleProtocolInterfaces(
    Driver->SerialHandle,
    &gEfiSerialIoProtocolGuid, &Driver->SerialProtocol,
    &gEfiDevicePathProtocolGuid, &Driver->SerialDevicePath,
    NULL
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to uninstall USB serial\n"));
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  Driver->SerialHandle = NULL;
  return EFI_SUCCESS;
}
