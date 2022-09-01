#include "../Driver.h"

#define EMULATED_NETWORK_GUID       \
  { 0x80702fa4, 0x379f, 0x4251, { 0xb5, 0xb9, 0x9e, 0xf3, 0x57, 0xf6, 0xb5, 0x5e } }

STATIC EFI_STATUS EFIAPI NetworkStart(
  IN EFI_SIMPLE_NETWORK_PROTOCOL  *This
) {
  if (This == NULL)
    return EFI_INVALID_PARAMETER;

  SNP_DRIVER *Snp = SNP_TO_DRIVER(This);

  switch (Snp->Mode.State) {
  case EfiSimpleNetworkStopped:
    Snp->Mode.State = EfiSimpleNetworkStarted;
    return EFI_SUCCESS;
  case EfiSimpleNetworkStarted:
  case EfiSimpleNetworkInitialized:
    return EFI_ALREADY_STARTED;
  default:
    return EFI_DEVICE_ERROR;
  }
}

STATIC EFI_STATUS EFIAPI NetworkStop(
  IN EFI_SIMPLE_NETWORK_PROTOCOL  *This
) {
  if (This == NULL)
    return EFI_INVALID_PARAMETER;

  SNP_DRIVER *Snp = SNP_TO_DRIVER(This);

  switch (Snp->Mode.State) {
  case EfiSimpleNetworkStarted:
    // TODO: stop all pending transfers
    Snp->Mode.State = EfiSimpleNetworkStopped;
    return EFI_SUCCESS;
  case EfiSimpleNetworkStopped:
    return EFI_NOT_STARTED;
  default:
    return EFI_DEVICE_ERROR;
  }
}

