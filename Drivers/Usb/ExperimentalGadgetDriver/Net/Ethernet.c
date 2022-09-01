#include "../Driver.h"

#define ETHERTYPE_IPV6 0x86dd

BOOLEAN NetDiscardPacket(SNP_DRIVER *Snp) {
  UINTN n;
  UINT16 PayloadSize;
  n = SimpleBufferRead(&Snp->RxBuffer, (UINT8*)&PayloadSize, 2);
  if (n == 0)
    return FALSE;

  ASSERT(n == 2);
  ASSERT(SimpleBufferLength(&Snp->RxBuffer) >= PayloadSize + 14);
  SimpleBufferDiscard(&Snp->RxBuffer, PayloadSize + 14);

  return TRUE;
}

VOID NetForwardPacketToSnp(SNP_DRIVER *Snp, UINT8 *Header, SIMPLE_BUFFER *Payload, UINTN PayloadLength) {
  // We put entire Ethernet frame into buffer and also we append 2 byte payload size.
  UINTN n;
  UINTN NeededFree = PayloadLength + 14 + 2;
  UINTN Length;

  DEBUG((EFI_D_INFO, "Forward packet len=%d, tgt buf len=%d\n", PayloadLength, SimpleBufferLength(&Snp->RxBuffer)));
  ASSERT(NeededFree <= Snp->RxBuffer.Capacity);

  if (NeededFree == Snp->RxBuffer.Capacity) {
    DEBUG((EFI_D_INFO, "Dropping entire RX buffer"));
    SimpleBufferClear(&Snp->RxBuffer);
  } else {
    // Discard old packets if needed
    UINTN Dropped = 0;
    while (TRUE) {
      UINTN Free = Snp->RxBuffer.Capacity - SimpleBufferLength(&Snp->RxBuffer);
      if (Free >= NeededFree)
        break;

      BOOLEAN discarded = NetDiscardPacket(Snp);
      ASSERT(discarded);
      Dropped += 1;
    }

    if (Dropped > 0)
      DEBUG((EFI_D_INFO, "Dropped %d incoming packets\n", Dropped));
  }

  UINT16 Len = PayloadLength;
  ASSERT(Len == PayloadLength);

  Length = 2;
  SimpleBufferWrite(&Snp->RxBuffer, (UINT8*)&Len, &Length);
  ASSERT(Length == 2);

  Length = 14;
  SimpleBufferWrite(&Snp->RxBuffer, Header, &Length);
  ASSERT(Length == 14);

  DEBUG((EFI_D_INFO, "Stage 0 buf len %d\n", SimpleBufferLength(&Snp->RxBuffer)));

  UINT8 *Payload1, *Payload2;
  UINTN Payload1Length, Payload2Length;
  n = SimpleBufferPeek(Payload, &Payload1, &Payload1Length, &Payload2, &Payload2Length);
  ASSERT(n >= PayloadLength);

  UINTN Left = PayloadLength;
  Length = MIN(Payload1Length, Left);
  SimpleBufferWrite(&Snp->RxBuffer, Payload1, &Length);
  ASSERT(Length == MIN(Payload1Length, Left));
  Left -= Length;

  if (Left > 0) {
    Length = MIN(Payload2Length, Left);
    SimpleBufferWrite(&Snp->RxBuffer, Payload2, &Length);
    ASSERT(Length == MIN(Payload2Length, Left));
    Left -= Length;
  }

  ASSERT(Left == 0);
  SimpleBufferDiscard(Payload, PayloadLength);

  Snp->Interrupt.Rx = TRUE;
}

VOID NetProcessEthernetFrame(SNP_DRIVER *Snp, SIMPLE_BUFFER *Buffer, UINTN Length) {
  if (Length > Snp->Mode.MaxPacketSize) {
    DEBUG((EFI_D_ERROR, "Packet size exceeds MTU, discarding...\n"));
    ASSERT(0);
    SimpleBufferDiscard(Buffer, Length);
    return;
  }

  UINT8 Header[14];
  UINTN n = SimpleBufferRead(Buffer, Header, sizeof Header);
  ASSERT(n == sizeof Header);

  UINT16 EtherType = (Header[12] << 8) | Header[13];
  switch (EtherType) {
  case ETHERTYPE_IPV6:
    break;
  default:
    DEBUG((EFI_D_WARN, "Discarding incoming packet of type 0x%04x\n", EtherType));
    SimpleBufferDiscard(Buffer, Length - sizeof Header);
    return;
  }

  NetForwardPacketToSnp(Snp, Header, Buffer, Length - sizeof Header);

#if 0
  DEBUG((EFI_D_INFO, "=====================================\n"));
  DEBUG((EFI_D_INFO, "HEXDUMP:\n"));
  UINT8 Byte;
  UINT32 Left = Length;
  UINT32 i = 0;
  while (Left > 0) {
    UINTN x = SimpleBufferRead(Buffer, &Byte, 1);
    ASSERT(x == 1);

    if (i > 0 && i % 16 == 0)
      DEBUG((EFI_D_INFO, "\n"));

    DEBUG((EFI_D_INFO, " %02x", Byte));
    Left--;
    i++;
  }
  DEBUG((EFI_D_INFO, "\n=====================================\n"));
#endif
}
