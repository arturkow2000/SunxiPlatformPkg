#include "Spl.h"

VOID SplVectorTable(VOID);

VOID SplSetup(VOID)
{
  // Data Cache enabled on Primary core when MMU is enabled.
  ArmDisableDataCache();
  // Invalidate instruction cache
  ArmInvalidateInstructionCache();
  // Enable Instruction Caches on all cores.
  ArmEnableInstructionCache();

  InvalidateDataCacheRange((VOID *)(UINTN)PcdGet64(PcdStackBase), PcdGet32(PcdStackSize));

  //
  // Note: Doesn't have to Enable CPU interface in non-secure world,
  // as Non-secure interface is already enabled in Secure world.
  //

  // Write VBAR - The Exception Vector table must be aligned to its requirement
  // Note: The AArch64 Vector table must be 2k-byte aligned - if this assertion fails ensure
  // 'Align=4K' is defined into your FDF for this module.
  ASSERT(((UINTN)SplVectorTable & ARM_VECTOR_TABLE_ALIGNMENT) == 0);
  ArmWriteVBar((UINTN)SplVectorTable);

  // Enable Floating Point
  if (FixedPcdGet32(PcdVFPEnabled))
  {
    ArmEnableVFP();
  }
}