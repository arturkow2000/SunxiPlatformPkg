#include "Driver.h"

#if FixedPcdGet32(UsbHighSpeedSupport)
#define RNDIS_LINK_SPEED 480000000
#else
#define RNDIS_LINK_SPEED 12000000
#endif

#define RNDIS_MTU 1500
#define ETHERNET_HEADER_SIZE 14
#define RNDIS_MAX_PACKET_SIZE (RNDIS_MTU + ETHERNET_HEADER_SIZE)

STATIC UINT8 RndisInterruptMsg[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

CONST UINT32 RndisSupportedOIDs[] = {
  OID_GEN_SUPPORTED_LIST,
  OID_GEN_HARDWARE_STATUS,
  OID_GEN_MEDIA_SUPPORTED,
  OID_GEN_MEDIA_IN_USE,
  OID_GEN_MAXIMUM_LOOKAHEAD,
  OID_GEN_MAXIMUM_FRAME_SIZE,
  OID_GEN_LINK_SPEED,
  OID_GEN_TRANSMIT_BUFFER_SPACE,
  OID_GEN_RECEIVE_BUFFER_SPACE,
  OID_GEN_TRANSMIT_BLOCK_SIZE,
  OID_GEN_RECEIVE_BLOCK_SIZE,
  OID_GEN_VENDOR_ID,
  OID_GEN_VENDOR_DESCRIPTION,
  OID_GEN_CURRENT_PACKET_FILTER,
  OID_GEN_CURRENT_LOOKAHEAD,
  OID_GEN_DRIVER_VERSION,
  OID_GEN_MAXIMUM_TOTAL_SIZE,
  OID_GEN_PROTOCOL_OPTIONS,
  OID_GEN_MAC_OPTIONS,
  OID_GEN_MEDIA_CONNECT_STATUS,
  OID_GEN_MAXIMUM_SEND_PACKETS,
  OID_GEN_VENDOR_DRIVER_VERSION,
  OID_802_3_PERMANENT_ADDRESS,
  OID_802_3_CURRENT_ADDRESS,
  OID_802_3_MULTICAST_LIST,
  OID_802_3_MAXIMUM_LIST_SIZE,
  OID_802_3_MAC_OPTIONS,
};

EFI_STATUS RndisInit(USB_GADGET *This) {
  EFI_STATUS Status;
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  RNDIS_DRIVER *Rndis = &Driver->Rndis;

  Rndis->DataInUrb = UsbGadgetAllocateUrb();
  if (!Rndis->DataInUrb)
    return EFI_OUT_OF_RESOURCES;

  Rndis->DataInUrb2 = UsbGadgetAllocateUrb();
  if (!Rndis->DataInUrb2)
    return EFI_OUT_OF_RESOURCES;

  Rndis->DataOutUrb = UsbGadgetAllocateUrb();
  if (!Rndis->DataOutUrb)
    return EFI_OUT_OF_RESOURCES;

  Rndis->InterruptUrb = UsbGadgetAllocateUrb();
  if (!Rndis->InterruptUrb)
    return EFI_OUT_OF_RESOURCES;

  Rndis->RxBufferTemp = AllocatePool(CDC_DATA_MAX_PACKET);
  if (!Rndis->RxBufferTemp)
    return EFI_OUT_OF_RESOURCES;

  Rndis->EncapsulatedMessage = AllocatePool(RNDIS_ENC_MAX);
  if (!Rndis->EncapsulatedMessage)
    return EFI_OUT_OF_RESOURCES;

  Status = SimpleBufferInit(&Rndis->TxBuffer, CDC_DATA_MAX_PACKET * 8);
  if (EFI_ERROR(Status))
    return Status;

  Status = SimpleBufferInit(&Rndis->RxBuffer, CDC_DATA_MAX_PACKET * 8);
  if (EFI_ERROR(Status))
    return Status;

  Rndis->EncTxBufferSize = 0;

  return EFI_SUCCESS;
}

STATIC VOID RndisProcessRx(GADGET_DRIVER *Gadget) {
  RNDIS_DRIVER *Rndis = &Gadget->Rndis;
  // Data in ring buffer may not be aligned nor continuous.
  RNDIS_DATA_HEADER Header;
  UINTN n = SimpleBufferPeekCopy(&Rndis->RxBuffer, (UINT8*)&Header, sizeof Header);
  // This function should not be called if we don't have enough data
  ASSERT(n == sizeof Header);

#if 0
  DEBUG((EFI_D_INFO, "=====================================\n"));
  DEBUG((EFI_D_INFO, "MessageType = %d\n", Header.MessageType));
  DEBUG((EFI_D_INFO, "MessageLength = %d\n", Header.MessageLength));
  DEBUG((EFI_D_INFO, "DataOffset = %d\n", Header.DataOffset));
  DEBUG((EFI_D_INFO, "DataLength = %d\n", Header.DataLength));
  DEBUG((EFI_D_INFO, "OOBDataOffset = %d\n", Header.OOBDataOffset));
  DEBUG((EFI_D_INFO, "OOBDataLength = %d\n", Header.OOBDataLength));
  DEBUG((EFI_D_INFO, "NumOOBDataElements = %d\n", Header.NumOOBDataElements));
  DEBUG((EFI_D_INFO, "PerPacketInfoOffset = %d\n", Header.PerPacketInfoOffset));
  DEBUG((EFI_D_INFO, "PerPacketInfoLength = %d\n", Header.PerPacketInfoLength));
  DEBUG((EFI_D_INFO, "DeviceVcHandle = %d\n", Header.DeviceVcHandle));
  DEBUG((EFI_D_INFO, "Reserved = %d\n", Header.Reserved));
  DEBUG((EFI_D_INFO, "=====================================\n"));
#endif

  ASSERT(Header.MessageType == 1);
  if (SimpleBufferLength(&Rndis->RxBuffer) >= Header.MessageLength) {
    DEBUG((EFI_D_ERROR, "Processing message\n"));
    UINT32 DataOffset = 8 + Header.DataOffset;
    // FIXME: use DataLength?
    UINT32 l = Header.MessageLength - DataOffset;
    // Copy payload and discard everything else
    SimpleBufferDiscard(&Rndis->RxBuffer, DataOffset);
    NetProcessEthernetFrame(&Gadget->Snp, &Rndis->RxBuffer, Header.DataLength);
    SimpleBufferDiscard(&Rndis->RxBuffer, l);

    Rndis->RxProcessThr = sizeof(RNDIS_DATA_HEADER);
  } else {
    Rndis->RxProcessThr = Header.MessageLength;
  }
}

STATIC VOID RndisHandleData(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  RNDIS_DRIVER *Rndis = &Driver->Rndis;

  if (Urb->Status == EFI_ABORTED)
    return;

  if (Urb->Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "RNDIS failed to receive data, status: %r\n", Urb->Status));
    return;
  }

  //DEBUG((EFI_D_INFO, "Incoming RNDIS data: len=%d status=%r\n", Urb->Actual, Urb->Status));
  UINTN Length = Urb->Actual;
  SimpleBufferWrite(&Rndis->RxBuffer, Urb->Buffer, &Length);
  if (Length != Urb->Actual) {
    DEBUG((EFI_D_ERROR, "RX underrun\n"));
    UINTN Offset = Length;
    Length = Urb->Actual - Length;
    SimpleBufferDiscard(&Rndis->RxBuffer, Length);
    SimpleBufferWrite(&Rndis->RxBuffer, Urb->Buffer + Offset, &Length);
  }

  while (SimpleBufferLength(&Rndis->RxBuffer) >= Rndis->RxProcessThr)
    RndisProcessRx(Driver);
}

