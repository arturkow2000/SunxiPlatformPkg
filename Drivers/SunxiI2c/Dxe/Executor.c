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

#define STAGE_SEND_START_COND 0
#define STAGE_SEND_SLA 1
#define STAGE_WAIT_SLA_RESP 2
#define STAGE_DATA_TRANSFER 3

VOID
EFIAPI
TaskFailed(
  IN I2C_DXE_DRIVER *Driver,
  IN EFI_STATUS Status
  )
{
  I2C_REQUEST_BLOCK *Request = Driver->CurrentTask.Request;
  ASSERT(Request);
  ASSERT(Request->Event);
  
  if (Request->OutI2cStatus)
    *Request->OutI2cStatus = Status;
  
  gBS->SignalEvent(Request->Event);

  FreePool(Request);

  // Proceed to next request
  Driver->CurrentTask.Request = NULL;

  // TODO: reset controller
}

VOID
EFIAPI
TaskOk(
  IN I2C_DXE_DRIVER *Driver
  )
{
  I2C_REQUEST_BLOCK *Request = Driver->CurrentTask.Request;
  ASSERT(Request);
  ASSERT(Request->Event);

  if (Request->OutI2cStatus)
    *Request->OutI2cStatus = EFI_SUCCESS;
  
  gBS->SignalEvent(Request->Event);
  FreePool(Request);
  
  // Proceed to next request
  Driver->CurrentTask.Request = NULL;
}

VOID
EFIAPI
FinishTask(
  IN I2C_DXE_DRIVER *Driver
  )
{
  EFI_STATUS Status;

  Status = SunxiI2cStop(&Driver->Driver);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "i2c%d failed to send STOP\n", Driver->Driver.Config->No));
    TaskFailed(Driver, EFI_DEVICE_ERROR);
    return;
  }

  TaskOk(Driver);
}

