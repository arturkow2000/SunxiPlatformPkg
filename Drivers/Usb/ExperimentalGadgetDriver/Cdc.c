#include "Driver.h"

STATIC VOID CdcHandleLineCoding(USB_GADGET *This, USB_REQUEST_BLOCK *Urb) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  USB_CDC_LINE_CODING *LineCoding = &Driver->CdcState.PendingLineCoding;

  if (Urb->Length != sizeof(USB_CDC_LINE_CODING)) {
    DEBUG((EFI_D_ERROR, "Invalid CDC set line coding request: expected len=%d, got %d\n", Urb->Length, sizeof(USB_CDC_LINE_CODING)));
    UsbGadgetHaltEndpoint(This, 0);
    return;
  }

  DEBUG((EFI_D_INFO, "Rate: %d\n", LineCoding->RteRate));
  switch(LineCoding->ParityType) {
  case USB_CDC_NO_PARITY:
    DEBUG((EFI_D_INFO, "Parity: None\n"));
    break;
  case USB_CDC_ODD_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Odd\n"));
    break;
  case USB_CDC_EVEN_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Even\n"));
    break;
  case USB_CDC_MARK_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Mark\n"));
    break;
  case USB_CDC_SPACE_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Space\n"));
    break;
  default:
    DEBUG((EFI_D_INFO, "Parity: Unknown\n"));
    break;
  }
  switch(LineCoding->CharFormat) {
  case USB_CDC_1_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 1\n"));
    break;
  case USB_CDC_1_5_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 1.5\n"));
    break;
  case USB_CDC_2_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 2\n"));
    break;
  default:
    DEBUG((EFI_D_INFO, "Stop bits: unknown\n"));
    break;
  }
  DEBUG((EFI_D_INFO, "Data bits: %d\n", LineCoding->DataBits));

  CopyMem(&Driver->CdcState.LineCoding, &Driver->CdcState.PendingLineCoding, sizeof(USB_CDC_LINE_CODING));
}

STATIC VOID CdcHandleData(USB_GADGET *This, USB_REQUEST_BLOCK *Urb);
STATIC VOID CdcTxComplete(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  if (EFI_ERROR(Urb->Status))
    DEBUG((EFI_D_ERROR, "CDC TX failed: %r\n", Urb->Status));

  Status = UsbGadgetQueue(
    This,
    Config->CdcDataEpOut.EndpointAddress,
    Driver->CdcDataOutUrb,
    Driver->CdcBuffer,
    CDC_DATA_MAX_PACKET,
    0,
    CdcHandleData
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to queue CDC RX transfer: %r\n", Status));
  }
}

STATIC VOID CdcHandleData(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  if (Urb->Actual > 0 && Urb->Status == EFI_SUCCESS) {
    Status = UsbGadgetQueue(
      This,
      Config->CdcDataEpIn.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
      Driver->CdcDataInUrb,
      Driver->CdcBuffer,
      Urb->Actual,
      USB_FLAG_TX | USB_FLAG_ZLP,
      CdcTxComplete
    );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to queue CDC TX transfer: %r\n", Status));
    }
  } else {
    Status = UsbGadgetQueue(This,
      Config->CdcDataEpOut.EndpointAddress,
      Driver->CdcDataOutUrb,
      Driver->CdcBuffer,
      CDC_DATA_MAX_PACKET,
      0,
      CdcHandleData
    );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to queue CDC RX transfer: %r\n", Status));
    }
  }
}

EFI_STATUS CdcHandleRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);

  switch (Request->Request)
  {
  case USB_CDC_REQ_SET_LINE_CODING:
    if (Request->RequestType & USB_ENDPOINT_DIR_IN)
      return EFI_DEVICE_ERROR;

    return UsbGadgetInitUrb(This, This->ControlUrb, &Driver->CdcState.PendingLineCoding, sizeof(USB_CDC_LINE_CODING), 0, CdcHandleLineCoding);
  
  case USB_CDC_REQ_GET_LINE_CODING:
    if (!(Request->RequestType & USB_ENDPOINT_DIR_IN))
      return EFI_DEVICE_ERROR;

    return UsbGadgetInitUrb(This, This->ControlUrb, &Driver->CdcState.LineCoding, sizeof(USB_CDC_LINE_CODING), 0, NULL);
  
  default:
    DEBUG((EFI_D_ERROR, "Unknown CDC request 0x%d\n", Request->Request));
    return EFI_DEVICE_ERROR;
  }
}

EFI_STATUS CdcEnable(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  Status = UsbGadgetEnableEndpoint(This, &Config->CdcControlEp);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable CDC control endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->CdcDataEpOut);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable CDC data out endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->CdcDataEpIn);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable CDC data in endpoint\n"));
    return Status;
  }

  Status = UsbGadgetQueue(This,
    Config->CdcDataEpOut.EndpointAddress,
    Driver->CdcDataOutUrb,
    Driver->CdcBuffer,
    CDC_DATA_MAX_PACKET,
    0,
    CdcHandleData
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to initialize CDC data transfer\n"));
    return Status;
  }

  return EFI_SUCCESS;
}
