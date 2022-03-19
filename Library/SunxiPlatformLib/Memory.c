#include <Uefi.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

#if FixedPcdGet32(FramebufferReserved) != 0
#define MAX_MEMORY_MAP_DESCRIPTORS 4
#else
#define MAX_MEMORY_MAP_DESCRIPTORS 3
#endif

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

#if FixedPcdGet32(FramebufferReserved) != 0
  VirtualMemoryTable[++Index].PhysicalBase = PcdGet64(PcdSystemMemoryBase);
  VirtualMemoryTable[Index].VirtualBase = PcdGet64(PcdSystemMemoryBase);
  VirtualMemoryTable[Index].Length = FixedPcdGet32(FramebufferReserved);
  VirtualMemoryTable[Index].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE | ARM_MEMORY_REGION_ATTRIBUTE_WRITE_THROUGH;
#endif

  VirtualMemoryTable[++Index].PhysicalBase = PcdGet64(PcdSystemMemoryBase) + FixedPcdGet32(FramebufferReserved);
  VirtualMemoryTable[Index].VirtualBase = PcdGet64(PcdSystemMemoryBase) + FixedPcdGet32(FramebufferReserved);
  VirtualMemoryTable[Index].Length = PcdGet64(PcdSystemMemorySize) - FixedPcdGet32(FramebufferReserved);
  VirtualMemoryTable[Index].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;

  VirtualMemoryTable[++Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase = 0;
  VirtualMemoryTable[Index].Length = 0;
  VirtualMemoryTable[Index].Attributes = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index + 1) == MAX_MEMORY_MAP_DESCRIPTORS);
  *VirtualMemoryMap = VirtualMemoryTable;
}
