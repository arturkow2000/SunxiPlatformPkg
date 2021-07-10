/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/I2cBusConfigurationManagement.h>
#include <Protocol/SunxiCcm.h>

#include "../Common/Driver.h"

extern EFI_DRIVER_BINDING_PROTOCOL gSunxiI2cDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL gComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gComponentName2;
extern EFI_I2C_MASTER_PROTOCOL gSunxiI2cMasterProto;
extern EFI_I2C_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL gSunxiI2cBusConfigurationProto;
extern SUNXI_CCM_PROTOCOL *gSunxiCcmProtocol;

#pragma pack(1)
typedef struct _I2C_VENDOR_PATH {
  VENDOR_DEVICE_PATH Vendor;
  UINT8 ControllerNo;
  UINTN ConfigAddr;
} PACKED I2C_VENDOR_PATH;

typedef struct _SUNXI_I2C_PATH {
  I2C_VENDOR_PATH Vendor;
  EFI_DEVICE_PATH End;
} PACKED SUNXI_I2C_PATH;
#pragma pack()

#define DXE_DRIVER_SIGNATURE SIGNATURE_32('i', '2', 'c', ' ')

#define DXE_DRIVER_FROM_COMMON_DRIVER(x) \
  CR(x, I2C_DXE_DRIVER, Driver, DXE_DRIVER_SIGNATURE)

#define DXE_DRIVER_FROM_MASTER(x) \
  CR(x, I2C_DXE_DRIVER, MasterProto, DXE_DRIVER_SIGNATURE)

typedef struct _I2C_REQUEST_BLOCK I2C_REQUEST_BLOCK;
typedef struct _I2C_DXE_DRIVER {
  UINT32 Signature;
  EFI_I2C_MASTER_PROTOCOL MasterProto;
  EFI_I2C_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL BusConfigProto;
  I2C_DRIVER Driver;
  EFI_EVENT TimerEvent;
  LIST_ENTRY Queue;
  struct {
    I2C_REQUEST_BLOCK *Request;
    UINT32 Stage;
    UINT32 CurrentOperation;
    UINT32 NumTransferedBytes;
  } CurrentTask;
} I2C_DXE_DRIVER;

#define REQUEST_BLOCK_FROM_NODE(x) \
  BASE_CR(x, I2C_REQUEST_BLOCK, Link)

struct _I2C_REQUEST_BLOCK {
  LIST_ENTRY Link;
  UINTN SlaveAddress;
  EFI_I2C_REQUEST_PACKET *Packet;
  EFI_EVENT Event;
  EFI_STATUS *OutI2cStatus;
};

VOID
EFIAPI
ExecuteQueuedTasks (
  IN EFI_EVENT Event,
  IN VOID *Context
);
