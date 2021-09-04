#include "Driver.h"

STATIC EFI_STATUS RegisterGadgetDriver(
  IN USB_PPI *This,
  IN USB_GADGET_DRIVER *Gadget,
  IN VOID *UserData
) {
  USB_PEI_DRIVER *Driver;
  
  if (!This || !Gadget)
    return EFI_INVALID_PARAMETER;
  
  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  if (Driver->GadgetDriver)
    return EFI_ALREADY_STARTED;

  Driver->GadgetDriver = Gadget;
  Driver->GadgetDriverData = UserData;

  UsbEnable(&Driver->Common);

  return EFI_SUCCESS;
}

STATIC EFI_STATUS UnregisterGadgetDriver(
  IN USB_PPI *This,
  IN USB_GADGET_DRIVER *Gadget
) {
  USB_PEI_DRIVER *Driver;
  
  if (!This || !Gadget)
    return EFI_INVALID_PARAMETER;
  
  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  if (Driver->GadgetDriver == Gadget) {
    UsbDisable(&Driver->Common);
    Driver->GadgetDriver = NULL;
    Driver->GadgetDriverData = NULL;

    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

STATIC EFI_STATUS HandleInterrupts(
  IN USB_PPI *This
) {
  USB_PEI_DRIVER *Driver;
  
  if (!This)
    return EFI_INVALID_PARAMETER;
  
  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  UsbHandleInterrupt(&Driver->Common);

  return EFI_SUCCESS;
}

STATIC EFI_STATUS GetNumberOfEndpoints(
  IN USB_PPI *This,
  OUT UINT32 *OutNumEndpoints
) {
  USB_PEI_DRIVER *Driver;
  
  if (!This || !OutNumEndpoints)
    return EFI_INVALID_PARAMETER;
  
  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  *OutNumEndpoints = gSunxiSocConfig.NumEndpoints;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS GetEndpointInfo(
  IN USB_PPI *This,
  IN UINT32 Endpoint,
  OUT UINT32 *OutMaxPacketSizeTx,
  OUT UINT32 *OutMaxPacketSizeRx
) {
  USB_PEI_DRIVER *Driver;
  
  if (!This || !(OutMaxPacketSizeTx || OutMaxPacketSizeRx))
    return EFI_INVALID_PARAMETER;
  
  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  if (Endpoint >= gSunxiSocConfig.NumEndpoints)
    return EFI_NOT_FOUND;

  if (OutMaxPacketSizeTx)
    *OutMaxPacketSizeTx = 1u << (UINT32)gSunxiSocConfig.EpFifoConfig[Endpoint].TxMaxPacketSizeLog2;

  if (OutMaxPacketSizeRx)
    *OutMaxPacketSizeRx = 1u << (UINT32)gSunxiSocConfig.EpFifoConfig[Endpoint].RxMaxPacketSizeLog2;
  
  return EFI_SUCCESS;
}

STATIC EFI_STATUS AllocateRequests(
  IN USB_PPI *This,
  IN UINT32 NumRequests,
  OUT USB_REQUEST **OutRequests
) {
  USB_PEI_DRIVER *Driver;
  USB_REQUEST *Requests;
  
  if (!This || !OutRequests || NumRequests == 0)
    return EFI_INVALID_PARAMETER;

  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  Requests = AllocateZeroPool(sizeof(USB_REQUEST) * NumRequests);
  if (!Requests)
    return EFI_OUT_OF_RESOURCES;

  *OutRequests = Requests;
  return EFI_SUCCESS;
}

#define ALL_VALID_FLAGS (USB_PPI_FLAG_ZLP)

STATIC EFI_STATUS InitRequest(
  IN USB_PPI *This,
  IN OUT USB_REQUEST *Request,
  IN VOID *Buffer,
  IN UINT32 Length,
  IN UINT32 Flags,
  IN USB_PPI_REQ_COMPLETE_CALLBACK Callback OPTIONAL,
  IN VOID *UserData OPTIONAL
) {
  USB_PEI_DRIVER *Driver;
  
  if (!This || !Request)
    return EFI_INVALID_PARAMETER;

  if ((!Buffer && Length > 0) || (Buffer && Length == 0))
    return EFI_INVALID_PARAMETER;

  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  Request->Buffer = Buffer;
  Request->Length = Length;
  Request->Zero = !!(Flags & USB_PPI_FLAG_ZLP);
  Request->Callback = Callback;
  Request->UserData = UserData;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS Ep0Queue(
  IN USB_PPI *This,
  IN USB_REQUEST *Request
) {
  USB_PEI_DRIVER *Driver;

  if (!This || !Request)
    return EFI_INVALID_PARAMETER;

  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  return UsbEp0QueuePacket(&Driver->Common, Request);
}

STATIC EFI_STATUS Halt(
  IN USB_PPI *This,
  IN UINT32 Endpoint
) {
  USB_PEI_DRIVER *Driver;

  if (!This)
    return EFI_INVALID_PARAMETER;

  Driver = USB_PPI_INTO_PEI_DRIVER(This);

  if (Endpoint == 0) {
    return UsbEp0Halt(&Driver->Common);
  } else {
    // TODO: support other endpoints
    ASSERT(0);
    return EFI_NOT_FOUND;
  }
}

STATIC USB_PPI mUsbPpi = {
  RegisterGadgetDriver,
  UnregisterGadgetDriver,
  HandleInterrupts,
  GetNumberOfEndpoints,
  GetEndpointInfo,
  AllocateRequests,
  InitRequest,
  Ep0Queue,
  Halt
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPpiDesc;

EFI_STATUS
EFIAPI
OtgPeiInitialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
  USB_PEI_DRIVER *Driver;

  Status = PeiServicesAllocatePool(sizeof(USB_PEI_DRIVER), (VOID**)&Driver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  ZeroMem(&Driver->Common, sizeof(USB_DRIVER));
  Driver->Common.Base = 0x01c13000;
  Driver->GadgetDriver = NULL;
  Driver->GadgetDriverData = NULL;

  Status = UsbInit(&Driver->Common);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto error;

  CopyMem(&Driver->Ppi, &mUsbPpi, sizeof(USB_PPI));

  mPpiDesc.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  mPpiDesc.Guid = &gUsbPpiGuid;
  mPpiDesc.Ppi = &Driver->Ppi;

  Status = PeiServicesInstallPpi(&mPpiDesc);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto error;

  return EFI_SUCCESS;

error:
  return Status;
}
