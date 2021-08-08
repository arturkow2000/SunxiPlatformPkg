#include "Driver.h"
#include "Hw.h"
#include <IndustryStandard/Usb.h>

#define USB_RECIP_DEVICE                0x00
#define USB_RECIP_INTERFACE             0x01
#define USB_RECIP_ENDPOINT              0x02

STATIC VOID UsbReadSetup(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Setup) {
  UsbReadFifo(Driver, 0, sizeof *Setup, Setup);

  DEBUG((
    EFI_D_INFO,
    "SETUP req%02x.%02x v%04x i%04x l%d\n",
    Request.RequestType,
    Request.Request,
    Request.Value,
    Request.Index,
    Request.Length
  ));

  /* clean up any leftover transfers */
  /// TODO: implement
  //r = next_ep0_request(musb);
  //if (r)
  //  musb_g_ep0_giveback(musb, &r->request);

  /* For zero-data requests we want to delay the STATUS stage to
   * avoid SETUPEND errors.  If we read data (OUT), delay accepting
   * packets until there's a buffer to store them in.
   *
   * If we write data, the controller acts happier if we enable
   * the TX FIFO right away, and give the controller a moment
   * to switch modes...
   */

  Driver->SetAddress = FALSE;
  Driver->AckPending = MUSB_CSR0_P_SVDRXPKTRDY;
  if (Setup->Length == 0) {
    if (Setup->RequestType & USB_ENDPOINT_DIR_IN)
      Driver->AckPending |= MUSB_CSR0_TXPKTRDY;
    Driver->Ep0State = MUSB_EP0_STAGE_ACKWAIT;
  } else if (Setup->RequestType & USB_ENDPOINT_DIR_IN) {
    Driver->Ep0State = MUSB_EP0_STAGE_TX;

    MmioWrite16(Driver->Base + MUSB_CSR0, MUSB_CSR0_P_SVDRXPKTRDY);
    while ((MmioRead16(Driver->Base + MUSB_CSR0) & MUSB_CSR0_RXPKTRDY) != 0)
      CpuPause();

    Driver->AckPending = 0;
  } else
    Driver->Ep0State = MUSB_EP0_STAGE_RX;
}

/*
 * Handle all control requests with no DATA stage, including standard
 * requests such as:
 * USB_REQ_SET_CONFIGURATION, USB_REQ_SET_INTERFACE, unrecognized
 *	always delegated to the gadget driver
 * USB_REQ_SET_ADDRESS, USB_REQ_CLEAR_FEATURE, USB_REQ_SET_FEATURE
 *	always handled here
 *
 */
STATIC INT8 UsbServiceZeroDataRequest(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Setup) {
  UINT8 Recip = Setup->RequestType & 0x1f;
  INT8 Status = -1;
  UINT8 Endpoint;
  UINT16 Csr;

  /* the gadget driver handles everything except what we MUST handle */
  if ((Setup->RequestType & (3 << 5)) == USB_REQ_TYPE_STANDARD) {
    switch (Setup->Request) {
    case USB_REQ_SET_ADDRESS:
      /* change it after the status stage */
      Driver->SetAddress = TRUE;
      Driver->Address = Setup->Value & 0x7f;
      break;

    case USB_REQ_CLEAR_FEATURE:
      switch (Recip) {
      case USB_RECIP_DEVICE:
      case USB_RECIP_INTERFACE:
        break;
      case USB_RECIP_ENDPOINT:
        Endpoint = Setup->Index & 0x0f;
        if (Endpoint == 0 || Endpoint >= gSunxiSocConfig.NumEndpoints || Setup->Value == USB_FEATURE_ENDPOINT_HALT)
          break;

        Status = 1;

        UsbSelectEndpoint(Driver, Endpoint);
        if (Setup->Index & USB_ENDPOINT_DIR_IN) {
          //Csr = MmioRead16(Driver->Base + MUSB_TXCSR);
          //Csr |= 
          MmioAndThenOr16(
            Driver->Base + MUSB_TXCSR,
            ~(MUSB_TXCSR_P_SENDSTALL | MUSB_TXCSR_P_SENTSTALL | MUSB_TXCSR_TXPKTRDY),
            MUSB_TXCSR_CLRDATATOG | MUSB_TXCSR_P_WZC_BITS
          );
        }

        break;
      }
      break;
    }
  }

  return Status;
}

VOID UsbEp0HandleIrq(USB_DRIVER *Driver) {
  UINT16 Csr0;
  UINT16 Len;
  USB_DEVICE_REQUEST Setup;

  UsbSelectEndpoint(Driver, 0);

  Csr0 = MmioRead16(Driver->Base + MUSB_CSR0);
  Len = MmioRead16(Driver->Base + MUSB_COUNT0);

  DEBUG((
    EFI_D_INFO,
    "csr %04x, count %u, addr %u\n",
    Csr0, Len, MmioRead16(Driver->Base + MUSB_FADDR)
  ));

  switch (Driver->Ep0State) {
  case MUSB_EP0_STAGE_SETUP:
    if (Csr0 & MUSB_CSR0_RXPKTRDY) {
      if (Len != 8) {
        DEBUG((EFI_D_ERROR, "setup packet len %d != 8\n", Len));
        break;
      }
    }

    UsbReadSetup(Driver, &Setup);

    switch (Driver->Ep0State) {
    /* sequence #3 (no data stage), includes requests
     * we can't forward (notably SET_ADDRESS and the
     * device/endpoint feature set/clear operations)
     * plus SET_CONFIGURATION and others we must
     */
    case MUSB_EP0_STAGE_ACKWAIT:
      UsbServiceZeroDataRequest(Driver, &Setup);
      break;
    
    /* sequence #1 (IN to host), includes GET_STATUS
     * requests that we can't forward, GET_DESCRIPTOR
     * and others that we must
     */
    }

    break;
  default:
    DEBUG((EFI_D_ERROR, "Invalid EP0 state %u\n", Driver->Ep0State));
    break;
  }
}