STATIC EFI_STATUS EFIAPI NetworkInitialize(
  IN EFI_SIMPLE_NETWORK_PROTOCOL                    *This,
  IN UINTN                                          ExtraRxBufferSize  OPTIONAL,
  IN UINTN                                          ExtraTxBufferSize  OPTIONAL
) {
  if (This == NULL)
    return EFI_INVALID_PARAMETER;

  SNP_DRIVER *Snp = SNP_TO_DRIVER(This);
  switch (Snp->Mode.State) {
  case EfiSimpleNetworkStarted:
    break;
  case EfiSimpleNetworkStopped:
    return EFI_NOT_STARTED;
  default:
    return EFI_DEVICE_ERROR;
  }

  Snp->Mode.State = EfiSimpleNetworkInitialized;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS EFIAPI NetworkReset(
  IN EFI_SIMPLE_NETWORK_PROTOCOL   *This,
  IN BOOLEAN                       ExtendedVerification
) {
  ASSERT(0);
  return EFI_DEVICE_ERROR;
}

STATIC EFI_STATUS EFIAPI NetworkShutdown(
  IN EFI_SIMPLE_NETWORK_PROTOCOL  *This
) {
  if (This == NULL)
    return EFI_INVALID_PARAMETER;

  SNP_DRIVER *Snp = SNP_TO_DRIVER(This);

  switch (Snp->Mode.State) {
  case EfiSimpleNetworkInitialized:
    break;

  case EfiSimpleNetworkStopped:
    return EFI_NOT_STARTED;

  default:
    return EFI_DEVICE_ERROR;
  }

  Snp->Mode.State = EfiSimpleNetworkStarted;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS EFIAPI NetworkReceiveFilters(
  IN EFI_SIMPLE_NETWORK_PROTOCOL                             *This,
  IN UINT32                                                  Enable,
  IN UINT32                                                  Disable,
  IN BOOLEAN                                                 ResetMCastFilter,
  IN UINTN                                                   MCastFilterCnt OPTIONAL,
  IN EFI_MAC_ADDRESS                                         *MCastFilter OPTIONAL
) {
  DEBUG((EFI_D_INFO, "SNP filter: enable=0x%08x, disable=0x%08x, multicast filter count=%d, reset=%d\n", Enable, Disable, MCastFilter, ResetMCastFilter));
  if (MCastFilterCnt > 0) {
    DEBUG((EFI_D_INFO, "Multicast filters:\n"));
    for (UINT32 i = 0; i < MCastFilterCnt; i++) {
      EFI_MAC_ADDRESS *Mac = &MCastFilter[i];
      DEBUG((EFI_D_INFO, "  %02X:%02X:%02X:%02X:%02X:%02X",
        Mac->Addr[0], Mac->Addr[1], Mac->Addr[2],
        Mac->Addr[3], Mac->Addr[4], Mac->Addr[5]
      ));
    }
  }
  return EFI_SUCCESS;
}

STATIC EFI_STATUS EFIAPI NetworkStationAddress(
  IN EFI_SIMPLE_NETWORK_PROTOCOL            *This,
  IN BOOLEAN                                Reset,
  IN EFI_MAC_ADDRESS                        *New OPTIONAL
) {
  ASSERT(0);
  return EFI_DEVICE_ERROR;
}

STATIC EFI_STATUS EFIAPI NetworkStatistics(
  IN EFI_SIMPLE_NETWORK_PROTOCOL          *This,
  IN BOOLEAN                              Reset,
  IN OUT UINTN                            *StatisticsSize   OPTIONAL,
  OUT EFI_NETWORK_STATISTICS              *StatisticsTable  OPTIONAL
) {
  ASSERT(0);
  return EFI_DEVICE_ERROR;
}

STATIC EFI_STATUS EFIAPI NetworkMCastIpToMac(
  IN EFI_SIMPLE_NETWORK_PROTOCOL          *This,
  IN BOOLEAN                              IPv6,
  IN EFI_IP_ADDRESS                       *IP,
  OUT EFI_MAC_ADDRESS                     *MAC
) {
  ASSERT(0);
  return EFI_DEVICE_ERROR;
}

STATIC EFI_STATUS EFIAPI NetworkNvData(
  IN EFI_SIMPLE_NETWORK_PROTOCOL          *This,
  IN BOOLEAN                              ReadWrite,
  IN UINTN                                Offset,
  IN UINTN                                BufferSize,
  IN OUT VOID                             *Buffer
) {
  ASSERT(0);
  return EFI_DEVICE_ERROR;
}

STATIC EFI_STATUS EFIAPI NetworkGetStatus(
  IN EFI_SIMPLE_NETWORK_PROTOCOL          *This,
  OUT UINT32                              *InterruptStatus OPTIONAL,
  OUT VOID                                **TxBuf OPTIONAL
) {
  if (This == NULL || (InterruptStatus == NULL && TxBuf == NULL))
    return EFI_INVALID_PARAMETER;

  SNP_DRIVER *Snp = SNP_TO_DRIVER(This);
  switch (Snp->Mode.State) {
  case EfiSimpleNetworkInitialized:
    break;
  case EfiSimpleNetworkStopped:
    return EFI_NOT_STARTED;
  default:
    return EFI_DEVICE_ERROR;
  }

  if (InterruptStatus) {
    UINT32 Status = 0;
    if (Snp->Interrupt.Rx) {
      Snp->Interrupt.Rx = FALSE;
      Status |= EFI_SIMPLE_NETWORK_RECEIVE_INTERRUPT;
    }

    *InterruptStatus = Status;
  }

  if (TxBuf)
    *TxBuf = NULL;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS EFIAPI NetworkTransmit(
  IN EFI_SIMPLE_NETWORK_PROTOCOL          *This,
  IN UINTN                                HeaderSize,
  IN UINTN                                BufferSize,
  IN VOID                                 *Buffer,
  IN EFI_MAC_ADDRESS                      *SrcAddr  OPTIONAL,
  IN EFI_MAC_ADDRESS                      *DestAddr OPTIONAL,
  IN UINT16                               *Protocol OPTIONAL
) {
  DEBUG((EFI_D_INFO, "Transmit HeaderSize=%d BufferSize=%d Src=%p Dst=%p Proto=%p\n", HeaderSize, BufferSize, SrcAddr, DestAddr, Protocol));
  if (SrcAddr)
    DEBUG((EFI_D_INFO, "  Src=%02X:%02X:%02X:%02X:%02X:%02X\n",
      SrcAddr->Addr[0], SrcAddr->Addr[1], SrcAddr->Addr[2],
      SrcAddr->Addr[3], SrcAddr->Addr[4], SrcAddr->Addr[5]
    ));

  if (DestAddr)
    DEBUG((EFI_D_INFO, "  Dest=%02X:%02X:%02X:%02X:%02X:%02X\n",
      DestAddr->Addr[0], DestAddr->Addr[1], DestAddr->Addr[2],
      DestAddr->Addr[3], DestAddr->Addr[4], DestAddr->Addr[5]
    ));

  if (Protocol)
    DEBUG((EFI_D_INFO, "  Protocol=0x%04x\n", *Protocol));
  

  if (!This || !Buffer)
    return EFI_INVALID_PARAMETER;

  SNP_DRIVER *Snp = SNP_TO_DRIVER(This);
  if (HeaderSize != 0 && HeaderSize != Snp->Mode.MediaHeaderSize)
    return EFI_INVALID_PARAMETER;

  if (HeaderSize != 0 && (!SrcAddr || !DestAddr || !Protocol))
    return EFI_INVALID_PARAMETER;

  if (HeaderSize != 0) {
    // TODO: implement
    DEBUG((EFI_D_ERROR, "HeaderSize = %d not supported\n", HeaderSize));
    ASSERT(0);
    return EFI_DEVICE_ERROR;
  }

  //GADGET_DRIVER *Driver = SNP_TO_GADGET_DRIVER(Snp);
  //return RndisSendPacket(&Driver->Gadget, Buffer,  BufferSize);
  return EFI_SUCCESS;
}

STATIC EFI_STATUS EFIAPI NetworkReceive(
  IN EFI_SIMPLE_NETWORK_PROTOCOL          *This,
  OUT UINTN                               *HeaderSize OPTIONAL,
  IN OUT UINTN                            *BufferSize,
  OUT VOID                                *Buffer,
  OUT EFI_MAC_ADDRESS                     *SrcAddr    OPTIONAL,
  OUT EFI_MAC_ADDRESS                     *DestAddr   OPTIONAL,
  OUT UINT16                              *Protocol   OPTIONAL
) {
  EFI_TPL OrigTpl;

  if (This == NULL)
    return EFI_INVALID_PARAMETER;

  if (!BufferSize || !Buffer)
    return EFI_INVALID_PARAMETER;

  SNP_DRIVER *Snp = SNP_TO_DRIVER(This);

  switch (Snp->Mode.State) {
  case EfiSimpleNetworkInitialized:
    break;
  case EfiSimpleNetworkStopped:
    return EFI_NOT_STARTED;
  default:
    return EFI_DEVICE_ERROR;
  }

  EFI_STATUS Status = EFI_NOT_READY;

  OrigTpl = gBS->RaiseTPL(TPL_NOTIFY);
  UINTN n;
  UINT16 PayloadSize;
  n = SimpleBufferRead(&Snp->RxBuffer, (UINT8*)&PayloadSize, 2);
  if (n == 0)
    goto exit;
  ASSERT(n == 2);

  if (DestAddr) {
    n = SimpleBufferRead(&Snp->RxBuffer, DestAddr->Addr, 6);
    ASSERT(n == 6);
  } else
    SimpleBufferDiscard(&Snp->RxBuffer, 6);

  if (SrcAddr) {
    n = SimpleBufferRead(&Snp->RxBuffer, SrcAddr->Addr, 6);
    ASSERT(n == 6);
  } else
    SimpleBufferDiscard(&Snp->RxBuffer, 6);

  // Discard EtherType field
  SimpleBufferDiscard(&Snp->RxBuffer, 2);

  if (*Protocol) {
    // Protocol = Ethernet
    *Protocol = 1;
  }

  if (*HeaderSize)
    *HeaderSize = 14;

  UINTN CopySize = MIN(*BufferSize, PayloadSize);
  if (CopySize != PayloadSize)
    DEBUG((EFI_D_ERROR, "Buffer too small, truncating packet from %d to %d.\n", PayloadSize, CopySize));

  n = SimpleBufferPeekCopy(&Snp->RxBuffer, Buffer, CopySize);
  ASSERT(n == CopySize);

  SimpleBufferDiscard(&Snp->RxBuffer, PayloadSize);
  DEBUG((EFI_D_INFO, "Read packet with payload size %d\n", PayloadSize));
  Status = EFI_SUCCESS;
exit:
  gBS->RestoreTPL(OrigTpl);

  return Status;
}

STATIC EFI_SIMPLE_NETWORK_PROTOCOL mSimpleNetworkProtocolTemplate = {
  .Revision = EFI_SIMPLE_NETWORK_INTERFACE_REVISION,
  .Start = NetworkStart,
  .Stop = NetworkStop,
  .Initialize = NetworkInitialize,
  .Reset = NetworkReset,
  .Shutdown = NetworkShutdown,
  .ReceiveFilters = NetworkReceiveFilters,
  .StationAddress = NetworkStationAddress,
  .Statistics = NetworkStatistics,
  .MCastIpToMac = NetworkMCastIpToMac,
  .NvData = NetworkNvData,
  .GetStatus = NetworkGetStatus,
  .Transmit = NetworkTransmit,
  .Receive = NetworkReceive,
};

STATIC NETWORK_DEVICE_PATH mDevicePath = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, { sizeof (VENDOR_DEVICE_PATH), 0} },
    EFI_CALLER_ID_GUID,
  },
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, { sizeof (VENDOR_DEVICE_PATH), 0} },
    EMULATED_NETWORK_GUID,
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 } }
};

