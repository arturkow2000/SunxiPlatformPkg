#include "Driver.h"
#include "Hw.h"

// Transfer data to host
STATIC VOID UsbEp0TxState(USB_DRIVER *Driver) {
  LIST_ENTRY *Node;
  USB_REQUEST *Request;
  UINT8 *Source;
  UINT32 Count;
  UINT16 Csr = MUSB_CSR0_TXPKTRDY;
  UINT16 FifoSize = 1 << (UINT16)gSunxiSocConfig.EpFifoConfig[0].TxMaxPacketSizeLog2;

  Node = GetFirstNode(&Driver->Ep0Queue);
  if (Node == &Driver->Ep0Queue)
    Request = NULL;
  else
    Request = USB_REQUEST_FROM_LINK(Node);

  if (!Request) {
    DEBUG((EFI_D_ERROR, "Got txstate with empty queue, csr0 0x%04x\n", MmioRead16(Driver->Base + MUSB_CSR0)));
    ASSERT(0);
    return;
  }

  /* load the data */
  Source = (UINT8*)Request->Buffer + Request->Actual;
  Count = MIN(FifoSize, Request->Length - Request->Actual);
  UsbWriteFifo(Driver, 0, Count, Source);
  Request->Actual += Count;

  /* update the flags */
  if (Count < FifoSize || (Request->Length == Request->Actual && !Request->Zero)) {
    Driver->Ep0State = MUSB_EP0_STAGE_STATUSOUT;
    Csr |= MUSB_CSR0_P_DATAEND;
  } else Request = NULL;

  /* send it out, triggering a "txpktrdy cleared" irq */
  UsbSelectEndpoint(Driver, 0);
  MmioWrite16(Driver->Base + MUSB_CSR0, Csr);

  /* report completions as soon as the fifo's loaded; there's no
   * win in waiting till this last packet gets acked.  (other than
   * very precise fault reporting, needed by USB TMC; possible with
   * this hardware, but not usable from portable gadget drivers.)
   */
  if (Request) {
    Driver->AckPending = Csr;

    // Complete request
    UsbEp0CompleteRequest(Driver, Request, EFI_SUCCESS);
    if (!Driver->AckPending)
      return;

    Driver->AckPending = 0;
  }
}

STATIC VOID UsbEp0RxState(USB_DRIVER *Driver) {
  LIST_ENTRY *Node;
  USB_REQUEST *Request;
  UINT16 Csr;
  UINT8 *Buffer;
  UINT32 Length;
  UINT16 Count;
  EFI_STATUS Status = EFI_SUCCESS;

  Node = GetFirstNode(&Driver->Ep0Queue);
  if (Node == &Driver->Ep0Queue)
    Request = NULL;
  else
    Request = USB_REQUEST_FROM_LINK(Node);

  /* read packet and ack; or stall because of gadget driver bug:
   * should have provided the rx buffer before setup() returned.
   */
  if (Request) {
    Buffer = (UINT8*)Request->Buffer + Request->Actual;
    Length = Request->Length - Request->Actual;
    
    /* read the buffer */
    Count = MmioRead16(Driver->Base + MUSB_COUNT0);
    if (Count > Length) {
      Status = EFI_BUFFER_TOO_SMALL;
      Count = Length;
    }
    UsbReadFifo(Driver, 0, Count, Buffer);
    Request->Actual += Count;

    Csr = MUSB_CSR0_P_SVDRXPKTRDY;
    if (Count < 64 || Request->Actual == Request->Length) {
      Driver->Ep0State = MUSB_EP0_STAGE_STATUSIN;
      Csr |= MUSB_CSR0_P_DATAEND;
    } else Request = NULL;
  } else
    Csr = MUSB_CSR0_P_SVDRXPKTRDY | MUSB_CSR0_P_SENDSTALL;

  /* Completion handler may choose to stall, e.g. because the
   * message just received holds invalid data.
   */
  if (Request) {
    Driver->AckPending = Csr;
    UsbEp0CompleteRequest(Driver, Request, Status);
    if (!Driver->AckPending)
      return;
    Driver->AckPending = 0;
  }
  UsbSelectEndpoint(Driver, 0);
  MmioWrite16(Driver->Base + MUSB_CSR0, Csr);
}

