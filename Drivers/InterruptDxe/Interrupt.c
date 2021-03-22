/**
 Copyright (c) Artur Kowalski.

 Based on:
  - Linux driver
    Copyright (C) 2012 Maxime Ripard <maxime.ripard@free-electrons.com>

    Allwinner Technology Co., Ltd. <www.allwinnertech.com>
    Benn Huang <benn@allwinnertech.com>
  
  - EDKII ARM GIC driver
    Copyright (c) 2013-2017, ARM Ltd. All rights reserved.<BR>
**/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/ArmLib.h>

#include <Protocol/Cpu.h>
#include <Protocol/HardwareInterrupt.h>

#define SUN4I_IRQ_BASE 0x1c20400

#define SUN4I_IRQ_VECTOR_REG            0x00
#define SUN4I_IRQ_PROTECTION_REG        0x08
#define SUN4I_IRQ_NMI_CTRL_REG          0x0c
#define SUN4I_IRQ_PENDING_REG(x)        (0x10 + 0x4 * x)
#define SUN4I_IRQ_FIQ_PENDING_REG(x)    (0x20 + 0x4 * x)
#define SUN4I_IRQ_ENABLE_REG(x)         (0x40 + 0x4 * x)
#define SUN4I_IRQ_MASK_REG(x)           (0x50 + 0x4 * x)

#define NUM_OF_INTERRUPT_VECTORS 96
HARDWARE_INTERRUPT_HANDLER  gRegisteredInterruptHandlers[NUM_OF_INTERRUPT_VECTORS];

VOID
EFIAPI
IrqInterruptHandler (
  IN EFI_EXCEPTION_TYPE           InterruptType,
  IN EFI_SYSTEM_CONTEXT           SystemContext
  )
{
  UINT32 Vector;
  HARDWARE_INTERRUPT_HANDLER InterruptHandler;

  Vector = MmioRead32(SUN4I_IRQ_BASE + SUN4I_IRQ_VECTOR_REG) >> 2;
  ArmDataSynchronizationBarrier();
  if (Vector == 0 &&
                  !(MmioRead32(SUN4I_IRQ_BASE + SUN4I_IRQ_PENDING_REG(0)) & 1))
  return;

  do {
    ASSERT(Vector < NUM_OF_INTERRUPT_VECTORS);
    InterruptHandler = gRegisteredInterruptHandlers[Vector];
    if (InterruptHandler != NULL)
      InterruptHandler(Vector, SystemContext);
    
    ArmDataSynchronizationBarrier();
    Vector = MmioRead32(SUN4I_IRQ_BASE + SUN4I_IRQ_VECTOR_REG) >> 2;
  } while (Vector != 0);
}

EFI_HANDLE gHardwareInterruptHandle = NULL;

/**
  Register Handler for the specified interrupt source.

  @param This     Instance pointer for this protocol
  @param Source   Hardware source of the interrupt
  @param Handler  Callback for interrupt. NULL to unregister

  @retval EFI_SUCCESS Source was updated to support Handler.
  @retval EFI_DEVICE_ERROR  Hardware could not be programmed.

**/
EFI_STATUS
EFIAPI
RegisterInterruptSource (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source,
  IN HARDWARE_INTERRUPT_HANDLER         Handler
  )
{
  if (Source > NUM_OF_INTERRUPT_VECTORS) {
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }

  if ((Handler == NULL) && (gRegisteredInterruptHandlers[Source] == NULL))
    return EFI_INVALID_PARAMETER;
  
  if ((Handler != NULL) && (gRegisteredInterruptHandlers[Source] != NULL))
    return EFI_ALREADY_STARTED;

  gRegisteredInterruptHandlers[Source] = Handler;
  
  return This->EnableInterruptSource(This, Source);
}

