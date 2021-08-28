#include "Driver.h"

/*STATIC USB_DEVICE_DESCRIPTOR mDeviceDescriptor = {
  .Length = sizeof(USB_DEVICE_DESCRIPTOR),
  .DescriptorType = USB_DT_DEVICE,
  .BcdUSB = 0x200,
  .DeviceClass = USB_CLASS_COMM,
  .DeviceSubClass = 0,
  .DeviceProtocol = 0,
  .IdVendor = 0x4444,
  .IdProduct = 0x5555,
  .StrManufacturer = 1,
  .StrProduct = 1,
  .NumConfigurations = 1,
  .MaxPacketSize0 = 64
};

STATIC USB_REQUEST mRq;*/

INT8 UsbForwardControlRequestToGadgetDriver(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Request) {
  EFI_STATUS Status;
  USB_PEI_DRIVER *PeiDriver = USB_DRIVER_INTO_PEI_DRIVER(Driver);

#if 0
  if (Request->Request == USB_REQ_GET_DESCRIPTOR) {
    mRq.Zero = 0;
    mRq.Buffer = (VOID*)&mDeviceDescriptor;
    mRq.Length = sizeof(USB_DEVICE_DESCRIPTOR);
    mRq.OutStatus = NULL;

    switch(Request->Value >> 8) {
    case USB_DT_DEVICE:
      Status = UsbEp0QueuePacket(Driver, &mRq);
      ASSERT_EFI_ERROR(Status);
      break;
    default:
      DEBUG((EFI_D_ERROR, "Unknown descriptor type %d\n", Request->Value >> 8));
      return -1;
    }

    return 1;
  } else DEBUG((EFI_D_ERROR, "Unknown request type %d\n", Request->Request));

  return 0;
#endif

  #if 1
  ASSERT(PeiDriver->GadgetDriver);
  ASSERT(PeiDriver->GadgetDriver->HandleControlRequest);

  Status = PeiDriver->GadgetDriver->HandleControlRequest(PeiDriver->GadgetDriver, (USB_DRIVER*)&PeiDriver->Ppi, Request);
  if (Status == EFI_SUCCESS)
    return 1;
  else if (Status == EFI_UNSUPPORTED)
    return 0;
  else return -1;
  #endif

  /*  EFI_STATUS Status;

    if (Request->Request == USB_REQ_GET_DESCRIPTOR) {
        // if (Request->RequestType != USB_DIR_IN)

        switch(Request->Value >> 8) {
        case USB_DT_DEVICE:
            UsbInitRequest(&mRq, (VOID*)&mDeviceDescriptor, sizeof(USB_DEVICE_DESCRIPTOR));
            Status = UsbEp0QueuePacket(Driver, &mRq);
            ASSERT_EFI_ERROR(Status);
            break;
        case USB_DT_CONFIG:
            break;
        default:
            DEBUG((EFI_D_ERROR, "Unknown descriptor type %d\n", Request->Value >> 8));
            break;
        }

        return 1;
    }
    return -1;*/
}