#include <Uefi.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

#define MAX_MEMORY_MAP_DESCRIPTORS 3

VOID ArmPlatformGetVirtualMemoryMap(
    IN ARM_MEMORY_REGION_DESCRIPTOR **VirtualMemoryMap)
{
  UINTN Index = 0;

  ASSERT(VirtualMemoryMap != NULL);

  ARM_MEMORY_REGION_DESCRIPTOR *VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR *)AllocatePages(EFI_SIZE_TO_PAGES(sizeof(ARM_MEMORY_REGION_DESCRIPTOR) * MAX_MEMORY_MAP_DESCRIPTORS));
  ASSERT(VirtualMemoryTable != NULL);

  DebugPrint(EFI_D_INFO, "Building memory descriptors ...\n");

  VirtualMemoryTable[Index].PhysicalBase = 0x00000000;
  VirtualMemoryTable[Index].VirtualBase = 0x00000000;
  VirtualMemoryTable[Index].Length = 0x40000000;
  VirtualMemoryTable[Index].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  VirtualMemoryTable[++Index].PhysicalBase = PcdGet64(PcdSystemMemoryBase);
  VirtualMemoryTable[Index].VirtualBase = PcdGet64(PcdSystemMemoryBase);
  VirtualMemoryTable[Index].Length = PcdGet64(PcdSystemMemorySize);
  VirtualMemoryTable[Index].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;

  /*VirtualMemoryTable[++Index].PhysicalBase = 0x7F000000;
  VirtualMemoryTable[Index].VirtualBase = 0x7F000000;
  VirtualMemoryTable[Index].Length = 0x1000000;
  VirtualMemoryTable[Index].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_THROUGH | ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;*/

  VirtualMemoryTable[++Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase = 0;
  VirtualMemoryTable[Index].Length = 0;
  VirtualMemoryTable[Index].Attributes = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index + 1) == MAX_MEMORY_MAP_DESCRIPTORS);
  *VirtualMemoryMap = VirtualMemoryTable;
}
