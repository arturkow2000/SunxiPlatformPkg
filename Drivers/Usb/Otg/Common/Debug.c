#include "Driver.h"

STATIC VOID UsbDumpRecipient(USB_DEVICE_REQUEST *Setup) {
  switch (Setup->RequestType & USB_RECIP_MASK) {
  case USB_RECIP_DEVICE:
    DEBUG((EFI_D_INFO, "(DEVICE) "));
    break;
  case USB_RECIP_INTERFACE:
    DEBUG((EFI_D_INFO, "(INTERFACE=%d) ", Setup->Index));
    break;
  case USB_RECIP_ENDPOINT:
    DEBUG((EFI_D_INFO, "(ENDPOINT=%d) ", Setup->Index));
    break;
  default:
    DEBUG((EFI_D_INFO, "(UNK:0x%x) ", Setup->RequestType & USB_RECIP_MASK));
    break;
  }
}

VOID UsbDumpSetupPacket(USB_DEVICE_REQUEST *Setup) {
  switch (Setup->RequestType & USB_TYPE_MASK) {
  case USB_REQ_TYPE_STANDARD: DEBUG((EFI_D_INFO, "SETUP STD")); UsbDumpRecipient(Setup); break;
  case USB_REQ_TYPE_CLASS: DEBUG((EFI_D_INFO, "SETUP CLASS")); UsbDumpRecipient(Setup); break;
  case USB_REQ_TYPE_VENDOR: DEBUG((EFI_D_INFO, "SETUP VENDOR")); UsbDumpRecipient(Setup); break;
  default: DEBUG((EFI_D_INFO, "SETUP type 0x%x ", Setup->RequestType &USB_TYPE_MASK)); break;
  }

  if ((Setup->RequestType & USB_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
    switch (Setup->Request) {
    case USB_REQ_GET_STATUS: DEBUG((EFI_D_INFO, "GET_STATUS ")); break;
    case USB_REQ_CLEAR_FEATURE: DEBUG((EFI_D_INFO, "CLEAR_FEATURE ")); break;
    case USB_REQ_SET_FEATURE: DEBUG((EFI_D_INFO, "SET_FEATURE ")); break;
    case USB_REQ_SET_ADDRESS: DEBUG((EFI_D_INFO, "SET_ADDRESS ")); break;
    case USB_REQ_GET_DESCRIPTOR: DEBUG((EFI_D_INFO, "GET_DESCRIPTOR ")); break;
    case USB_REQ_SET_DESCRIPTOR: DEBUG((EFI_D_INFO, "SET_DESCRIPTOR ")); break;
    case USB_REQ_GET_CONFIG: DEBUG((EFI_D_INFO, "GET_CONFIG ")); break;
    case USB_REQ_SET_CONFIG: DEBUG((EFI_D_INFO, "SET_CONFIG ")); break;
    case USB_REQ_GET_INTERFACE: DEBUG((EFI_D_INFO, "GET_INTERFACE ")); break;
    case USB_REQ_SET_INTERFACE: DEBUG((EFI_D_INFO, "SET_INTERFACE ")); break;
    case USB_REQ_SYNCH_FRAME: DEBUG((EFI_D_INFO, "SYNCH_FRAME ")); break;
    default: DEBUG((EFI_D_INFO, "req 0x%x ", Setup->Request)); break;
    }
  } else DEBUG((EFI_D_INFO, "req 0x%x ", Setup->Request));

  DEBUG((EFI_D_INFO, "idx 0x%x val 0x%x len 0x%x\n", Setup->Index, Setup->Value, Setup->Length));
}

CONST CHAR16 *UsbEp0StateStr(USB_DRIVER *Driver) {
  switch (Driver->Ep0State)
  {
  case MUSB_EP0_STAGE_IDLE:
    return L"idle";
  case MUSB_EP0_STAGE_SETUP:
    return L"setup";
  case MUSB_EP0_STAGE_TX:
    return L"tx";
  case MUSB_EP0_STAGE_RX:
    return L"rx";
  case MUSB_EP0_STAGE_STATUSIN:
    return L"statusin";
  case MUSB_EP0_STAGE_STATUSOUT:
    return L"statusout";
  case MUSB_EP0_STAGE_ACKWAIT:
    return L"ackwait";
  default:
    return NULL;
  }
}