EFI_STATUS RndisEnable(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  RNDIS_DRIVER *Rndis = &Driver->Rndis;
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  Status = UsbGadgetEnableEndpoint(This, &Config->RndisControlEp);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable RNDIS control endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->RndisDataEpOut);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable RNDIS data out endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->RndisDataEpIn);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable RNDIS data in endpoint\n"));
    return Status;
  }

  // Start processing incoming data after receiving header
  Rndis->RxProcessThr = sizeof(RNDIS_DATA_HEADER);
  Status = UsbGadgetQueue(
    This,
    Config->RndisDataEpOut.EndpointAddress,
    Rndis->DataOutUrb,
    Rndis->RxBufferTemp,
    CDC_DATA_MAX_PACKET,
    0,
    RndisHandleData
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to initialize RNDIS data transfer\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS RndisDisable(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  RNDIS_DRIVER *Rndis = &Driver->Rndis;
  EFI_STATUS Status;

  Status = UsbGadgetDisableEndpoint(This, &Config->RndisControlEp);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable RNDIS control endpoint\n"));
  }

  Status = UsbGadgetDisableEndpoint(This, &Config->RndisDataEpIn);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable RNDIS data in endpoint\n"));
  }

  Status = UsbGadgetDisableEndpoint(This, &Config->RndisDataEpOut);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable RNDIS data out endpoint\n"));
  }

  SimpleBufferClear(&Rndis->TxBuffer);

  return EFI_SUCCESS;
}