STATIC VOID UsbReadSetup(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Setup) {
  LIST_ENTRY *Node;
  USB_REQUEST *Request;

  UsbReadFifo(Driver, 0, sizeof *Setup, (UINT8*)Setup);

  /*DEBUG((
    EFI_D_INFO,
    "SETUP req%02x.%02x v%04x i%04x l%d\n",
    Setup->RequestType,
    Setup->Request,
    Setup->Value,
    Setup->Index,
    Setup->Length
  ));*/

  UsbDumpSetupPacket(Setup);

  /* clean up any leftover transfers */
  for (Node = GetFirstNode(&Driver->Ep0Queue); Node != &Driver->Ep0Queue; Node = GetNextNode(&Driver->Ep0Queue, Node)) {
    Request = USB_REQUEST_FROM_LINK(Node);
    UsbEp0CompleteRequest(Driver, Request, EFI_SUCCESS);
  }

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
  INT8 Handled = -1;
  UINT8 Endpoint;
  //UINT16 Csr;

  /* the gadget driver handles everything except what we MUST handle */
  if ((Setup->RequestType & USB_TYPE_MASK) != USB_REQ_TYPE_STANDARD)
    return 0;

  switch (Setup->Request) {
  case USB_REQ_SET_ADDRESS:
    /* change it after the status stage */
    Driver->SetAddress = TRUE;
    Driver->Address = Setup->Value & 0x7f;
    Handled = 1;
    break;

  case USB_REQ_CLEAR_FEATURE:
    DEBUG((EFI_D_INFO, "USB_REQ_CLEAR_FEATURE\n"));
    switch (Recip) {
    case USB_RECIP_DEVICE:
    case USB_RECIP_INTERFACE:
      break;
    case USB_RECIP_ENDPOINT:
      Endpoint = Setup->Index & 0x0f;
      if (Endpoint == 0 || Endpoint >= gSunxiSocConfig.NumEndpoints || Setup->Value == USB_FEATURE_ENDPOINT_HALT)
        break;

      Handled = 1;

      UsbSelectEndpoint(Driver, Endpoint);
      if (Setup->Index & USB_ENDPOINT_DIR_IN) {
        //Csr = MmioRead16(Driver->Base + MUSB_TXCSR);
        //Csr |= 
        /*MmioAndThenOr16(
          Driver->Base + MUSB_TXCSR,
          ~(MUSB_TXCSR_P_SENDSTALL | MUSB_TXCSR_P_SENTSTALL | MUSB_TXCSR_TXPKTRDY),
          MUSB_TXCSR_CLRDATATOG | MUSB_TXCSR_P_WZC_BITS
        );*/
      }

      break;
    }
    break;
  case USB_REQ_SET_FEATURE:
    DEBUG((EFI_D_INFO, "USB_REQ_SET_FEATURE\n"));
    break;
  default:
    /* delegate SET_CONFIGURATION, etc */
    Handled = 0;
    break;
  }

  return Handled;
}