VOID
EFIAPI
SnpWaitForPacketNotify (
  EFI_EVENT     Event,
  VOID          *SnpPtr
) {
  ASSERT(0);
}

VOID NetInitMode(SNP_DRIVER *Driver) {
  EFI_SIMPLE_NETWORK_MODE *Mode = &Driver->Mode;
  Mode->State = EfiSimpleNetworkStopped;
  Mode->HwAddressSize = NET_ETHER_ADDR_LEN;
  Mode->MediaHeaderSize = 14;
  // TODO: verify this
  Mode->MaxPacketSize = 1500;
  Mode->NvRamSize = 0;
  Mode->NvRamAccessSize = 0;
  Mode->ReceiveFilterMask = EFI_SIMPLE_NETWORK_RECEIVE_PROMISCUOUS;
  Mode->MaxMCastFilterCount = 0;
  Mode->MCastFilterCount = 0;
  UINT8 *Mac = NetGetMacAddress();
  CopyMem(&Mode->CurrentAddress, Mac, 6);
  CopyMem(&Mode->PermanentAddress, Mac, 6);
  Mode->IfType = NET_IFTYPE_ETHERNET;
  Mode->MacAddressChangeable = FALSE;
  Mode->MultipleTxSupported = FALSE;
  Mode->MediaPresentSupported = FALSE;
  Mode->MediaPresent = TRUE;
}

EFI_STATUS NetInitSnp(SNP_DRIVER *Driver) {
  EFI_STATUS Status;

  CopyMem(&Driver->Protocol, &mSimpleNetworkProtocolTemplate, sizeof mSimpleNetworkProtocolTemplate);

  Status = gBS->CreateEvent(
    EVT_NOTIFY_WAIT,
    TPL_NOTIFY,
    SnpWaitForPacketNotify,
    NULL,
    &Driver->Protocol.WaitForPacket
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to create event\n"));
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  Driver->Protocol.Mode = &Driver->Mode;
  NetInitMode(Driver);
  Driver->Handle = NULL;
  Driver->Interrupt.Rx = FALSE;

  Status = SimpleBufferInit(&Driver->RxBuffer, Driver->Mode.MaxPacketSize + 16);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to allocate SNP RX buffer\n"));
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Driver->Handle,
    &gEfiSimpleNetworkProtocolGuid, &Driver->Protocol,
    &gEfiDevicePathProtocolGuid, &mDevicePath,
    NULL
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to install network protocol\n"));
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  return EFI_SUCCESS;
}
