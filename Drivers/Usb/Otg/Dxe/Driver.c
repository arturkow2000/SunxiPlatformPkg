#include "Driver.h"

STATIC EFI_STATUS RegisterGadgetDriver(
  IN USB_PROTOCOL *This,
  IN USB_GADGET_INTERFACE *Gadget,
  IN VOID *UserData
) {
  EFI_STATUS Status;
  USB_DXE_DRIVER *Driver;
  
  if (!This || !Gadget)
    return EFI_INVALID_PARAMETER;
  
  Driver = GET_DXE_DRIVER(This);

  if (Driver->GadgetDriver)
    return EFI_ALREADY_STARTED;

  // Timer is disabled at this stage so all accesses are safe without a lock.
  Driver->GadgetDriver = Gadget;

  UsbEnable(&Driver->Common);
  Status = gBS->SetTimer(Driver->Timer, TimerPeriodic, EFI_TIMER_PERIOD_MILLISECONDS(1));
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS UnregisterGadgetDriver(
  IN USB_PROTOCOL *This,
  IN USB_GADGET_INTERFACE *Gadget
) {
  USB_DXE_DRIVER *Driver;
  EFI_STATUS Status;

  if (!This || !Gadget)
    return EFI_INVALID_PARAMETER;
  
  Driver = GET_DXE_DRIVER(This);

  if (Driver->GadgetDriver == Gadget) {
    // Disable timer before accessing shared data
    Status = gBS->SetTimer(Driver->Timer, TimerCancel, 0);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;

    UsbDisable(&Driver->Common);
    Driver->GadgetDriver = NULL;

    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

STATIC EFI_STATUS GetNumberOfEndpoints(
  IN USB_PROTOCOL *This,
  OUT UINT32 *OutNumEndpoints
) {
  USB_DXE_DRIVER *Driver;
  
  if (!This || !OutNumEndpoints)
    return EFI_INVALID_PARAMETER;
  
  Driver = GET_DXE_DRIVER(This);

  *OutNumEndpoints = gSunxiSocConfig.NumEndpoints;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS GetEndpointInfo(
  IN USB_PROTOCOL *This,
  IN UINT32 Endpoint,
  OUT UINT32 *OutMaxPacketSizeTx,
  OUT UINT32 *OutMaxPacketSizeRx
) {
  USB_DXE_DRIVER *Driver;

  if (!This || !(OutMaxPacketSizeTx || OutMaxPacketSizeRx))
    return EFI_INVALID_PARAMETER;
  
  Driver = GET_DXE_DRIVER(This);

  if (Endpoint >= gSunxiSocConfig.NumEndpoints)
    return EFI_NOT_FOUND;

  if (OutMaxPacketSizeTx)
    *OutMaxPacketSizeTx = 1u << (UINT32)gSunxiSocConfig.EpFifoConfig[Endpoint].TxMaxPacketSizeLog2;

  if (OutMaxPacketSizeRx)
    *OutMaxPacketSizeRx = 1u << (UINT32)gSunxiSocConfig.EpFifoConfig[Endpoint].RxMaxPacketSizeLog2;
  
  return EFI_SUCCESS;
}

STATIC EFI_STATUS Queue(
  IN USB_PROTOCOL *This,
  IN UINT32 Endpoint,
  IN USB_REQUEST_BLOCK *Urb
) {
  USB_DXE_DRIVER *Driver;
  EFI_STATUS Status;

  if (!This || !Urb)
    return EFI_INVALID_PARAMETER;

  if (Endpoint >= gSunxiSocConfig.NumEndpoints)
    return EFI_NOT_FOUND;

  Driver = GET_DXE_DRIVER(This);

  if (Endpoint == 0)
    Status = UsbEp0QueuePacket(&Driver->Common, Urb);
  else
    Status = UsbEpxQueue(&Driver->Common, Endpoint, Urb);

  return Status;
}

STATIC EFI_STATUS Halt(
  IN USB_PROTOCOL *This,
  IN UINT32 Endpoint,
  IN BOOLEAN Halt
) {
  USB_DXE_DRIVER *Driver;

  if (!This)
    return EFI_INVALID_PARAMETER;

  Driver = GET_DXE_DRIVER(This);

  if (Endpoint == 0) {
    if (!Halt) {
      DEBUG((EFI_D_ERROR, "Cannot unhalt EP0\n"));
      return EFI_INVALID_PARAMETER;
    }

    return UsbEp0Halt(&Driver->Common);
  } else {
    return UsbEpxHalt(&Driver->Common, Endpoint, Halt);
  }
}

STATIC EFI_STATUS EnableEndpoint(
  IN USB_PROTOCOL *This,
  IN USB_ENDPOINT_DESCRIPTOR *Descriptor
) {
  USB_DXE_DRIVER *Driver;

  if (!This)
    return EFI_INVALID_PARAMETER;

  Driver = GET_DXE_DRIVER(This);

  return UsbEnableEndpoint(&Driver->Common, Descriptor);
}

STATIC USB_PROTOCOL mUsbProto = {
  RegisterGadgetDriver,
  UnregisterGadgetDriver,
  GetNumberOfEndpoints,
  GetEndpointInfo,
  Queue,
  Halt,
  EnableEndpoint,
};

STATIC VOID TimerHandler (
  IN EFI_EVENT Event,
  IN USB_DXE_DRIVER *Driver
  )
{
  // Already running at TPL_NOTIFY, there is no need to acquire lock
  UsbHandleInterrupt(&Driver->Common);
}


EFI_STATUS EFIAPI OtgDxeInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  USB_DXE_DRIVER *Driver;
  EFI_HANDLE Handle = NULL;

#if FixedPcdGet32(UsbPhySupport)
  Status = SunxiUsbPhyInit();
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;
#endif

  Driver = AllocatePool(sizeof(USB_DXE_DRIVER));
  if (!Driver) {
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  ZeroMem(&Driver->Common, sizeof(USB_DRIVER));
  Driver->GadgetDriver = NULL;
  Status = gBS->CreateEvent(
    EVT_TIMER | EVT_NOTIFY_SIGNAL,
    TPL_NOTIFY,
    (EFI_EVENT_NOTIFY)TimerHandler,
    Driver,
    &Driver->Timer
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto error;

  Status = UsbInit(&Driver->Common);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto error;

  CopyMem(&Driver->Proto, &mUsbProto, sizeof(USB_PROTOCOL));

  gBS->InstallMultipleProtocolInterfaces(
    &Handle,
    &gUsbProtocolGuid, &Driver->Proto,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto error;

  return EFI_SUCCESS;
error:
  FreePool(Driver);
  return Status;
}
