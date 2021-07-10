/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#include "Driver.h"

STATIC
EFI_STATUS
EFIAPI
I2cLazyInit(
  I2C_PEI_DRIVER *Driver
  )
{
  EFI_STATUS Status;
  ASSERT(Driver);

  if (Driver == NULL)
    return EFI_INVALID_PARAMETER;

  if (Driver->Initialized)
    return EFI_SUCCESS;

  Status = SunxiI2cInit(&Driver->Common);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Driver->Initialized = TRUE;

  return EFI_SUCCESS;
}

/**
  Set the frequency for the I2C clock line.

  @param This                   Pointer to an EFI_PEI_I2C_MASTER_PPI structure.
  @param BusClockHertz          Pointer to the requested I2C bus clock frequency in Hertz.
                                Upon return this value contains the actual frequency
                                in use by the I2C controller.

  @retval EFI_SUCCESS           The bus frequency was set successfully.
  @retval EFI_INVALID_PARAMETER BusClockHertz is NULL
  @retval EFI_UNSUPPORTED       The controller does not support this frequency.

**/
STATIC
EFI_STATUS
EFIAPI
SetBusFrequency(
  IN EFI_PEI_I2C_MASTER_PPI   *This,
  IN UINTN                    *BusClockHertz
  )
{
  ASSERT(0);
  return EFI_UNSUPPORTED;
}

/**
  Reset the I2C controller and configure it for use.

  @param  This                  Pointer to an EFI_PEI_I2C_MASTER_PPI structure.

  @retval EFI_SUCCESS           The reset completed successfully.
  @retval EFI_DEVICE_ERROR      The reset operation failed.

**/
STATIC
EFI_STATUS
EFIAPI
Reset(
  IN CONST EFI_PEI_I2C_MASTER_PPI  *This
  )
{
  ASSERT(0);
  return EFI_DEVICE_ERROR;
}

/**
  Start an I2C transaction on the host controller.

  @param  This                   Pointer to an EFI_PEI_I2C_MASTER_PPI structure.
  @param  SlaveAddress           Address of the device on the I2C bus.
                                 Set the I2C_ADDRESSING_10_BIT when using 10-bit addresses,
                                 clear this bit for 7-bit addressing.
                                 Bits 0-6 are used for 7-bit I2C slave addresses and
                                 bits 0-9 are used for 10-bit I2C slave addresses.
  @param  RequestPacket          Pointer to an EFI_I2C_REQUEST_PACKET structure describing the I2C transaction.

  @retval EFI_SUCCESS             The transaction completed successfully.
  @retval EFI_BAD_BUFFER_SIZE     The RequestPacket->LengthInBytes value is too large.
  @retval EFI_DEVICE_ERROR       There was an I2C error (NACK) during the transaction.
  @retval EFI_INVALID_PARAMETER   RequestPacket is NULL
  @retval EFI_NO_RESPONSE         The I2C device is not responding to the slave address.
                                 EFI_DEVICE_ERROR will be returned if the controller cannot distinguish when the NACK occurred.
  @retval EFI_NOT_FOUND           Reserved bit set in the SlaveAddress parameter
  @retval EFI_OUT_OF_RESOURCES   Insufficient memory for I2C transaction
  @retval EFI_UNSUPPORTED         The controller does not support the requested transaction.

**/
STATIC
EFI_STATUS
EFIAPI
StartRequest(
  IN CONST EFI_PEI_I2C_MASTER_PPI     *This,
  IN UINTN                            SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET           *RequestPacket
  )
{
  EFI_STATUS Status;
  I2C_PEI_DRIVER *PeiDriver = PEI_DRIVER_FROM_MASTER(This);

  if (RequestPacket == NULL)
    return EFI_INVALID_PARAMETER;

  Status = I2cLazyInit(PeiDriver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  // TODO: support 10-bit address
  if (SlaveAddress & ~0x7f) {
    ASSERT(0);
    return EFI_NOT_FOUND;
  }

  if (RequestPacket->OperationCount == 0)
    return EFI_INVALID_PARAMETER;

  return SunxiI2cExecute(PeiDriver, SlaveAddress, RequestPacket);
}

STATIC CONST EFI_I2C_CONTROLLER_CAPABILITIES mCapabilities = {
  sizeof(EFI_I2C_CONTROLLER_CAPABILITIES),
  32,
  32,
  32
};

EFI_PEI_I2C_MASTER_PPI gI2cMasterPpi = {
  SetBusFrequency,
  Reset,
  StartRequest,
  &mCapabilities,
  {
    0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  }
};
