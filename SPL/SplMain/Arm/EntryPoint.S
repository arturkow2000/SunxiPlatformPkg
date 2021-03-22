//  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
//  Copyright (c) 2021, Artur Kowalski.
//  Copyright (c) 2015 Google, Inc
//
//  SPDX-License-Identifier: GPL-2.0+
//

#include <AsmMacroIoLib.h>

ASM_FUNC(_ModuleEntryPoint)
  mov lr, r0
  ldr r0, =fel_stash
  str sp, [r0, #0]
  str lr, [r0, #4]
  mrs lr, cpsr
  str lr, [r0, #8]
  mrc p15, 0, lr, c1, c0, 0
  str lr, [r0, #12]
  mrc p15, 0, lr, c12, c0, 0  @ Read VBAR
  str lr, [r0, #16]
  mrc p15, 0, lr, c1, c0, 0   @ Read CP15 Control Register
  str lr, [r0, #20]

  MOV32 (r0, FixedPcdGet32(PcdStackBase) + FixedPcdGet32(PcdStackSize))
  mov sp, r0

  bl ASM_PFX(SplHWInitSoc)
  bl ASM_PFX(SplSetup)
  bl ASM_PFX(SplTimerInit)
  bl ASM_PFX(SplMain)

  1:
  wfi
  b 1b

.global fel_stash
fel_stash:
  .word 0 @ SP
  .word 0 @ LR
  .word 0 @ CPSR
  .word 0 @ SCTLR
  .word 0 @ VBAR
  .word 0 @ CR
.size fel_stash, . - fel_stash

ASM_FUNC(_ReturnToFel)
  ldr r0, =fel_stash
  ldr sp, [r0, #0]
  ldr lr, [r0, #4]
  ldr r1, [r0, #20]
  mcr p15, 0, r1, c1, c0, 0   @ Write CP15 Control Register
  ldr r1, [r0, #16]
  mcr p15, 0, r1, c12, c0, 0  @ Write VBAR
  ldr r1, [r0, #12]
  mcr p15, 0, r1, c1, c0, 0   @ Write CP15 SCTLR Register
  ldr r1, [r0, #8]
  msr cpsr, r1                @ Write CPSR
  bx lr