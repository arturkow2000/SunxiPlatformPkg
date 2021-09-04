#include "Driver.h"

VOID UsbDumpSetupPacket(USB_DEVICE_REQUEST *Setup) {
  switch (Setup->RequestType & USB_TYPE_MASK) {
  case USB_REQ_TYPE_STANDARD: DEBUG((EFI_D_INFO, "SETUP STD ")); break;
  case USB_REQ_TYPE_CLASS: DEBUG((EFI_D_INFO, "SETUP CLASS ")); break;
  case USB_REQ_TYPE_VENDOR: DEBUG((EFI_D_INFO, "SETUP VENDOR ")); break;
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