STATIC INT8 UsbServiceInRequest(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Setup) {
  INT8 Handled = 0;

  if ((Setup->RequestType & USB_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
    switch (Setup->Request) {
    case USB_REQ_GET_STATUS:
      DEBUG((EFI_D_INFO, "USB_REQ_GET_STATUS\n"));
      break;

    default:
      break;
    }
  }

  return Handled;
}

VOID UsbEp0HandleIrq(USB_DRIVER *Driver) {
  UINT16 Csr0;
  UINT16 Len;
  USB_DEVICE_REQUEST Setup;
  INT8 Handled;
  LIST_ENTRY *Node;

  UsbSelectEndpoint(Driver, 0);

  Csr0 = MmioRead16(Driver->Base + MUSB_CSR0);
  Len = MmioRead16(Driver->Base + MUSB_COUNT0);

  DEBUG((
    EFI_D_INFO,
    "csr %04x, count %u, addr %u\n",
    Csr0, Len, MmioRead16(Driver->Base + MUSB_FADDR)
  ));

  if (Csr0 & MUSB_CSR0_P_DATAEND) {
    /*
     * If DATAEND is set we should not call the callback,
     * hence the status stage is not complete.
     */
    return;
  }

  /* I sent a stall.. need to acknowledge it now.. */
  if (Csr0 & MUSB_CSR0_P_SENTSTALL) {
    MmioWrite16(Driver->Base + MUSB_CSR0, Csr0 & ~MUSB_CSR0_P_SENTSTALL);
    Driver->Ep0State = MUSB_EP0_STAGE_IDLE;
    Csr0 = MmioRead16(Driver->Base + MUSB_CSR0);
  }
  
  /* request ended "early" */
  if (Csr0 & MUSB_CSR0_P_SETUPEND) {
    MmioWrite16(Driver->Base + MUSB_CSR0, MUSB_CSR0_P_SVDSETUPEND);
    /* Transition into the early status phase */
    switch (Driver->Ep0State) {
    case MUSB_EP0_STAGE_TX:
      Driver->Ep0State = MUSB_EP0_STAGE_STATUSOUT;
      break;
    case MUSB_EP0_STAGE_RX:
      Driver->Ep0State = MUSB_EP0_STAGE_STATUSIN;
      break;
    default:
      DEBUG((EFI_D_ERROR, "SetupEnd came in a wrong ep0stage %d\n", Driver->Ep0State));
    }

    Csr0 = MmioRead16(Driver->Base + MUSB_CSR0);
  }

  /* docs from Mentor only describe tx, rx, and idle/setup states.
   * we need to handle nuances around status stages, and also the
   * case where status and setup stages come back-to-back ...
   */
  switch (Driver->Ep0State) {
  case MUSB_EP0_STAGE_TX:
    /* irq on clearing txpktrdy */
    if ((Csr0 & MUSB_CSR0_TXPKTRDY) == 0) {
      UsbEp0TxState(Driver);
    }
    break;
  
  case MUSB_EP0_STAGE_RX:
    /* irq on set rxpktrdy */
    if (Csr0 & MUSB_CSR0_RXPKTRDY) {
      UsbEp0RxState(Driver);
    }
    break;

  case MUSB_EP0_STAGE_STATUSIN:
    /* end of sequence #2 (OUT/RX state) or #3 (no data) */

    /* update address (if needed) only @ the end of the
     * status phase per usb spec, which also guarantees
     * we get 10 msec to receive this irq... until this
     * is done we won't see the next packet.
     */
    if (Driver->SetAddress) {
      Driver->SetAddress = FALSE;
      MmioWrite8(Driver->Base + MUSB_FADDR, Driver->Address);
    }

    /* fallthrough */
  case MUSB_EP0_STAGE_STATUSOUT:
    /* end of sequence #1: write to host (TX state) */
    Node = GetFirstNode(&Driver->Ep0Queue);
    if (Node != &Driver->Ep0Queue) {
      DEBUG((EFI_D_INFO, "%s completing request, node=%p\n",
        (Driver->Ep0State == MUSB_EP0_STAGE_STATUSOUT) ? L"MUSB_EP0_STAGE_STATUSOUT" : L"MUSB_EP0_STAGE_STATUSIN",
        GetFirstNode(&Driver->Ep0Queue)));
      UsbEp0CompleteRequest(Driver, USB_REQUEST_FROM_LINK(Node), EFI_SUCCESS);
    }

    /*
     * In case when several interrupts can get coalesced,
     * check to see if we've already received a SETUP packet...
     */
    if (Csr0 & MUSB_CSR0_RXPKTRDY)
      goto setup;

    Driver->Ep0State = MUSB_EP0_STAGE_IDLE;

    break;

  case MUSB_EP0_STAGE_IDLE:
    /*
     * This state is typically (but not always) indiscernible
     * from the status states since the corresponding interrupts
     * tend to happen within too little period of time (with only
     * a zero-length packet in between) and so get coalesced...
     */
    Driver->Ep0State = MUSB_EP0_STAGE_SETUP;
    /* FALLTHROUGH */

  case MUSB_EP0_STAGE_SETUP:
setup:
    if (Csr0 & MUSB_CSR0_RXPKTRDY) {
      if (Len != 8) {
        DEBUG((EFI_D_ERROR, "setup packet len %d != 8\n", Len));
        break;
      }

      UsbReadSetup(Driver, &Setup);

      switch (Driver->Ep0State) {
      /* sequence #3 (no data stage), includes requests
       * we can't forward (notably SET_ADDRESS and the
       * device/endpoint feature set/clear operations)
       * plus SET_CONFIGURATION and others we must
       */
      case MUSB_EP0_STAGE_ACKWAIT:
        Handled = UsbServiceZeroDataRequest(Driver, &Setup);

        /*
         * We're expecting no data in any case, so
         * always set the DATAEND bit -- doing this
         * here helps avoid SetupEnd interrupt coming
         * in the idle stage when we're stalling...
         */
        Driver->AckPending |= MUSB_CSR0_P_DATAEND;

        /* status stage might be immediate */
        if (Handled > 0)
          Driver->Ep0State = MUSB_EP0_STAGE_STATUSIN;
        break;
    
      /* sequence #1 (IN to host), includes GET_STATUS
       * requests that we can't forward, GET_DESCRIPTOR
       * and others that we must
       */
      case MUSB_EP0_STAGE_TX:
        Handled = UsbServiceInRequest(Driver, &Setup);
        if (Handled > 0) {
          Driver->AckPending = MUSB_CSR0_TXPKTRDY | MUSB_CSR0_P_DATAEND;
          Driver->Ep0State = MUSB_EP0_STAGE_STATUSOUT;
        }
        break;
    
      /* sequence #2 (OUT from host), always forward */
      default:
        break;
      }

      /* unless we need to delegate this to the gadget
       * driver, we know how to wrap this up:  csr0 has
       * not yet been written.
       */
      if (Handled < 0)
        goto stall;
      else if (Handled > 0)
        goto finish;

      Handled = UsbForwardControlRequestToGadgetDriver(Driver, &Setup);
      if (Handled <= 0) {
        UsbSelectEndpoint(Driver, 0);
stall:
        DEBUG((
          EFI_D_INFO,
          "Stall (%d) request 0x%x type 0x%x index 0x%x value 0x%x length 0x%x\n",
          Handled,
          Setup.Request,
          Setup.RequestType,
          Setup.Index,
          Setup.Value,
          Setup.Length
        ));
        Driver->AckPending |= MUSB_CSR0_P_SENDSTALL;
        Driver->Ep0State = MUSB_EP0_STAGE_IDLE;
finish:
        MmioWrite16(Driver->Base + MUSB_CSR0, Driver->AckPending);
        Driver->AckPending = 0;
      }
    }
    break;
  
  default:
    DEBUG((EFI_D_ERROR, "Invalid EP0 state %u\n", Driver->Ep0State));
    MmioWrite16(Driver->Base + MUSB_CSR0, MUSB_CSR0_P_SENDSTALL);
    Driver->Ep0State = MUSB_EP0_STAGE_IDLE;
    break;
  }
}

EFI_STATUS UsbEp0QueuePacket(USB_DRIVER *Driver, USB_REQUEST *Request) {
  switch (Driver->Ep0State) {
  case MUSB_EP0_STAGE_RX:       /* control-OUT data */
  case MUSB_EP0_STAGE_TX:       /* control-IN data */
  case MUSB_EP0_STAGE_ACKWAIT:  /* zero-length data */
    break;
  default:
    DEBUG((EFI_D_ERROR, "ep0 request queued in state %d\n", Driver->Ep0State));
    return EFI_DEVICE_ERROR;
  }

  /// TODO: ensure node is not inserted anywhere else

  Request->Actual = 0;

  InsertHeadList(&Driver->Ep0Queue, &Request->Node);

  UsbSelectEndpoint(Driver, 0);

  /* sequence #1, IN ... start writing the data */
  if (Driver->Ep0State == MUSB_EP0_STAGE_TX)
    UsbEp0TxState(Driver);
  /* sequence #3, no-data ... issue IN status */
  else if (Driver->Ep0State == MUSB_EP0_STAGE_ACKWAIT) {
    if (Request->Length > 0)
      return EFI_DEVICE_ERROR;
    else {
      DEBUG((EFI_D_INFO, "+++++++++++++++++++++++++++++++++++++++++ SENDING STATUS\n"));
      Driver->Ep0State = MUSB_EP0_STAGE_STATUSIN;
      MmioWrite16(Driver->Base + MUSB_CSR0, Driver->AckPending | MUSB_CSR0_P_DATAEND);
      Driver->AckPending = 0;
      UsbEp0CompleteRequest(Driver, Request, EFI_SUCCESS);
    }
  }
  /* else for sequence #2 (OUT), caller provides a buffer
   * before the next packet arrives.  deferred responses
   * (after SETUP is acked) are racey.
   */
  else if (Driver->AckPending) {
    MmioWrite16(Driver->Base + MUSB_CSR0, Driver->AckPending);
    Driver->AckPending = 0;
  }

  return EFI_SUCCESS;
}

VOID UsbEp0CompleteRequest(USB_DRIVER *Driver, USB_REQUEST *Request, EFI_STATUS Status) {
  RemoveEntryList(&Request->Node);

  // Signal completion
  UsbSignalCompletion(Driver, 0, Request, EFI_SUCCESS);
}

EFI_STATUS UsbEp0Halt(USB_DRIVER *Driver) {
  UINT16 Csr0;

  if (!IsListEmpty(&Driver->Ep0Queue)) {
    DEBUG((EFI_D_ERROR, "Attempting to halt EP0 with non-empty queue.\n"));
    return EFI_DEVICE_ERROR;
  }

  UsbSelectEndpoint(Driver, 0);
  Csr0 = Driver->AckPending;

  switch (Driver->Ep0State)
  {
    /* Stalls are usually issued after parsing SETUP packet, either
     * directly in irq context from setup() or else later.
     */
  case MUSB_EP0_STAGE_TX: /* control-IN data */
  case MUSB_EP0_STAGE_ACKWAIT: /* STALL for zero-length data */
  case MUSB_EP0_STAGE_RX: /* control-OUT data */
    Csr0 = MmioRead16(Driver->Base + MUSB_CSR0);
    /* FALLTHROUGH */

  /* It's also OK to issue stalls during callbacks when a non-empty
   * DATA stage buffer has been read (or even written).
   */
  case MUSB_EP0_STAGE_STATUSIN: /* control-OUT status */
  case MUSB_EP0_STAGE_STATUSOUT: /* control-IN status */
    Csr0 |= MUSB_CSR0_P_SENDSTALL;
    MmioWrite16(Driver->Base + MUSB_CSR0, Csr0);
    Driver->Ep0State = MUSB_EP0_STAGE_IDLE;
    Driver->AckPending = 0;
    break;
  
  default:
    DEBUG((EFI_D_ERROR, "EP0 can't halt in state %d\n", Driver->Ep0State));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