VOID
EFIAPI
RunTask(
  IN I2C_DXE_DRIVER *Driver
  )
{
  EFI_STATUS Status;
  UINT32 InterruptStatus;
  EFI_I2C_OPERATION *Operation;
  UINT32 i;

  ASSERT(Driver->CurrentTask.Request != NULL);
  ASSERT(Driver->CurrentTask.Request->Packet->OperationCount > 0);

  Operation = &Driver->CurrentTask.Request->Packet->Operation[Driver->CurrentTask.CurrentOperation];

  ASSERT(Operation->LengthInBytes > 0);

  switch (Driver->CurrentTask.Stage) {
  /* Send START condition */
  case STAGE_SEND_START_COND:
    // TODO: ensure bus is idle before sending START

    Status = SunxiI2cStart(&Driver->Driver, Driver->CurrentTask.CurrentOperation > 0);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "i2c%d failed to send START\n", Driver->Driver.Config->No));
      TaskFailed(Driver, EFI_DEVICE_ERROR);
      return;
    }
    break;
  case STAGE_SEND_SLA:
    // Wait till controller signals that START has been sent
    if (!SunxiI2cPoll(&Driver->Driver, &InterruptStatus))
      return;

    if (InterruptStatus != SUNXI_I2C_STA_TX_STA && InterruptStatus != SUNXI_I2C_STA_TX_RESTA) {
      DEBUG((EFI_D_ERROR, "i2c%d failed to send START\n", Driver->Driver.Config->No));
      TaskFailed(Driver, EFI_DEVICE_ERROR);
      return;
    }

    if (Driver->CurrentTask.CurrentOperation > 0)
      ASSERT(InterruptStatus == SUNXI_I2C_STA_TX_RESTA);
    else
      ASSERT(InterruptStatus == SUNXI_I2C_STA_TX_STA);
    
    // Send slave address
    SunxiI2cWriteByte(&Driver->Driver, (Driver->CurrentTask.Request->SlaveAddress << 1) | ((Operation->Flags & I2C_FLAG_READ) == I2C_FLAG_READ));
    break;
  case STAGE_WAIT_SLA_RESP:
    if (!SunxiI2cPoll(&Driver->Driver, &InterruptStatus))
      return;

    if (InterruptStatus != ((Operation->Flags & I2C_FLAG_READ) ? SUNXI_I2C_STA_TX_SLAR_ACK : SUNXI_I2C_STA_TX_SLAW_ACK)) {
      if (InterruptStatus == SUNXI_I2C_STA_TX_SLAR_NAK || InterruptStatus == SUNXI_I2C_STA_TX_SLAW_NAK)
        DEBUG((EFI_D_ERROR, "i2c%d got NAK in response to SLA+%c\n", Driver->Driver.Config->No, Operation->Flags & I2C_FLAG_READ ? L'R' : L'W'));

      TaskFailed(Driver, EFI_DEVICE_ERROR);
      return;
    }

    // TODO: support 10-bit addresses

    if (Operation->Flags & I2C_FLAG_READ) {
      if (Operation->LengthInBytes > 1)
        MmioAndThenOr32(Driver->Driver.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG, SUNXI_I2C_CTL_ACK);
      else
        MmioAnd32(Driver->Driver.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);
    } else {
      // send first data byte
      ASSERT(Driver->CurrentTask.NumTransferedBytes == 0);
      MmioWrite32(Driver->Driver.Config->Base + SUNXI_I2C_DATA, Operation->Buffer[Driver->CurrentTask.NumTransferedBytes]);
      MmioAnd32(Driver->Driver.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);
    }

    Driver->CurrentTask.Stage = STAGE_DATA_TRANSFER;    
    /* Fall through */
  case STAGE_DATA_TRANSFER:
    if (!SunxiI2cPoll(&Driver->Driver, &InterruptStatus))
      return;

    i = Driver->CurrentTask.NumTransferedBytes;

    if (Operation->Flags & I2C_FLAG_READ) {
      if (InterruptStatus == SUNXI_I2C_STA_RXD_ACK) {
        // If it's the last byte then don't ACK
        if (i == Operation->LengthInBytes - 1)
          MmioAnd32(Driver->Driver.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_ACK);
        Operation->Buffer[i] = MmioRead32(Driver->Driver.Config->Base + SUNXI_I2C_DATA) & 0xff;
        // Get next byte
        MmioAnd32(Driver->Driver.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);
      } else if (InterruptStatus == SUNXI_I2C_STA_RXD_NAK) {
        // Got last byte
        Operation->Buffer[i] = MmioRead32(Driver->Driver.Config->Base + SUNXI_I2C_DATA) & 0xff;
        
        if (Driver->CurrentTask.CurrentOperation == Driver->CurrentTask.Request->Packet->OperationCount - 1) {
          // last operation, send STOP and signal completion
          FinishTask(Driver);
          return;
        } else {
          // schedule next operation
          Driver->CurrentTask.NumTransferedBytes = 0;
          Driver->CurrentTask.CurrentOperation += 1;
          Driver->CurrentTask.Stage = STAGE_SEND_START_COND;
          return;
        }
      } else {
        DEBUG((
          EFI_D_ERROR,
          "i2c%d error 0x%x during data transfer (read)\n",
          Driver->Driver.Config->No,
          InterruptStatus
        ));
        TaskFailed(Driver, EFI_DEVICE_ERROR);
        return;
      }

      Driver->CurrentTask.NumTransferedBytes += 1;
    } else {
      if (InterruptStatus == SUNXI_I2C_STA_TXD_ACK) {
        // byte transmitted, ACK received
        Driver->CurrentTask.NumTransferedBytes += 1;
        i = Driver->CurrentTask.NumTransferedBytes;

        if (i == Operation->LengthInBytes) {
          // last byte sent
          if (Driver->CurrentTask.CurrentOperation == Driver->CurrentTask.Request->Packet->OperationCount - 1) {
            // last operation, send STOP and signal completion
            FinishTask(Driver);
            return;
          } else {
            // schedule next operation
            Driver->CurrentTask.NumTransferedBytes = 0;
            Driver->CurrentTask.CurrentOperation += 1;
            Driver->CurrentTask.Stage = STAGE_SEND_START_COND;
            return;
          }
        } else {
          // send next byte
          MmioWrite32(Driver->Driver.Config->Base + SUNXI_I2C_DATA, Operation->Buffer[i]);
          MmioAnd32(Driver->Driver.Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);
        }
      } else if (InterruptStatus == SUNXI_I2C_STA_TXD_NAK) {
        DEBUG((
          EFI_D_ERROR,
          "i2c%d got NAK during data transfer, transferred %u out of %u bytes\n",
          Driver->Driver.Config->No,
          i,
          Operation->LengthInBytes
        ));
        TaskFailed(Driver, EFI_DEVICE_ERROR);
        return;
      } else {
        DEBUG((
          EFI_D_ERROR,
          "i2c%d error 0x%x during data transfer (write)\n",
          Driver->Driver.Config->No,
          InterruptStatus
        ));
        TaskFailed(Driver, EFI_DEVICE_ERROR);
        return;
      }
    }

    // Do not advance stage yet
    return;
  default:
    ASSERT(0);
    break;
  }

  Driver->CurrentTask.Stage += 1;
}

VOID
EFIAPI
ExecuteQueuedTasks (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
  I2C_DXE_DRIVER *Driver;
  LIST_ENTRY *Node;
  I2C_REQUEST_BLOCK *Request;
  
  Driver = (I2C_DXE_DRIVER *)Context;

  if (Driver->CurrentTask.Request == NULL) {
    Node = GetFirstNode(&Driver->Queue);
    if (Node == &Driver->Queue)
      return;
    RemoveEntryList(Node);

    Request = REQUEST_BLOCK_FROM_NODE(Node);

    Driver->CurrentTask.Request = Request;
    Driver->CurrentTask.Stage = STAGE_SEND_START_COND;
    Driver->CurrentTask.CurrentOperation = 0;
    Driver->CurrentTask.NumTransferedBytes = 0;
  }

  if (Driver->CurrentTask.Request)
    RunTask(Driver);
}