STATIC UINT32 RndisQuery(RNDIS_QUERY_MSG *Msg) {
  CONST VOID *Data = NULL;
  UINT32 Size = 0;
  UINT32 Oid = Msg->Oid;
  UINT32 RequestId = Msg->RequestId;
  RNDIS_QUERY_CMPLT *Cmplt = (RNDIS_QUERY_CMPLT*)Msg;
  UINT32 U32Temp;

#define H_ARR(oid, array)   \
case oid:                   \
  Data = &(array)[0];       \
  Size = sizeof (array);    \
  break

#define H_U32(oid, value)   \
case oid:                   \
  U32Temp = (value);        \
  Data = &U32Temp;          \
  Size = sizeof U32Temp;    \
  break;

  DEBUG((EFI_D_INFO, "RNDIS query OID 0x%08x\n", Oid));
  switch (Oid) {
  H_ARR(OID_GEN_SUPPORTED_LIST, RndisSupportedOIDs);
  H_U32(OID_GEN_HARDWARE_STATUS, NdisHardwareStatusReady);
  H_U32(OID_GEN_MEDIA_SUPPORTED, RNDIS_MEDIUM_802_3);
  H_U32(OID_GEN_MEDIA_IN_USE, RNDIS_MEDIUM_802_3);
  // TODO: OID_GEN_MAXIMUM_LOOKAHEAD
  H_U32(OID_GEN_MAXIMUM_FRAME_SIZE, RNDIS_MTU);
  H_U32(OID_GEN_LINK_SPEED, RNDIS_LINK_SPEED);
  // TODO: OID_GEN_TRANSMIT_BUFFER_SPACE
  // TODO: OID_GEN_RECEIVE_BUFFER_SPACE
  // TODO: OID_GEN_TRANSMIT_BLOCK_SIZE
  // TODO: OID_GEN_RECEIVE_BLOCK_SIZE
  // TODO: OID_GEN_VENDOR_ID
  // TODO: OID_GEN_VENDOR_DESCRIPTION
  // TODO: OID_GEN_CURRENT_PACKET_FILTER
  // TODO: OID_GEN_CURRENT_LOOKAHEAD
  // TODO: OID_GEN_DRIVER_VERSION
  H_U32(OID_GEN_MAXIMUM_TOTAL_SIZE, RNDIS_MAX_PACKET_SIZE);
  // TODO: OID_GEN_PROTOCOL_OPTIONS
  // TODO: OID_GEN_MAC_OPTIONS
  H_U32(OID_GEN_MEDIA_CONNECT_STATUS, NdisMediaStateConnected);
  case OID_802_3_PERMANENT_ADDRESS:
  case OID_802_3_CURRENT_ADDRESS:
    Data = NetGetMacAddress();
    Size = 6;
    ASSERT(Data);
    break;
  H_U32(OID_802_3_MAXIMUM_LIST_SIZE, 1);
  default:
    DEBUG((EFI_D_ERROR, "RNDIS unsupported OID 0x%08x\n", Oid));
    ASSERT(0);
    break;
  }

  Cmplt->Header.Type = REMOTE_NDIS_QUERY_CMPLT;
  Cmplt->Header.Length = sizeof(RNDIS_QUERY_CMPLT) + Size;
  Cmplt->RequestId = RequestId;
  if (Data) {
    ASSERT(Size > 0);
    Cmplt->Status = RNDIS_STATUS_SUCCESS;
    Cmplt->InformationBufferOffset = sizeof(RNDIS_QUERY_CMPLT) - sizeof(RNDIS_MESSAGE_HEADER);
    Cmplt->InformationBufferLength = Size;
    CopyMem(&Cmplt[1], Data, Size);
  } else {
    ASSERT(Size == 0);
    Cmplt->Status = RNDIS_STATUS_FAILURE;
    Cmplt->InformationBufferOffset = 0;
    Cmplt->InformationBufferLength = 0;
  }

  return Cmplt->Header.Length;
}

