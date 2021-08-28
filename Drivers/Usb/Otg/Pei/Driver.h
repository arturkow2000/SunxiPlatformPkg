#pragma once

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "../Common/Driver.h"

#include <Library/UsbGadgetLib.h>
#include <Ppi/UsbPpi.h>

#define USB_PPI_INTO_PEI_DRIVER(Record)  \
  BASE_CR(Record, USB_PEI_DRIVER, Ppi)

#define USB_DRIVER_INTO_PEI_DRIVER(Record)  \
  BASE_CR(Record, USB_PEI_DRIVER, Common)

typedef struct _USB_PEI_DRIVER {
  USB_PPI Ppi;
  USB_DRIVER Common;

  USB_GADGET_DRIVER *GadgetDriver;
  VOID *GadgetDriverData;
} USB_PEI_DRIVER;
