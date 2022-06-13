#include "Buffer.h"

EFI_STATUS SimpleBufferInit(SIMPLE_BUFFER *Buffer, UINTN Capacity) {
  ASSERT(Buffer);
  ASSERT(Capacity);

  Buffer->Data = (UINT8*)AllocatePool(Capacity);
  if (!Buffer->Data)
    return EFI_OUT_OF_RESOURCES;

  Buffer->Head = 0;
  Buffer->Tail = 0;
  Buffer->Capacity = Capacity;
  Buffer->Full = FALSE;

  return EFI_SUCCESS;
}

VOID SimpleBufferDestroy(SIMPLE_BUFFER *Buffer) {
  ASSERT(Buffer);
  ASSERT(Buffer->Data);
  FreePool(Buffer->Data);
}

VOID SimpleBufferClear(SIMPLE_BUFFER *Buffer) {
  ASSERT(Buffer);
  Buffer->Head = 0;
  Buffer->Tail = 0;
  Buffer->Full = FALSE;
}

UINTN SimpleBufferLength(SIMPLE_BUFFER *Buffer) {
  ASSERT(Buffer);

  if (Buffer->Full) {
    ASSERT(Buffer->Head == Buffer->Tail);
    return Buffer->Capacity;
  }

  if (Buffer->Head == Buffer->Tail)
    return 0;
  else if (Buffer->Head < Buffer->Tail)
    return Buffer->Tail - Buffer->Head;
  else
    return Buffer->Capacity - Buffer->Head + Buffer->Tail;
}

BOOLEAN SimpleBufferIsEmpty(SIMPLE_BUFFER *Buffer) {
  ASSERT(Buffer);

  if (Buffer->Full) {
    ASSERT(Buffer->Head == Buffer->Tail);
    return FALSE;
  }

  return Buffer->Tail == Buffer->Head;
}

VOID SimpleBufferWrite(SIMPLE_BUFFER *Buffer, UINT8 *Data, UINTN* Length) {
  UINTN n;
  UINTN t;
  UINTN u = 0;

  ASSERT(Buffer);
  ASSERT(Data);
  ASSERT(Length);
  ASSERT(*Length);

  if (Buffer->Full) {
    ASSERT(Buffer->Head == Buffer->Tail);
    *Length = 0;
    return;
  }

  t = *Length;

  if (Buffer->Head == Buffer->Tail) {
    n = MIN(t, Buffer->Capacity);
    CopyMem(Buffer->Data, Data, n);
    if (n == Buffer->Capacity)
      Buffer->Full = TRUE;
    *Length = n;
    Buffer->Tail += n;
    if (Buffer->Tail >= Buffer->Capacity)
      Buffer->Tail = Buffer->Capacity - Buffer->Tail;
  } else if (Buffer->Head > Buffer->Tail) {
tail_lower_than_head:
    n = MIN(t, Buffer->Head - Buffer->Tail);
    CopyMem(&Buffer->Data[Buffer->Tail], Data, n);
    Buffer->Tail += n;
    if (Buffer->Tail >= Buffer->Capacity)
      Buffer->Tail = Buffer->Capacity - Buffer->Tail;

    if (Buffer->Tail == Buffer->Head) {
      ASSERT(Buffer->Tail != Buffer->Capacity);
      Buffer->Full = TRUE;
    }
    *Length = n + u;
  } else if (Buffer->Head < Buffer->Tail) {
    n = MIN(t, Buffer->Capacity - Buffer->Head);
    CopyMem(&Buffer->Data[Buffer->Tail], Data, n);
    if (Buffer->Tail + n == Buffer->Capacity) {
      Buffer->Tail = 0;
      if (Buffer->Head != 0) {
        if (n != t) {
          u = t;
          goto tail_lower_than_head;
        }
      } else {
        Buffer->Full = TRUE;
      }
      *Length = n;
    }
    else
      Buffer->Tail += n;
  } else {
    ASSERT(0);
  }
}

VOID SimpleBufferDiscard(SIMPLE_BUFFER *Buffer, UINTN Count) {
  UINTN Length;

  ASSERT(Buffer);
  
  if (Count == 0)
    return;

  Length = SimpleBufferLength(Buffer);
  if (Length == 0)
    return;

  if (Count >= Length)
    return SimpleBufferClear(Buffer);

  Buffer->Head += Count;
  if (Buffer->Head >= Buffer->Capacity)
    Buffer->Head = Buffer->Head - Buffer->Capacity;

  Buffer->Full = FALSE;
}

UINTN SimpleBufferPeek(
  SIMPLE_BUFFER *Buffer,
  UINT8** Part1,
  UINTN *Part1Length,
  UINT8** Part2,
  UINTN *Part2Length
) {
  UINTN Total;

  ASSERT(Buffer);
  ASSERT(Part1);

  *Part1 = NULL;
  if (Part1Length) *Part1Length = 0;
  if (Part2) *Part2 = NULL;
  if (Part2Length) *Part2Length = 0;

  if (!Buffer->Full && Buffer->Head == Buffer->Tail)
    return 0;

  if (Buffer->Head > Buffer->Tail) {
    *Part1 = &Buffer->Data[Buffer->Head];
    Total = Buffer->Capacity - Buffer->Head;
    if (Part1Length)
      *Part1Length = Total;

    if (Buffer->Tail != 0 && Part2) {
      ASSERT(Buffer->Tail != Buffer->Capacity);
      *Part2 = Buffer->Data;
      if (Part2Length)
        *Part2Length = Buffer->Tail;
      Total += Buffer->Tail;
    }
  } else {
    Total = Buffer->Tail - Buffer->Head;
    *Part1 = &Buffer->Data[Buffer->Head];
    if (Part1Length)
      *Part1Length = Total;
  }

  return Total;
}

UINTN SimpleBufferRead(SIMPLE_BUFFER *Buffer, UINT8* Destination, UINTN Length) {
  UINT8* Part1, *Part2;
  UINTN Part1Length, Part2Length;
  UINTN n;
  UINTN o;

  ASSERT(Buffer);
  ASSERT(Destination);
  ASSERT(Length);

  SimpleBufferPeek(Buffer, &Part1, &Part1Length, &Part2, &Part2Length);

  o = 0;
  n = MIN(Part1Length, Length);
  CopyMem(Destination, Part1, n);

  o += n;
  if (n != Length) {
    n += n;
    Length -= n;
    n = MIN(Part2Length, Length);
    CopyMem(&Destination[o], Part2, n);
    o += n;
  }

  SimpleBufferDiscard(Buffer, o);
  return o;
}
