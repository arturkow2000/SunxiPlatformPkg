#pragma once

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

typedef struct {
  UINT8 *Data;
  UINTN Head;
  UINTN Tail;
  UINTN Capacity;
  BOOLEAN Full;
} SIMPLE_BUFFER;

EFI_STATUS SimpleBufferInit(SIMPLE_BUFFER *Buffer, UINTN Capacity);
VOID SimpleBufferDestroy(SIMPLE_BUFFER *Buffer);
VOID SimpleBufferClear(SIMPLE_BUFFER *Buffer);
UINTN SimpleBufferLength(SIMPLE_BUFFER *Buffer);
VOID SimpleBufferWrite(SIMPLE_BUFFER *Buffer, UINT8 *Data, UINTN* Length);
UINTN SimpleBufferReadAvailable(SIMPLE_BUFFER *Buffer);
VOID SimpleBufferDiscard(SIMPLE_BUFFER *Buffer, UINTN Count);
UINTN SimpleBufferPeek(SIMPLE_BUFFER *Buffer, UINT8** Part1, UINTN *Part1Length, UINT8** Part2, UINTN *Part2Length);