STATIC UINT32 RndisSet(RNDIS_SET_MSG *Msg) {
  UINT32 Oid = Msg->Oid;
  UINT32 RequestId = Msg->RequestId;
  RNDIS_SET_CMPLT *Cmplt = (RNDIS_SET_CMPLT*)Msg;

  DEBUG((EFI_D_INFO, "RNDIS set OID 0x%08x data len %d\n", Oid, Msg->InformationBufferLength));

  Cmplt->Header.Type = REMOTE_NDIS_SET_CMPLT;
  Cmplt->Header.Length = sizeof(RNDIS_SET_CMPLT);
  Cmplt->RequestId = RequestId;
  Cmplt->Status = RNDIS_STATUS_SUCCESS;

  return Cmplt->Header.Length;
}

STATIC UINT32 RndisReset(RNDIS_RESET_MSG *Msg) {
  RNDIS_RESET_CMPLT *Cmplt = (RNDIS_RESET_CMPLT*)Msg;
  Cmplt->Header.Type = REMOTE_NDIS_RESET_CMPLT;
  Cmplt->Header.Length = sizeof(RNDIS_RESET_CMPLT);
  Cmplt->Status = RNDIS_STATUS_SUCCESS;
  Cmplt->AddressingReset = 1;

  return sizeof(RNDIS_RESET_CMPLT);
}

STATIC VOID RndisHandleCommand(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  RNDIS_DRIVER *Rndis = &Driver->Rndis;
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  if (Urb->Status == EFI_ABORTED)
    return;

  if (Urb->Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "RNDIS control transfer failed\n"));
    return;
  }

  RNDIS_MESSAGE_HEADER *Header = (RNDIS_MESSAGE_HEADER*)Rndis->EncapsulatedMessage;
  if (Header->Length > Urb->Actual) {
    DEBUG((EFI_D_ERROR, "RNDIS message bigger than control transfer size\n"));
    return;
  }
  switch (Header->Type) {
  case REMOTE_NDIS_INITIALIZE_MSG:
    if (Header->Length < sizeof(RNDIS_INITIALIZE_MSG))
      goto error;

    RNDIS_INITIALIZE_MSG *Init = (RNDIS_INITIALIZE_MSG*)Header;
    DEBUG((EFI_D_INFO, "RNDIS init version %d.%d, max transfer=%d\n", Init->Major, Init->Minor, Init->MaxTransferSize));

    RNDIS_INITIALIZE_CMPLT *InitCmplt = (RNDIS_INITIALIZE_CMPLT*)Header;
    InitCmplt->Header.Type = REMOTE_NDIS_INITIALIZE_CMPLT;
    InitCmplt->Header.Length = sizeof(RNDIS_INITIALIZE_CMPLT);
    InitCmplt->RequestId = Init->RequestId;
    InitCmplt->Status = RNDIS_STATUS_SUCCESS;
    InitCmplt->Major = 1;
    InitCmplt->Minor = 0;
    InitCmplt->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
    InitCmplt->Medium = RNDIS_MEDIUM_802_3;
    InitCmplt->MaxPacketsPerMessage = 1;
    InitCmplt->PacketAlignmentFactor = 0;
    InitCmplt->AFListOffset = 0;
    InitCmplt->AFListSize = 0;
    Rndis->EncTxBufferSize = sizeof(RNDIS_INITIALIZE_CMPLT);
    break;
  case REMOTE_NDIS_QUERY_MSG:
    if (Header->Length < sizeof(RNDIS_QUERY_MSG))
      goto error;

    Rndis->EncTxBufferSize = RndisQuery((RNDIS_QUERY_MSG*)Header);
    break;
  case REMOTE_NDIS_SET_MSG:
    if (Header->Length < sizeof(RNDIS_SET_MSG))
      goto error;

    Rndis->EncTxBufferSize = RndisSet((RNDIS_SET_MSG*)Header);
    break;
  case REMOTE_NDIS_RESET_MSG:
    if (Header->Length < sizeof(RNDIS_RESET_MSG))
      goto error;

    Rndis->EncTxBufferSize = RndisReset((RNDIS_RESET_MSG*)Header);
    break;
  case REMOTE_NDIS_KEEPALIVE_MSG:
    if (Header->Length < sizeof(RNDIS_KEEPALIVE_MSG))
      goto error;

    RNDIS_KEEPALIVE_CMPLT *KeepaliveCmplt = (RNDIS_KEEPALIVE_CMPLT*)Header;
    KeepaliveCmplt->Header.Type = REMOTE_NDIS_KEEPALIVE_CMPLT;
    KeepaliveCmplt->Header.Length = sizeof(RNDIS_KEEPALIVE_CMPLT);
    KeepaliveCmplt->Status = RNDIS_STATUS_SUCCESS;

    Rndis->EncTxBufferSize = sizeof(RNDIS_KEEPALIVE_CMPLT);

    Status = UsbGadgetQueue(
      This,
      Config->RndisDataEpIn.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
      Rndis->DataInUrb,
      Rndis->RxBufferTemp,
      CDC_DATA_MAX_PACKET,
      USB_FLAG_TX | USB_FLAG_ZLP,
      NULL
    );
    ASSERT_EFI_ERROR(Status);
    break;
  // TODO: handle this properly
  case REMOTE_NDIS_HALT_MSG:
    goto error_silent;
  default:
error:
    DEBUG((EFI_D_ERROR, "Unhandled RNDIS message Type=0x%08x Length=%d\n", Header->Type, Header->Length));
    ASSERT(0);
error_silent:
    // RNDIS expects 1 byte message filled with zeros instead of stall.
    Rndis->EncTxBufferSize = 1;
    Rndis->EncapsulatedMessage[0] = 0;
    break;
  }

  // Host won't poll control endpoint until we send interrupt.
  Status = UsbGadgetQueue(
    This,
    Config->RndisControlEp.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
    Rndis->InterruptUrb,
    RndisInterruptMsg,
    sizeof RndisInterruptMsg,
    USB_FLAG_TX,
    NULL
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to send RNDIS interrupt (%r), expect RNDIS to break\n", Status));
  }
}