EFI_STATUS
EFIAPI
EnableInterruptSource (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source
  )
{
  UINTN Bank;
  UINTN Bit;

  Bank = Source / 32;
  Bit = (1ul << (Source % 32));

  MmioOr32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(Bank), Bit);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DisableInterruptSource (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source
  )
{
  UINTN Bank;
  UINTN Bit;

  Bank = Source / 32;
  Bit = (1ul << (Source % 32));
  MmioAnd32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(Bank), ~Bit);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetInterruptSourceState (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source,
  IN BOOLEAN                            *InterruptState
  )
{
  UINTN Bank;
  UINTN Bit;

  if (InterruptState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Source > NUM_OF_INTERRUPT_VECTORS) {
    ASSERT(FALSE);
    return EFI_UNSUPPORTED;
  }

  Bank = Source / 32;
  Bit = (1ul << (Source % 32));

  if ((MmioRead32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(Bank)) & Bit) == Bit)
    *InterruptState = FALSE;
  else
    *InterruptState = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EndOfInterrupt (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source
  )
{
  ArmDataSynchronizationBarrier();
  return EFI_SUCCESS;
}

EFI_HARDWARE_INTERRUPT_PROTOCOL gHardwareInterruptProtocol = {
  RegisterInterruptSource,
  EnableInterruptSource,
  DisableInterruptSource,
  GetInterruptSourceState,
  EndOfInterrupt
};

STATIC VOID *mCpuArchProtocolNotifyEventRegistration;

STATIC
VOID
EFIAPI
CpuArchEventProtocolNotify (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{
  EFI_STATUS Status;
  EFI_CPU_ARCH_PROTOCOL *Cpu;

  Status = gBS->LocateProtocol(&gEfiCpuArchProtocolGuid, NULL, (VOID**)&Cpu);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = Cpu->RegisterInterruptHandler(Cpu, EXCEPT_ARM_IRQ, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Cpu->RegisterInterruptHandler() - %r\n",
      __FUNCTION__, Status));
    ASSERT (FALSE);
    return;
  }

  Status = Cpu->RegisterInterruptHandler(Cpu, EXCEPT_ARM_IRQ, IrqInterruptHandler);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Cpu->RegisterInterruptHandler() - %r\n",
      __FUNCTION__, Status));
    ASSERT (FALSE);
    return;
  }
}

EFI_EVENT EfiExitBootServicesEvent      = (EFI_EVENT)NULL;

VOID
EFIAPI
ExitBootServicesEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  // disable all interrupts
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(0), 0);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(1), 0);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(2), 0);
}

EFI_STATUS
InterruptDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS Status;

  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gHardwareInterruptProtocolGuid);

  /* Disable all interrupts */
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(0), 0);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(1), 0);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_ENABLE_REG(2), 0);

  /* Unmask all the interrupts, ENABLE_REG(x) is used for masking */
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_MASK_REG(0), 0);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_MASK_REG(1), 0);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_MASK_REG(2), 0);

  /* Clear all the pending interrupts */
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_PENDING_REG(0), 0xFFFFFFFF);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_PENDING_REG(1), 0xFFFFFFFF);
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_PENDING_REG(2), 0xFFFFFFFF);

  /* Enable protection mode */
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_PROTECTION_REG, 1);

  /* Configure the external interrupt source type */
  MmioWrite32(SUN4I_IRQ_BASE + SUN4I_IRQ_NMI_CTRL_REG, 0);

  Status = gBS->InstallMultipleProtocolInterfaces(
    &gHardwareInterruptHandle,
    &gHardwareInterruptProtocolGuid, &gHardwareInterruptProtocol,
    NULL
  );
  ASSERT_EFI_ERROR(Status);

  EFI_EVENT CpuArchEvent = EfiCreateProtocolNotifyEvent (
    &gEfiCpuArchProtocolGuid,
    TPL_CALLBACK,
    CpuArchEventProtocolNotify,
    NULL,
    &mCpuArchProtocolNotifyEventRegistration
  );
  (VOID)CpuArchEvent;
  ASSERT(CpuArchEvent != NULL);

  Status = gBS->CreateEvent(
    EVT_SIGNAL_EXIT_BOOT_SERVICES,
    TPL_NOTIFY,
    ExitBootServicesEvent,
    NULL,
    &EfiExitBootServicesEvent
  );
  ASSERT_EFI_ERROR(Status);

  return Status;
}