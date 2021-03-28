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

#define SUNXI_I2C_ADDR  0x00
#define SUNXI_I2C_XADDR 0x04
#define SUNXI_I2C_DATA 0x08
#define SUNXI_I2C_CTL 0x0c
#define SUNXI_I2C_STA 0x10
#define SUNXI_I2C_CLK 0x14
#define SUNXI_I2C_SWRST 0x18
#define SUNXI_I2C_EFR 0x1c
#define SUNXI_I2C_LCR 0x20
#define SUNXI_I2C_DVFS 0x24

#define SUNXI_I2C_ADDR_GCE_EN (1 << 0)

#define SUNXI_I2C_CTL_ACK (1 << 2)
#define SUNXI_I2C_CTL_INTFLG (1 << 3)
#define SUNXI_I2C_CTL_STOP (1 << 4)
#define SUNXI_I2C_CTL_STA (1 << 5)
#define SUNXI_I2C_CTL_BUSEN (1 << 6)
#define SUNXI_I2C_CTL_INTEN (1 << 7)

#define SUNXI_I2C_SWRST_RESET (1 << 0)

#define SUNXI_I2C_LCR_SDA_EN (1 << 0)
#define SUNXI_I2C_LCR_SDA_CTL (1 << 1)
#define SUNXI_I2C_LCR_SCL_EN (1 << 2)
#define SUNXI_I2C_LCR_SCL_CTL (1 << 3)
#define SUNXI_I2C_LCR_SDA_STA (1 << 4)
#define SUNXI_I2C_LCR_SCL_STA (1 << 5)
#define SUNXI_I2C_LCR_IDLE 0x3a

#define SUNXI_I2C_STA_BUS_ERR 0x00     /* BUS ERROR */
#define SUNXI_I2C_STA_TX_STA 0x08      /* START condition transmitted */
#define SUNXI_I2C_STA_TX_RESTA 0x10    /* Repeated START condition transmitted */
#define SUNXI_I2C_STA_TX_SLAW_ACK 0x18 /* SLA+W transmitted, ACK received */
#define SUNXI_I2C_STA_TX_SLAW_NAK 0x20 /* SLA+W transmitted, ACK not received */
#define SUNXI_I2C_STA_TXD_ACK 0x28     /* data byte transmitted in master mode, ack received */
#define SUNXI_I2C_STA_TXD_NAK 0x30     /* data byte transmitted in master mode, ack not received */
#define SUNXI_I2C_STA_ARBLOST 0x38     /* arbitration lost in address or data byte */
#define SUNXI_I2C_STA_TX_SLAR_ACK 0x40 /* SLA+R transmitted, ACK received */
#define SUNXI_I2C_STA_TX_SLAR_NAK 0x48 /* SLA+R transmitted, ACK not received */
#define SUNXI_I2C_STA_RXD_ACK 0x50     /* data byte received in master mode, ack transmitted */
#define SUNXI_I2C_STA_RXD_NAK 0x58     /* date byte received in master mode, not ack transmitted */
#define SUNXI_I2C_STA_IDLE 0xF8
