#include <AsmMacroIoLib.h>
#include <AutoGen.h>

.text
.align 2

GCC_ASM_EXPORT(ArmPlatformIsPrimaryCore)
GCC_ASM_EXPORT(ArmPlatformGetPrimaryCoreMpId)
GCC_ASM_EXPORT(ArmPlatformPeiBootAction)
GCC_ASM_IMPORT(ArmReadMpidr)

ASM_PFX(ArmPlatformIsPrimaryCore):
  mov   r0, #1
  bx    lr

ASM_PFX(ArmPlatformPeiBootAction):
  // TODO: remove unneeded
  mrc     p15, 0, r0, c1, c0, 0
  bic     r0, r0, #0x00002000     @ clear bits 13 (--V-)
  bic     r0, r0, #0x00000007     @ clear bits 2:0 (-CAM)
  bic     r0, r0, #0x00000002     @ clear bit 1 (--A-) Align
  orr     r0, r0, #0x00000800     @ set bit 11 (Z---) BTB
  bic     r0, r0, #0x00001000     @ clear bit 12 (I) I-cache
  mcr     p15, 0, r0, c1, c0, 0

  mov     r0, #0                  @ set up for MCR
  mcr     p15, 0, r0, c8, c7, 0   @ invalidate TLBs
  mcr     p15, 0, r0, c7, c5, 0   @ invalidate icache
  mcr     p15, 0, r0, c7, c5, 6   @ invalidate BP array
  mcr     p15, 0, r0, c7, c10, 4  @ DSB
  mcr     p15, 0, r0, c7, c5, 4   @ ISB
  bx      lr

ASM_PFX(ArmPlatformGetPrimaryCoreMpId):
  b   ASM_PFX(ArmReadMpidr)

ASM_FUNCTION_REMOVE_IF_UNREFERENCED
