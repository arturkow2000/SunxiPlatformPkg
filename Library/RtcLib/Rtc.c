/** @file

  SPDX-License-Identifier: BSD-3-Clause

  Copyright (c) 2021, Artur Kowalski.

**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/RealTimeClockLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/Pcf8563.h>

// FIXME: Currently RTC after exiting boot services is not supported
// if OS uses paging, addresses must be translated from physical to virtual
// which currently is not supported in I2C driver
STATIC BOOLEAN mBootServicesDone = FALSE;

/**
  Returns the current time and date information, and the time-keeping
  capabilities of the hardware platform.

  @param  Time                  A pointer to storage to receive a snapshot of
                                the current time.
  @param  Capabilities          An optional pointer to a buffer to receive the
                                real time clock device's capabilities.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER Time is NULL.
  @retval EFI_DEVICE_ERROR      The time could not be retrieved due to hardware
                                error.

**/
EFI_STATUS
EFIAPI
LibGetTime (
  OUT EFI_TIME                *Time,
  OUT EFI_TIME_CAPABILITIES   *Capabilities
  )
{
  EFI_STATUS Status;
  PCF8563_PROTOCOL *Pcf;

  if (mBootServicesDone)
    return EFI_DEVICE_ERROR;

  Status = gBS->LocateProtocol(&gPcf8563ProtocolGuid, NULL, (VOID**)&Pcf);
  if (EFI_ERROR(Status))
    return EFI_DEVICE_ERROR;

  return Pcf->GetTime(Pcf, Time, Capabilities);
}

/**
  Sets the current local time and date information.

  @param  Time                  A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware
                                error.

**/
EFI_STATUS
EFIAPI
LibSetTime (
  IN EFI_TIME                *Time
  )
{
  EFI_STATUS Status;
  PCF8563_PROTOCOL *Pcf;

  if (mBootServicesDone)
    return EFI_DEVICE_ERROR;

  Status = gBS->LocateProtocol(&gPcf8563ProtocolGuid, NULL, (VOID**)&Pcf);
  if (EFI_ERROR(Status))
    return EFI_DEVICE_ERROR;

  return Pcf->SetTime(Pcf, Time);
}

/**
  Returns the current wakeup alarm clock setting.

  @param  Enabled               Indicates if the alarm is currently enabled or
                                disabled.
  @param  Pending               Indicates if the alarm signal is pending and
                                requires acknowledgement.
  @param  Time                  The current alarm setting.

  @retval EFI_SUCCESS           The alarm settings were returned.
  @retval EFI_INVALID_PARAMETER Any parameter is NULL.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a
                                hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this
                                platform.

**/
EFI_STATUS
EFIAPI
LibGetWakeupTime (
  OUT BOOLEAN     *Enabled,
  OUT BOOLEAN     *Pending,
  OUT EFI_TIME    *Time
  )
{
  //
  // Currently unimplemented. The PCF8563 does not support setting the alarm
  // for an arbitrary date/time, but only for a minute/hour/day/weekday
  // combination. It should also depend on a platform specific setting that
  // indicates whether the PCF8563's interrupt line is connected in a way that
  // allows it to power up the system in the first place.
  //
  return EFI_UNSUPPORTED;
}

/**
  Sets the system wakeup alarm clock time.

  @param  Enabled               Enable or disable the wakeup alarm.
  @param  Time                  If Enable is TRUE, the time to set the wakeup
                                alarm for.

  @retval EFI_SUCCESS           If Enable is TRUE, then the wakeup alarm was
                                enabled. If Enable is FALSE, then the wakeup
                                alarm was disabled.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be set due to a
                                hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this
                                platform.

**/
EFI_STATUS
EFIAPI
LibSetWakeupTime (
  IN BOOLEAN      Enabled,
  OUT EFI_TIME    *Time
  )
{
  // see comment above
  return EFI_UNSUPPORTED;
}

EFI_EVENT EfiExitBootServicesEvent = (EFI_EVENT)NULL;

VOID
EFIAPI
ExitBootServicesEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  mBootServicesDone = TRUE;
}

/**
  Library entry point

  @param  ImageHandle           Handle that identifies the loaded image.
  @param  SystemTable           System Table for this image.

  @retval EFI_SUCCESS           The operation completed successfully.

**/
EFI_STATUS
EFIAPI
LibRtcInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS Status;

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
