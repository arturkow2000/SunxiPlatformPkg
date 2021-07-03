#include "Driver.h"

EFI_STATUS AxpRead8(
  IN AXP209_DRIVER *CommonDriver,
  IN UINT8 Reg,
  OUT UINT8 *OutData
  )
{
  AXP209_PACKET Packet;
  AXP209_DXE_DRIVER *Driver;

  ASSERT(CommonDriver != NULL);
  if (CommonDriver == NULL)
    return EFI_INVALID_PARAMETER;

  Driver = AXP209_COMMON_DRIVER_TO_DXE_DRIVER(CommonDriver);

  Packet.OperationCount = 2;
  Packet.Operation[0].Buffer = &Reg;
  Packet.Operation[0].Flags = 0;
  Packet.Operation[0].LengthInBytes = 1;

  Packet.Operation[1].Buffer = OutData;
  Packet.Operation[1].Flags = I2C_FLAG_READ;
  Packet.Operation[1].LengthInBytes = 1;

  return Driver->I2cIo->QueueRequest(Driver->I2cIo, 0, NULL, (EFI_I2C_REQUEST_PACKET*)&Packet, NULL);
}

EFI_STATUS AxpWrite8(
  IN AXP209_DRIVER *CommonDriver,
  IN UINT8 Reg,
  IN UINT8 Data
  )
{
  AXP209_PACKET Packet;
  AXP209_DXE_DRIVER *Driver;
  UINT8 Buffer[2];

  ASSERT(CommonDriver != NULL);
  if (CommonDriver == NULL)
    return EFI_INVALID_PARAMETER;

  Driver = AXP209_COMMON_DRIVER_TO_DXE_DRIVER(CommonDriver);

  Buffer[0] = Reg;
  Buffer[1] = Data;

  Packet.OperationCount = 1;
  Packet.Operation[0].Buffer = Buffer;
  Packet.Operation[0].Flags = 0;
  Packet.Operation[0].LengthInBytes = 2;

  return Driver->I2cIo->QueueRequest(Driver->I2cIo, 0, NULL, (EFI_I2C_REQUEST_PACKET*)&Packet, NULL);
}
