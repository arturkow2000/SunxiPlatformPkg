/**
  SPDX-License-Identifier: GPL-2.0+

  Copyright (c) 2021, Artur Kowalski. <arturkow2000@gmail.com>

  Based on SdMmcPciHci driver:
  Copyright (c) 2015, Intel Corporation. All rights reserved.
**/

#include "Driver.h"
#include "../Common/MmcDef.h"

STATIC EMMC_DEVICE_PATH mEmmcDpTemplate = {
  {
    MESSAGING_DEVICE_PATH,
    MSG_EMMC_DP,
    {
      (UINT8)(sizeof(EMMC_DEVICE_PATH)),
      (UINT8)((sizeof(EMMC_DEVICE_PATH)) >> 8),
    },
  },
  0,
};

STATIC SD_DEVICE_PATH mSdDpTemplate = {
  {
    MESSAGING_DEVICE_PATH,
    MSG_SD_DP,
    {
      (UINT8)(sizeof(SD_DEVICE_PATH)),
      (UINT8)((sizeof(SD_DEVICE_PATH)) >> 8),
    },
  },
  0,
};

/**
  Sends SD command to an SD card that is attached to the SD controller.

  The PassThru() function sends the SD command specified by Packet to the SD card
  specified by Slot.

  If Packet is successfully sent to the SD card, then EFI_SUCCESS is returned.

  If a device error occurs while sending the Packet, then EFI_DEVICE_ERROR is returned.

  If Slot is not in a valid range for the SD controller, then EFI_INVALID_PARAMETER
  is returned.

  If Packet defines a data command but both InDataBuffer and OutDataBuffer are NULL,
  EFI_INVALID_PARAMETER is returned.

  @param[in]     This           A pointer to the EFI_SD_MMC_PASS_THRU_PROTOCOL instance.
  @param[in]     Slot           The slot number of the SD card to send the command to.
  @param[in,out] Packet         A pointer to the SD command data structure.
  @param[in]     Event          If Event is NULL, blocking I/O is performed. If Event is
                                not NULL, then nonblocking I/O is performed, and Event
                                will be signaled when the Packet completes.

  @retval EFI_SUCCESS           The SD Command Packet was sent by the host.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to send the SD
                                command Packet.
  @retval EFI_INVALID_PARAMETER Packet, Slot, or the contents of the Packet is invalid.
  @retval EFI_INVALID_PARAMETER Packet defines a data command but both InDataBuffer and
                                OutDataBuffer are NULL.
  @retval EFI_NO_MEDIA          SD Device not present in the Slot.
  @retval EFI_UNSUPPORTED       The command described by the SD Command Packet is not
                                supported by the host controller.
  @retval EFI_BAD_BUFFER_SIZE   The InTransferLength or OutTransferLength exceeds the
                                limit supported by SD card ( i.e. if the number of bytes
                                exceed the Last LBA).

**/
STATIC
EFI_STATUS
EFIAPI
PassThru(
  IN SUNXI_MMC_DRIVER *This,
  IN UINT8 Slot,
  IN OUT EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet,
  IN EFI_EVENT Event OPTIONAL
  )
{
  EFI_STATUS Status;
  BOOLEAN IsInserted;

  UINT32 ResponseType;
  UINT32 Response[4];
  VOID *Data;
  UINT32 Length;
  UINT32 BlockLength;
  BOOLEAN IsWrite;
  UINT32 CommandArgument;

  if (This == NULL || Packet == NULL)
    return EFI_INVALID_PARAMETER;
  
  if (Packet->SdMmcCmdBlk == NULL || Packet->SdMmcStatusBlk == NULL)
    return EFI_INVALID_PARAMETER;
  
  if (Packet->OutDataBuffer == NULL && Packet->OutTransferLength != 0)
    return EFI_INVALID_PARAMETER;
  
  if (Packet->InDataBuffer == NULL && Packet->InTransferLength != 0)
    return EFI_INVALID_PARAMETER;

  Status = SunxiMmcDetectCard(&This->Mmc, &IsInserted, NULL);
  if (EFI_ERROR(Status))
    return Status;

  if (!IsInserted)
    return EFI_NO_MEDIA;

  Data = NULL;
  Length = 0;
  BlockLength = 0;
  IsWrite = FALSE;

  // FIXME: currently all requests are done synchronously
  if (Packet->SdMmcCmdBlk->CommandType == SdMmcCommandTypeBc)
    ResponseType = MMC_RSP_NONE;
  else {
    switch (Packet->SdMmcCmdBlk->ResponseType) {
    case SdMmcResponseTypeR1:
      ResponseType = MMC_RSP_R1;
      break;
    case SdMmcResponseTypeR1b:
      ResponseType = MMC_RSP_R1b;
      break;
    case SdMmcResponseTypeR2:
      ResponseType = MMC_RSP_R2;
      break;
    case SdMmcResponseTypeR3:
      ResponseType = MMC_RSP_R3;
      break;
    case SdMmcResponseTypeR4:
      ResponseType = MMC_RSP_R4;
      break;
    case SdMmcResponseTypeR5:
      ResponseType = MMC_RSP_R5;
      break;
    case SdMmcResponseTypeR6:
      ResponseType = MMC_RSP_R6;
      break;
    case SdMmcResponseTypeR7:
      ResponseType = MMC_RSP_R7;
      break;
    default:
      DEBUG((EFI_D_ERROR, "Unsupported response type %u\n", Packet->SdMmcCmdBlk->ResponseType));
      ASSERT(0);
      return EFI_DEVICE_ERROR;
    }
  }

  if (Packet->OutDataBuffer) {
    DEBUG((EFI_D_ERROR, "Write not supported yet\n"));
    ASSERT(0);
    return EFI_DEVICE_ERROR;
  }

  if (Packet->InDataBuffer) {
    Data = Packet->InDataBuffer;
    Length = Packet->InTransferLength;
    IsWrite = FALSE;
  }

  if (Data) {
    switch (Packet->SdMmcCmdBlk->CommandIndex) {
    case MMC_CMD_READ_SINGLE_BLOCK:
    case MMC_CMD_READ_MULTIPLE_BLOCK:
    case MMC_CMD_WRITE_SINGLE_BLOCK:
    case MMC_CMD_WRITE_MULTIPLE_BLOCK:
    case MMC_CMD_SEND_EXT_CSD:
      BlockLength = 512;
      break;
    default:
      DEBUG((EFI_D_ERROR, "Data transfer not supported for CMD%02d\n", Packet->SdMmcCmdBlk->CommandIndex));
      ASSERT(0);
      return EFI_DEVICE_ERROR;
    }
  }

  CommandArgument = Packet->SdMmcCmdBlk->CommandArgument;

  if (!Data) {
    DEBUG((
      EFI_D_INFO,
      "Executing CMD%02d arg=0x%x type=%u resp=(%u => %u)\n",
      Packet->SdMmcCmdBlk->CommandIndex,
      CommandArgument,
      Packet->SdMmcCmdBlk->CommandType,
      Packet->SdMmcCmdBlk->ResponseType,
      ResponseType
    ));
  } else {
    DEBUG((
      EFI_D_INFO,
      "Executing CMD%02d arg=0x%x type=%u resp=(%u => %u) IsWrite=%d Length=%d BlockLength=%d\n",
      Packet->SdMmcCmdBlk->CommandIndex,
      CommandArgument,
      Packet->SdMmcCmdBlk->CommandType,
      Packet->SdMmcCmdBlk->ResponseType,
      ResponseType,
      IsWrite,
      Length,
      BlockLength
    ));
  }

  Status = SunxiMmcExecuteCommandEx(
    &This->Mmc,
    Packet->SdMmcCmdBlk->CommandIndex,
    CommandArgument,
    ResponseType,
    Response,
    Data,
    IsWrite,
    Length,
    BlockLength
  );

  if (!EFI_ERROR(Status)) {
    Packet->SdMmcStatusBlk->Resp0 = Response[0];
    Packet->SdMmcStatusBlk->Resp1 = Response[1];
    Packet->SdMmcStatusBlk->Resp2 = Response[2];
    Packet->SdMmcStatusBlk->Resp3 = Response[3];
  }

  Packet->TransactionStatus = Status;

  if (Event)
    gBS->SignalEvent(Event);

  return Status;
}

