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

/**
  Set the frequency for the I2C clock line.

  This routine must be called at or below TPL_NOTIFY.

  The software and controller do a best case effort of using the specified
  frequency for the I2C bus.  If the frequency does not match exactly then
  the I2C master protocol selects the next lower frequency to avoid
  exceeding the operating conditions for any of the I2C devices on the bus.
  For example if 400 KHz was specified and the controller's divide network
  only supports 402 KHz or 398 KHz then the I2C master protocol selects 398
  KHz.  If there are not lower frequencies available, then return
  EFI_UNSUPPORTED.

  @param[in] This           Pointer to an EFI_I2C_MASTER_PROTOCOL structure
  @param[in] BusClockHertz  Pointer to the requested I2C bus clock frequency
                            in Hertz.  Upon return this value contains the
                            actual frequency in use by the I2C controller.

  @retval EFI_SUCCESS           The bus frequency was set successfully.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_INVALID_PARAMETER BusClockHertz is NULL
  @retval EFI_UNSUPPORTED       The controller does not support this frequency.

**/
STATIC
EFI_STATUS
EFIAPI
SetBusFrequency(
  IN CONST EFI_I2C_MASTER_PROTOCOL   *This,
  IN OUT UINTN                       *BusClockHertz
  )
{
  I2C_DXE_DRIVER *Driver;

  if (BusClockHertz == NULL)
    return EFI_INVALID_PARAMETER;

  Driver = DXE_DRIVER_FROM_MASTER(This);

  if (*BusClockHertz != Driver->Driver.BusClockHz)
    return EFI_UNSUPPORTED;

  return EFI_SUCCESS;
}

/**
  Reset the I2C controller and configure it for use

  This routine must be called at or below TPL_NOTIFY.

  The I2C controller is reset.  The caller must call SetBusFrequench() after
  calling Reset().

  @param[in]     This       Pointer to an EFI_I2C_MASTER_PROTOCOL structure.

  @retval EFI_SUCCESS         The reset completed successfully.
  @retval EFI_ALREADY_STARTED The controller is busy with another transaction.
  @retval EFI_DEVICE_ERROR    The reset operation failed.

**/
STATIC
EFI_STATUS
EFIAPI
Reset(
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
  )
{
  // TODO: implement
  return EFI_SUCCESS;
}

