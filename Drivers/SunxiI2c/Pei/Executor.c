#include "Driver.h"

STATIC
VOID
EFIAPI
WaitForInterrupt(
  IN I2C_PEI_DRIVER *Driver,
  OUT UINT32 *Status
  )
{
  ASSERT(Driver);
  ASSERT(Status);

  while (!SunxiI2cPoll(&Driver->Common, Status)) {
    CpuPause();
  }
}

STATIC
EFI_STATUS
EFIAPI
ExpectStatus(
  IN I2C_PEI_DRIVER *Driver,
  IN UINT32 ExpectedStatus
  )
{
  UINT32 Status;

  WaitForInterrupt(Driver, &Status);

  if (Status != ExpectedStatus) {
    DEBUG((EFI_D_ERROR, "i2c%d expected status 0x%x, got 0x%x\n", Driver->Common.Config->No, ExpectedStatus, Status));
    return EFI_DEVICE_ERROR;
  } else
    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
TaskFailed(
  IN I2C_PEI_DRIVER *Driver,
  EFI_STATUS Status
  )
{
  // TODO: reset controller

  return Status;
}

EFI_STATUS
EFIAPI
SunxiI2cExecute(
  IN I2C_PEI_DRIVER *Driver,
  IN UINTN SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET *RequestPacket
  )
{
  EFI_STATUS Status;
  UINTN CurrentOperationIndex;
  UINT32 j;
  EFI_I2C_OPERATION *Operation;
  UINT32 InterruptStatus;

  ASSERT(Driver);
  ASSERT(RequestPacket);
  ASSERT(RequestPacket->OperationCount > 0);

  if (Driver == NULL || RequestPacket == NULL || RequestPacket->OperationCount == 0)
    return EFI_INVALID_PARAMETER;

  for (CurrentOperationIndex = 0; CurrentOperationIndex < RequestPacket->OperationCount; CurrentOperationIndex++) {
    Operation = &RequestPacket->Operation[CurrentOperationIndex];
    ASSERT(Operation->LengthInBytes > 0);

    // TODO: ensure bus is idle before sending START
    Status = SunxiI2cStart(&Driver->Common, CurrentOperationIndex > 0);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "i2c%d failed to send START\n", Driver->Common.Config->No));
      ASSERT_EFI_ERROR(Status);
      return EFI_DEVICE_ERROR;
    }

    Status = ExpectStatus(Driver, CurrentOperationIndex > 0 ? SUNXI_I2C_STA_TX_RESTA : SUNXI_I2C_STA_TX_STA);
    if (EFI_ERROR(Status))
      return TaskFailed(Driver, Status);

    SunxiI2cWriteByte(&Driver->Common, (SlaveAddress << 1) | ((Operation->Flags & I2C_FLAG_READ) == I2C_FLAG_READ));
    Status = ExpectStatus(Driver, (Operation->Flags & I2C_FLAG_READ) ? SUNXI_I2C_STA_TX_SLAR_ACK : SUNXI_I2C_STA_TX_SLAW_ACK);
    if (EFI_ERROR(Status))
      return TaskFailed(Driver, Status);

    // TODO: support 10-bit addresses

    // do data transfer
    if (Operation->Flags & I2C_FLAG_READ) {
      if (Operation->LengthInBytes > 1)
        MmioAndThenOr32(Driver->Common.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG, SUNXI_I2C_CTL_ACK);
      else
        MmioAnd32(Driver->Common.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);

      for (j = 0; j < Operation->LengthInBytes; j++) {
        WaitForInterrupt(Driver, &InterruptStatus);

        if (InterruptStatus == SUNXI_I2C_STA_RXD_ACK) {
          if (j == Operation->LengthInBytes - 1)
            MmioAnd32(Driver->Common.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_ACK);

          Operation->Buffer[j] = MmioRead32(Driver->Common.Config->Base + SUNXI_I2C_DATA) & 0xff;

          // Get next byte
          MmioAnd32(Driver->Common.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);
        } else if (InterruptStatus == SUNXI_I2C_STA_RXD_NAK) {
          Operation->Buffer[j] = MmioRead32(Driver->Common.Config->Base + SUNXI_I2C_DATA) & 0xff;

          // Incomplete transfer
          if (j != Operation->LengthInBytes - 1) {
            DEBUG((
              EFI_D_ERROR,
              "i2c%d incomplete transfer, expected %d bytes got %d\n",
              Driver->Common.Config->No,
              Operation->LengthInBytes,
              j
            ));
            return TaskFailed(Driver, EFI_DEVICE_ERROR);
          }
        } else {
          DEBUG((
            EFI_D_ERROR,
            "i2c%d error 0x%x during data transfer (read)\n",
            Driver->Common.Config->No,
            InterruptStatus
          ));
          return TaskFailed(Driver, EFI_DEVICE_ERROR);
        }
      }
    } else {
      for (j = 0; j < Operation->LengthInBytes; j++) {
        MmioWrite32(Driver->Common.Config->Base + SUNXI_I2C_DATA, Operation->Buffer[j]);
        MmioAnd32(Driver->Common.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);

        WaitForInterrupt(Driver, &InterruptStatus);
        Status = ExpectStatus(Driver, SUNXI_I2C_STA_TXD_ACK);
        if (EFI_ERROR(Status))
          return TaskFailed(Driver, Status);
      }
    }
  }

  Status = SunxiI2cStop(&Driver->Common);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "i2c%d failed to send STOP\n", Driver->Common.Config->No));
    return TaskFailed(Driver, EFI_DEVICE_ERROR);
  }

  return EFI_SUCCESS;
}