EFI_STATUS RndisHandleRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  RNDIS_DRIVER *Rndis = &Driver->Rndis;
  UINT32 TotalLength;

  if (Request->RequestType & USB_ENDPOINT_DIR_IN) {
    if (Rndis->EncTxBufferSize == 0) {
      DEBUG((EFI_D_ERROR, "RNDIS host issued IN transfer but we have nothing to say, stalling\n"));
      return EFI_DEVICE_ERROR;
    }

    TotalLength = MIN(Request->Length, Rndis->EncTxBufferSize);
    return UsbGadgetInitUrb(
      This,
      This->ControlUrb,
      Rndis->EncapsulatedMessage,
      TotalLength,
      TotalLength == Request->Length ? USB_FLAG_ZLP : 0 | USB_FLAG_TX,
      NULL
    );
  } else {
    if (Request->Length > RNDIS_ENC_MAX) {
      DEBUG((EFI_D_ERROR, "RNDIS request too big (%d, max is %d)\n", Request->Length, RNDIS_ENC_MAX));
      return EFI_DEVICE_ERROR;
    }

    if (Request->Length < sizeof(RNDIS_MESSAGE_HEADER)) {
      DEBUG((EFI_D_ERROR, "RNDIS message smaller than header\n"));
      return EFI_DEVICE_ERROR;
    }

    return UsbGadgetInitUrb(
      This,
      This->ControlUrb,
      Rndis->EncapsulatedMessage,
      Request->Length,
      0,
      RndisHandleCommand
    );
  }
}

STATIC BOOLEAN RndisDiscardPacket(RNDIS_DRIVER *Rndis) {
  RNDIS_DATA_HEADER Header;
  UINTN n;

  n = SimpleBufferPeekCopy(&Rndis->TxBuffer, (UINT8*)&Header, sizeof Header);
  if (n == 0)
    return FALSE;

  ASSERT(n == sizeof(Header));
  ASSERT(Header.MessageType == 1);
  ASSERT(SimpleBufferLength(&Rndis->TxBuffer) >= Header.MessageLength);
  SimpleBufferDiscard(&Rndis->TxBuffer, Header.MessageLength);

  return TRUE;
}

