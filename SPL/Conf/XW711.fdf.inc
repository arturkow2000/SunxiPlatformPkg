#
#  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
#  Copyright (c) 2014, Linaro Limited. All rights reserved.
#  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.
#  Copyright (c) 2021, Artur Kowalski.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

[FD.SUNXI_SPL]
BaseAddress   = 0x0000|gSunxiSplTokenSpaceGuid.PcdSplBaseAddress
Size          = 0x8000|gSunxiSplTokenSpaceGuid.PcdSplSize
ErasePolarity = 1

# This one is tricky, it must be: BlockSize * NumBlocks = Size
BlockSize     = 0x00001000
NumBlocks     = 8

#
# eGON header in U-Boot format
#
0x00000000|0x00000060
DATA = {
  # b 0x60
  0x16, 0x00, 0x00, 0xea,
  # Header signature
  # eGON.BT0
  0x65, 0x47, 0x4F, 0x4E, 0x2E, 0x42, 0x54, 0x30,
  # FIXME: checksum should be generated during SPL build
  # currently external tools must be used to patch header
  0x00, 0x00, 0x00, 0x00,
  # SPL size, this must match FD.SUNXI_SPL.Size
  0x00, 0x80, 0x00, 0x00,
  # U-Boot SPL signature
  0x53, 0x50, 0x4c, 0x02
}

0x00000060|0x00000fa0
DATA = {
  # this is required because EDK inserts BL as first instruction
  # which overrides FEL return address
  # mov r0, lr
  0x0e, 0x00, 0xa0, 0xe1,
  # b 0x1000
  0xe5, 0x03, 0x00, 0xea
}

0x00001000|0x00006000
FV = SPL

0x00007000|0x00001000
gSunxiSplTokenSpaceGuid.PcdStackBase|gSunxiSplTokenSpaceGuid.PcdStackSize

[FV.SPL]
FvAlignment        = 16
ERASE_POLARITY     = 1
MEMORY_MAPPED      = TRUE
STICKY_WRITE       = TRUE
LOCK_CAP           = TRUE
LOCK_STATUS        = TRUE
WRITE_DISABLED_CAP = TRUE
WRITE_ENABLED_CAP  = TRUE
WRITE_STATUS       = TRUE
WRITE_LOCK_CAP     = TRUE
WRITE_LOCK_STATUS  = TRUE
READ_DISABLED_CAP  = TRUE
READ_ENABLED_CAP   = TRUE
READ_STATUS        = TRUE
READ_LOCK_CAP      = TRUE
READ_LOCK_STATUS   = TRUE
  INF SunxiPlatformPkg/SPL/SplMain/SplMainXW711.inf
