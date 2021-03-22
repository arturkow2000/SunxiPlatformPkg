/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC register definition for allwinner sunxi platform.
 */

#pragma once

#define SUNXI_MMC_GCTRL     0x000
#define SUNXI_MMC_CLKCR     0x004
#define SUNXI_MMC_TIMEOUT   0x008
#define SUNXI_MMC_WIDTH     0x00c
#define SUNXI_MMC_BLKSZ     0x010
#define SUNXI_MMC_BYTECNT   0x014
#define SUNXI_MMC_CMD       0x018
#define SUNXI_MMC_ARG       0x01c
#define SUNXI_MMC_RESP0     0x020
#define SUNXI_MMC_RESP1     0x024
#define SUNXI_MMC_RESP2     0x028
#define SUNXI_MMC_RESP3     0x02c
#define SUNXI_MMC_IMASK     0x030
#define SUNXI_MMC_MINT      0x034
#define SUNXI_MMC_RINT      0x038
#define SUNXI_MMC_STATUS    0x03c
#define SUNXI_MMC_FTRGLEVEL 0x040
#define SUNXI_MMC_FUNCSEL   0x044
#define SUNXI_MMC_CBCR      0x048
#define SUNXI_MMC_BBCR      0x04c
#define SUNXI_MMC_DBGC      0x050
#define SUNXI_MMC_A12A      0x058
#define SUNXI_MMC_NTSR      0x05c
#define SUNXI_MMC_DMAC      0x080
#define SUNXI_MMC_DLBA      0x084
#define SUNXI_MMC_IDST      0x088
#define SUNXI_MMC_IDIE      0x08c
#define SUNXI_MMC_CHDA      0x090
#define SUNXI_MMC_CBDA      0x094

#define SUNXI_MMC_GCTRL_SOFT_RESET (1 << 0)
#define SUNXI_MMC_GCTRL_FIFO_RESET (1 << 1)
#define SUNXI_MMC_GCTRL_DMA_RESET (1 << 2)
#define SUNXI_MMC_GCTRL_DMA_ENABLE (1 << 5)
#define SUNXI_MMC_GCTRL_ACCESS_BY_AHB (1 << 31)

#define SUNXI_MMC_CLKCR_DIV_MASK (0xff)
#define SUNXI_MMC_CLKCR_EN (1 << 16)
#define SUNXI_MMC_CLKCR_PWRSAVE (1 << 17)

#define SUNXI_MMC_CMD_RESP_EXPIRE (1 << 6)
#define SUNXI_MMC_CMD_LONG_RESPONSE (1 << 7)
#define SUNXI_MMC_CMD_CHECK_RESPONSE_CRC (1 << 8)
#define SUNXI_MMC_CMD_DATA_EXPIRE (1 << 9)
#define SUNXI_MMC_CMD_WRITE (1 << 10)
#define SUNXI_MMC_CMD_AUTO_STOP (1 << 12)
#define SUNXI_MMC_CMD_WAIT_PRE_OVER (1 << 13)
#define SUNXI_MMC_CMD_SEND_INIT_SEQ (1 << 15)
#define SUNXI_MMC_CMD_UPCLK_ONLY (1 << 21)
#define SUNXI_MMC_CMD_START (1 << 31)

#define SUNXI_MMC_RINT_RESP_ERROR (1 << 1)
#define SUNXI_MMC_RINT_COMMAND_DONE (1 << 2)
#define SUNXI_MMC_RINT_DATA_OVER (1 << 3)
#define SUNXI_MMC_RINT_TX_DATA_REQUEST (1 << 4)
#define SUNXI_MMC_RINT_RX_DATA_REQUEST (1 << 5)
#define SUNXI_MMC_RINT_RESP_CRC_ERROR (1 << 6)
#define SUNXI_MMC_RINT_DATA_CRC_ERROR (1 << 7)
#define SUNXI_MMC_RINT_RESP_TIMEOUT (1 << 8)
#define SUNXI_MMC_RINT_DATA_TIMEOUT (1 << 9)
#define SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE (1 << 10)
#define SUNXI_MMC_RINT_FIFO_RUN_ERROR (1 << 11)
#define SUNXI_MMC_RINT_HARD_WARE_LOCKED (1 << 12)
#define SUNXI_MMC_RINT_START_BIT_ERROR (1 << 13)
#define SUNXI_MMC_RINT_AUTO_COMMAND_DONE (1 << 14)
#define SUNXI_MMC_RINT_END_BIT_ERROR (1 << 15)
#define SUNXI_MMC_RINT_SDIO_INTERRUPT (1 << 16)
#define SUNXI_MMC_RINT_CARD_INSERT (1 << 30)
#define SUNXI_MMC_RINT_CARD_REMOVE (1 << 31)

#define SUNXI_MMC_STATUS_RXWL (1 << 0)
#define SUNXI_MMC_STATUS_TXWL (1 << 1)
#define SUNXI_MMC_STATUS_FIFO_EMPTY (1 << 2)
#define SUNXI_MMC_STATUS_FIFO_FULL (1 << 3)
#define SUNXI_MMC_STATUS_CARD_PRESENT (1 << 8)
#define SUNXI_MMC_STATUS_CARD_DATA_BUSY (1 << 9)
#define SUNXI_MMC_STATUS_DATA_FSM_BUSY (1 << 10)