STATIC VOID RndisTxComplete(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  RNDIS_DRIVER *Rndis = &Driver->Rndis;

  Rndis->TxPending = FALSE;

  if (Urb->Status == EFI_ABORTED)
    return;

    ASSERT(0);

  if (Urb->Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "RNDIS TX failed: %r\n", Urb->Status));
    return;
  }

  ASSERT(SimpleBufferLength(&Rndis->TxBuffer) >= Urb->Actual);
  SimpleBufferDiscard(&Rndis->TxBuffer, Urb->Actual);

  if (!SimpleBufferIsEmpty(&Rndis->TxBuffer)) {
    // Queue next packet
    UINT8 *Part1;
    UINTN Part1Length;
    SimpleBufferPeek(&Rndis->TxBuffer, &Part1, &Part1Length, NULL, NULL);

    UINTN l = MIN(Part1Length, CDC_DATA_MAX_PACKET);
    UINT32 Flags = USB_FLAG_TX;
    if (l % CDC_DATA_MAX_PACKET == 0)
      Flags |= USB_FLAG_ZLP;

    EFI_STATUS Status = UsbGadgetQueue(
      This,
      Config->RndisDataEpIn.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
      Rndis->DataInUrb2,
      Part1,
      l,
      Flags,
      RndisTxComplete
    );
    ASSERT_EFI_ERROR(Status);
    Rndis->TxPending = TRUE;

    // Host won't poll data endpoint until we send interrupt.
    Status = UsbGadgetQueue(
      This,
      Config->RndisControlEp.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
      Rndis->InterruptUrb,
      RndisInterruptMsg,
      sizeof RndisInterruptMsg,
      USB_FLAG_TX,
      NULL
    );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to send RNDIS interrupt (%r), expect RNDIS to break\n", Status));
    }
  }
}

EFI_STATUS RndisSendPacket(USB_GADGET *This, VOID *Payload, UINTN PayloadLength)
{
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  RNDIS_DRIVER *Rndis = &Driver->Rndis;

  RNDIS_DATA_HEADER Header = {0};
  Header.MessageType = 1;
  Header.MessageLength = sizeof(Header) + PayloadLength;
  Header.DataOffset = sizeof(Header) - 8;
  Header.DataLength = PayloadLength;

  UINTN NeededFree = Header.MessageLength;
  if (NeededFree == Rndis->TxBuffer.Capacity) {
    DEBUG((EFI_D_INFO, "RNDIS dropping entire TX buffer\n"));
    SimpleBufferClear(&Rndis->TxBuffer);
  } else {
    // Discard old packets if needed
    UINTN Dropped = 0;
    while (TRUE) {
      UINTN Free = Rndis->TxBuffer.Capacity - SimpleBufferLength(&Rndis->TxBuffer);
      if (Free >= NeededFree)
        break;

      BOOLEAN discarded = RndisDiscardPacket(Rndis);
      ASSERT(discarded);
      Dropped += 1;
    }

    if (Dropped > 0)
      DEBUG((EFI_D_INFO, "RNDIS dropped %d outgoing packets\n", Dropped));
  }

  UINTN l = sizeof(Header);
  SimpleBufferWrite(&Rndis->TxBuffer, (UINT8*)&Header, &l);
  ASSERT(l == sizeof(Header));

  l = PayloadLength;
  SimpleBufferWrite(&Rndis->TxBuffer, (UINT8*)&Header, &l);
  ASSERT(l == PayloadLength);

  // TX is in progress, another transfer will be automatically queued after the
  // current packet is sent.
  if (Rndis->TxPending)
    return EFI_SUCCESS;

  UINT8 *Part1;
  UINTN Part1Length;
  // Send as many data as possible, driver currently can accept only one pointer.
  SimpleBufferPeek(&Rndis->TxBuffer, &Part1, &Part1Length, NULL, NULL);

  l = MIN(Part1Length, CDC_DATA_MAX_PACKET);
  UINT32 Flags = USB_FLAG_TX;
  if (l % CDC_DATA_MAX_PACKET == 0)
    Flags |= USB_FLAG_ZLP;

  EFI_STATUS Status = UsbGadgetQueue(
    This,
    Config->RndisDataEpIn.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
    Rndis->DataInUrb2,
    Part1,
    l,
    Flags,
    RndisTxComplete
  );
  ASSERT_EFI_ERROR(Status);
  if (!EFI_ERROR(Status))
    Rndis->TxPending = TRUE;

  // Host won't poll data endpoint until we send interrupt.
  Status = UsbGadgetQueue(
    This,
    Config->RndisControlEp.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
    Rndis->InterruptUrb,
    RndisInterruptMsg,
    sizeof RndisInterruptMsg,
    USB_FLAG_TX,
    NULL
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to send RNDIS interrupt (%r), expect RNDIS to break\n", Status));
  }

  return Status;
}
