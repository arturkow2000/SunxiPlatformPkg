/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright (c) 2021, Artur Kowalski.

  Based on PCF8563 library from edk2-platforms
  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

**/

#include "Driver.h"

#define PCF8563_DATA_REG_OFFSET   0x2

#define PCF8563_CLOCK_INVALID     0x80
#define PCF8563_SECONDS_MASK      0x7f
#define PCF8563_MINUTES_MASK      0x7f
#define PCF8563_HOURS_MASK        0x3f
#define PCF8563_DAYS_MASK         0x3f
#define PCF8563_WEEKDAYS_MASK     0x07
#define PCF8563_MONTHS_MASK       0x1f
#define PCF8563_CENTURY_MASK      0x80

//
// The PCF8563 has a 'century' flag, which means it could theoretically span
// 200 years. However, it treats all years divisible by 4 as leap years,
// including the years 1900 and 2100 (which are not leap years), so the only
// sane base year is 2000 (which was a leap year).
//
#define EPOCH_BASE                2000

#pragma pack(1)
typedef struct {
  UINT8                           VL_seconds;
  UINT8                           Minutes;
  UINT8                           Hours;
  UINT8                           Days;
  UINT8                           Weekdays;
  UINT8                           Century_months;
  UINT8                           Years;
} RTC_DATETIME;

typedef struct {
  UINT8                           Reg;
  RTC_DATETIME                    DateTime;
} RTC_SET_DATETIME_PACKET;
#pragma pack()

typedef struct {
  UINTN                           OperationCount;
  EFI_I2C_OPERATION               SetAddressOp;
  EFI_I2C_OPERATION               GetDateTimeOp;
} RTC_GET_I2C_REQUEST;

STATIC
EFI_STATUS
EFIAPI
GetTime(
  IN  PCF8563_PROTOCOL       *This,
  OUT EFI_TIME               *Time,
  OUT EFI_TIME_CAPABILITIES  *Capabilities
  )
{
  DRIVER_DATA *Data = (DRIVER_DATA*)This;
  EFI_STATUS Status;
  RTC_GET_I2C_REQUEST Packet;
  UINT8 Reg;
  RTC_DATETIME DateTime;

  if (Time == NULL)
    return EFI_INVALID_PARAMETER;

  Reg = PCF8563_DATA_REG_OFFSET;

  Packet.OperationCount = 2;
  Packet.SetAddressOp.Buffer = &Reg;
  Packet.SetAddressOp.Flags = 0;
  Packet.SetAddressOp.LengthInBytes = 1;
  
  Packet.GetDateTimeOp.Buffer = (UINT8*)&DateTime;
  Packet.GetDateTimeOp.Flags = I2C_FLAG_READ;
  Packet.GetDateTimeOp.LengthInBytes = sizeof(RTC_DATETIME);

  Status = Data->I2cIo->QueueRequest(Data->I2cIo, 0, NULL, (EFI_I2C_REQUEST_PACKET*)&Packet, NULL);
  if (EFI_ERROR(Status))
    return EFI_DEVICE_ERROR;

  if ((DateTime.VL_seconds & PCF8563_CLOCK_INVALID) != 0) {
    Time->Nanosecond = 0;
    Time->Second  = 0;
    Time->Minute  = 0;
    Time->Hour    = 0;
    Time->Day     = 1;
    Time->Month   = 1;
    Time->Year    = EPOCH_BASE;
  } else {
    Time->Nanosecond = 0;
    Time->Second  = BcdToDecimal8 (DateTime.VL_seconds & PCF8563_SECONDS_MASK);
    Time->Minute  = BcdToDecimal8 (DateTime.Minutes & PCF8563_MINUTES_MASK);
    Time->Hour    = BcdToDecimal8 (DateTime.Hours & PCF8563_HOURS_MASK);
    Time->Day     = BcdToDecimal8 (DateTime.Days & PCF8563_DAYS_MASK);
    Time->Month   = BcdToDecimal8 (DateTime.Century_months & PCF8563_MONTHS_MASK);
    Time->Year    = BcdToDecimal8 (DateTime.Years) + EPOCH_BASE;

    if (DateTime.Century_months & PCF8563_CENTURY_MASK)
      Time->Year += 100;
  }

  if (Capabilities != NULL) {
    Capabilities->Resolution = 1;
    Capabilities->Accuracy = 0;
    Capabilities->SetsToZero = TRUE;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SetTime(
  IN  PCF8563_PROTOCOL       *This,
  IN  EFI_TIME               *Time
  )
{
  DRIVER_DATA *Data = (DRIVER_DATA*)This;
  EFI_STATUS Status;
  EFI_I2C_REQUEST_PACKET I2cPacket;
  RTC_SET_DATETIME_PACKET Packet;

  if (Time == NULL)
    return EFI_INVALID_PARAMETER;

  Packet.DateTime.VL_seconds          = DecimalToBcd8 (Time->Second);
  Packet.DateTime.Minutes             = DecimalToBcd8 (Time->Minute);
  Packet.DateTime.Hours               = DecimalToBcd8 (Time->Hour);
  Packet.DateTime.Days                = DecimalToBcd8 (Time->Day);
  Packet.DateTime.Weekdays            = 0;
  Packet.DateTime.Century_months      = DecimalToBcd8 (Time->Month);
  Packet.DateTime.Years               = DecimalToBcd8 (Time->Year % 100);
  if (Time->Year >= EPOCH_BASE + 100) {
    if (Time->Year >= EPOCH_BASE + 200) {
      return EFI_DEVICE_ERROR;
    }
    Packet.DateTime.Century_months    |= PCF8563_CENTURY_MASK;
  }

  Packet.Reg = PCF8563_DATA_REG_OFFSET;

  I2cPacket.OperationCount = 1;
  I2cPacket.Operation[0].Buffer = (UINT8*)&Packet;
  I2cPacket.Operation[0].Flags = 0;
  I2cPacket.Operation[0].LengthInBytes = sizeof(RTC_SET_DATETIME_PACKET);

  Status = Data->I2cIo->QueueRequest(Data->I2cIo, 0, NULL, &I2cPacket, NULL);
  if (EFI_ERROR(Status))
    return EFI_DEVICE_ERROR;

  return EFI_SUCCESS;
}

PCF8563_PROTOCOL gPcf8563Protocol = {
  GetTime,
  SetTime
};