/**
  Used to retrieve next slot numbers supported by the SD controller. The function
  returns information about all available slots (populated or not-populated).

  The GetNextSlot() function retrieves the next slot number on an SD controller.
  If on input Slot is 0xFF, then the slot number of the first slot on the SD controller
  is returned.

  If Slot is a slot number that was returned on a previous call to GetNextSlot(), then
  the slot number of the next slot on the SD controller is returned.

  If Slot is not 0xFF and Slot was not returned on a previous call to GetNextSlot(),
  EFI_INVALID_PARAMETER is returned.

  If Slot is the slot number of the last slot on the SD controller, then EFI_NOT_FOUND
  is returned.

  @param[in]     This           A pointer to the EFI_SD_MMMC_PASS_THRU_PROTOCOL instance.
  @param[in,out] Slot           On input, a pointer to a slot number on the SD controller.
                                On output, a pointer to the next slot number on the SD controller.
                                An input value of 0xFF retrieves the first slot number on the SD
                                controller.

  @retval EFI_SUCCESS           The next slot number on the SD controller was returned in Slot.
  @retval EFI_NOT_FOUND         There are no more slots on this SD controller.
  @retval EFI_INVALID_PARAMETER Slot is not 0xFF and Slot was not returned on a previous call
                                to GetNextSlot().

**/
STATIC
EFI_STATUS
EFIAPI
GetNextSlot(
  IN EFI_SD_MMC_PASS_THRU_PROTOCOL *This,
  IN OUT UINT8 *Slot
  )
{
  if (!Slot)
    return EFI_INVALID_PARAMETER;

  // One slot per controller
  if (*Slot == 0xff)
  {
    *Slot = 0;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
  Used to allocate and build a device path node for an SD card on the SD controller.

  The BuildDevicePath() function allocates and builds a single device node for the SD
  card specified by Slot.

  If the SD card specified by Slot is not present on the SD controller, then EFI_NOT_FOUND
  is returned.

  If DevicePath is NULL, then EFI_INVALID_PARAMETER is returned.

  If there are not enough resources to allocate the device path node, then EFI_OUT_OF_RESOURCES
  is returned.

  Otherwise, DevicePath is allocated with the boot service AllocatePool(), the contents of
  DevicePath are initialized to describe the SD card specified by Slot, and EFI_SUCCESS is
  returned.

  @param[in]     This           A pointer to the EFI_SD_MMMC_PASS_THRU_PROTOCOL instance.
  @param[in]     Slot           Specifies the slot number of the SD card for which a device
                                path node is to be allocated and built.
  @param[out]    DevicePath     A pointer to a single device path node that describes the SD
                                card specified by Slot. This function is responsible for
                                allocating the buffer DevicePath with the boot service
                                AllocatePool(). It is the caller's responsibility to free
                                DevicePath when the caller is finished with DevicePath.

  @retval EFI_SUCCESS           The device path node that describes the SD card specified by
                                Slot was allocated and returned in DevicePath.
  @retval EFI_NOT_FOUND         The SD card specified by Slot does not exist on the SD controller.
  @retval EFI_INVALID_PARAMETER DevicePath is NULL.
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to allocate DevicePath.

**/
STATIC
EFI_STATUS
EFIAPI
BuildDevicePath(
  IN SUNXI_MMC_DRIVER *This,
  IN UINT8 Slot,
  OUT EFI_DEVICE_PATH_PROTOCOL **OutDevicePath
  )
{
  EFI_STATUS Status;
  BOOLEAN IsInserted;
  UINT32 CardType;
  SD_DEVICE_PATH *SdNode;
  EMMC_DEVICE_PATH *EmmcNode;

  if (!OutDevicePath)
    return EFI_INVALID_PARAMETER;
  
  if (Slot != 0)
    return EFI_NOT_FOUND;

  Status = SunxiMmcDetectCard(&This->Mmc, &IsInserted, &CardType);
  if (EFI_ERROR(Status))
    return Status;
  
  if (!IsInserted)
    return EFI_NOT_FOUND;

  if (CardType == CARD_TYPE_EMMC) {
    EmmcNode = AllocateCopyPool(sizeof(EMMC_DEVICE_PATH), &mEmmcDpTemplate);
    if (EmmcNode == NULL)
      return EFI_OUT_OF_RESOURCES;
    *OutDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)EmmcNode;
  } else if (CardType == CARD_TYPE_SD) {
    SdNode = AllocateCopyPool(sizeof(SD_DEVICE_PATH), &mSdDpTemplate);
    if (SdNode == NULL)
      return EFI_OUT_OF_RESOURCES;
    *OutDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)SdNode;
  } else {
    DEBUG((EFI_D_ERROR, "Invalid card type %u\n", CardType));
    ASSERT(0);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  This function retrieves an SD card slot number based on the input device path.

  The GetSlotNumber() function retrieves slot number for the SD card specified by
  the DevicePath node. If DevicePath is NULL, EFI_INVALID_PARAMETER is returned.

  If DevicePath is not a device path node type that the SD Pass Thru driver supports,
  EFI_UNSUPPORTED is returned.

  @param[in]  This              A pointer to the EFI_SD_MMC_PASS_THRU_PROTOCOL instance.
  @param[in]  DevicePath        A pointer to the device path node that describes a SD
                                card on the SD controller.
  @param[out] Slot              On return, points to the slot number of an SD card on
                                the SD controller.

  @retval EFI_SUCCESS           SD card slot number is returned in Slot.
  @retval EFI_INVALID_PARAMETER Slot or DevicePath is NULL.
  @retval EFI_UNSUPPORTED       DevicePath is not a device path node type that the SD
                                Pass Thru driver supports.

**/
STATIC
EFI_STATUS
EFIAPI
GetSlotNumber(
  IN EFI_SD_MMC_PASS_THRU_PROTOCOL *This,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  OUT UINT8 *Slot
  )
{
  ASSERT(0);
  return EFI_UNSUPPORTED;
}

/**
  Resets an SD card that is connected to the SD controller.

  The ResetDevice() function resets the SD card specified by Slot.

  If this SD controller does not support a device reset operation, EFI_UNSUPPORTED is
  returned.

  If Slot is not in a valid slot number for this SD controller, EFI_INVALID_PARAMETER
  is returned.

  If the device reset operation is completed, EFI_SUCCESS is returned.

  @param[in]  This              A pointer to the EFI_SD_MMC_PASS_THRU_PROTOCOL instance.
  @param[in]  Slot              Specifies the slot number of the SD card to be reset.

  @retval EFI_SUCCESS           The SD card specified by Slot was reset.
  @retval EFI_UNSUPPORTED       The SD controller does not support a device reset operation.
  @retval EFI_INVALID_PARAMETER Slot number is invalid.
  @retval EFI_NO_MEDIA          SD Device not present in the Slot.
  @retval EFI_DEVICE_ERROR      The reset command failed due to a device error

**/
STATIC
EFI_STATUS
EFIAPI
ResetDevice(
  IN EFI_SD_MMC_PASS_THRU_PROTOCOL *This,
  IN UINT8 Slot
  )
{
  ASSERT(0);
  return EFI_UNSUPPORTED;
}

EFI_SD_MMC_PASS_THRU_PROTOCOL gSunxiMmcProtocol = {
  .PassThru = (EFI_SD_MMC_PASS_THRU_PASSTHRU)PassThru,
  .GetNextSlot = GetNextSlot,
  .BuildDevicePath = (EFI_SD_MMC_PASS_THRU_BUILD_DEVICE_PATH)BuildDevicePath,
  .GetSlotNumber = GetSlotNumber,
  .ResetDevice = ResetDevice,
};