/**
  Start an I2C transaction on the host controller.

  This routine must be called at or below TPL_NOTIFY.  For synchronous
  requests this routine must be called at or below TPL_CALLBACK.

  This function initiates an I2C transaction on the controller.  To
  enable proper error handling by the I2C protocol stack, the I2C
  master protocol does not support queuing but instead only manages
  one I2C transaction at a time.  This API requires that the I2C bus
  is in the correct configuration for the I2C transaction.

  The transaction is performed by sending a start-bit and selecting the
  I2C device with the specified I2C slave address and then performing
  the specified I2C operations.  When multiple operations are requested
  they are separated with a repeated start bit and the slave address.
  The transaction is terminated with a stop bit.

  When Event is NULL, StartRequest operates synchronously and returns
  the I2C completion status as its return value.

  When Event is not NULL, StartRequest synchronously returns EFI_SUCCESS
  indicating that the I2C transaction was started asynchronously.  The
  transaction status value is returned in the buffer pointed to by
  I2cStatus upon the completion of the I2C transaction when I2cStatus
  is not NULL.  After the transaction status is returned the Event is
  signaled.

  Note: The typical consumer of this API is the I2C host protocol.
  Extreme care must be taken by other consumers of this API to prevent
  confusing the third party I2C drivers due to a state change at the
  I2C device which the third party I2C drivers did not initiate.  I2C
  platform specific code may use this API within these guidelines.

  @param[in] This           Pointer to an EFI_I2C_MASTER_PROTOCOL structure.
  @param[in] SlaveAddress   Address of the device on the I2C bus.  Set the
                            I2C_ADDRESSING_10_BIT when using 10-bit addresses,
                            clear this bit for 7-bit addressing.  Bits 0-6
                            are used for 7-bit I2C slave addresses and bits
                            0-9 are used for 10-bit I2C slave addresses.
  @param[in] RequestPacket  Pointer to an EFI_I2C_REQUEST_PACKET
                            structure describing the I2C transaction.
  @param[in] Event          Event to signal for asynchronous transactions,
                            NULL for asynchronous transactions
  @param[out] I2cStatus     Optional buffer to receive the I2C transaction
                            completion status

  @retval EFI_SUCCESS           The asynchronous transaction was successfully
                                started when Event is not NULL.
  @retval EFI_SUCCESS           The transaction completed successfully when
                                Event is NULL.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_BAD_BUFFER_SIZE   The RequestPacket->LengthInBytes value is too
                                large.
  @retval EFI_DEVICE_ERROR      There was an I2C error (NACK) during the
                                transaction.
  @retval EFI_INVALID_PARAMETER RequestPacket is NULL
  @retval EFI_NOT_FOUND         Reserved bit set in the SlaveAddress parameter
  @retval EFI_NO_RESPONSE       The I2C device is not responding to the slave
                                address.  EFI_DEVICE_ERROR will be returned if
                                the controller cannot distinguish when the NACK
                                occurred.
  @retval EFI_OUT_OF_RESOURCES  Insufficient memory for I2C transaction
  @retval EFI_UNSUPPORTED       The controller does not support the requested
                                transaction.

**/
STATIC
EFI_STATUS
EFIAPI
StartRequest(
  IN CONST EFI_I2C_MASTER_PROTOCOL *This,
  IN UINTN                         SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET        *RequestPacket,
  IN EFI_EVENT                     Event         OPTIONAL,
  OUT EFI_STATUS                   *OutI2cStatus OPTIONAL
  )
{
  EFI_STATUS Status;
  EFI_STATUS I2cStatus;
  I2C_DXE_DRIVER *Driver;
  I2C_REQUEST_BLOCK *RequestBlock;
  BOOLEAN Synchronous = FALSE;
  EFI_TPL OldTpl;
  UINT32 EventIndex;

  if (RequestPacket == NULL)
    return EFI_INVALID_PARAMETER;

  Driver = DXE_DRIVER_FROM_MASTER(This);

  // TODO: support 10-bit address
  if (SlaveAddress & ~0x7f) {
    ASSERT(0);
    return EFI_NOT_FOUND;
  }

  if (RequestPacket->OperationCount == 0)
    return EFI_INVALID_PARAMETER;

  // TODO: verify flags

  if (Event == NULL) {
    Synchronous = TRUE;
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, NULL, NULL, &Event);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;
  }

  RequestBlock = AllocatePool(sizeof(I2C_REQUEST_BLOCK));
  if (RequestBlock == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  RequestBlock->Event = Event;
  RequestBlock->OutI2cStatus = Synchronous ? &I2cStatus : OutI2cStatus;
  RequestBlock->Packet = RequestPacket;
  RequestBlock->SlaveAddress = SlaveAddress;
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  InsertTailList(&Driver->Queue, &RequestBlock->Link);
  gBS->RestoreTPL(OldTpl);

  if (Synchronous) {
    Status = gBS->WaitForEvent(1, &Event, &EventIndex);
    ASSERT_EFI_ERROR(Status);
    goto Exit;
    gBS->CloseEvent(Event);
    if (OutI2cStatus)
      *OutI2cStatus = I2cStatus;

    return I2cStatus;
  }

  Status = EFI_SUCCESS;

  Exit:
  if (Synchronous && Event)
    gBS->CloseEvent(Event);

  return EFI_SUCCESS;
}

STATIC CONST EFI_I2C_CONTROLLER_CAPABILITIES mControllerCapabilities = {
  sizeof(EFI_I2C_CONTROLLER_CAPABILITIES),
  // TODO: replace with correct values
  1,
  1,
  1
};

EFI_I2C_MASTER_PROTOCOL gSunxiI2cMasterProto = {
  SetBusFrequency,
  Reset,
  StartRequest,
  &mControllerCapabilities
};
