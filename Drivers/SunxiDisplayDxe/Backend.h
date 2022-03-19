/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#pragma once

#define SUNXI_DE_BE_MODE                 0x800
#define SUNXI_DE_BE_BACKCOLOR            0x804
#define SUNXI_DE_BE_DISP_SIZE            0x808
#define SUNXI_DE_BE_LAYER0_SIZE          0x810
#define SUNXI_DE_BE_LAYER1_SIZE          0x814
#define SUNXI_DE_BE_LAYER2_SIZE          0x818
#define SUNXI_DE_BE_LAYER3_SIZE          0x81c
#define SUNXI_DE_BE_LAYER0_POS           0x820
#define SUNXI_DE_BE_LAYER1_POS           0x824
#define SUNXI_DE_BE_LAYER2_POS           0x828
#define SUNXI_DE_BE_LAYER3_POS           0x82c
#define SUNXI_DE_BE_LAYER0_STRIDE        0x840
#define SUNXI_DE_BE_LAYER1_STRIDE        0x844
#define SUNXI_DE_BE_LAYER2_STRIDE        0x848
#define SUNXI_DE_BE_LAYER3_STRIDE        0x84c
#define SUNXI_DE_BE_LAYER0_ADDR_LOW32B   0x850
#define SUNXI_DE_BE_LAYER1_ADDR_LOW32B   0x854
#define SUNXI_DE_BE_LAYER2_ADDR_LOW32B   0x858
#define SUNXI_DE_BE_LAYER3_ADDR_LOW32B   0x85c
#define SUNXI_DE_BE_LAYER0_ADDR_HIGH4B   0x860
#define SUNXI_DE_BE_LAYER1_ADDR_HIGH4B   0x864
#define SUNXI_DE_BE_LAYER2_ADDR_HIGH4B   0x868
#define SUNXI_DE_BE_LAYER3_ADDR_HIGH4B   0x86c
#define SUNXI_DE_BE_REG_CTRL             0x870
#define SUNXI_DE_BE_COLOR_KEY_MAX        0x880
#define SUNXI_DE_BE_COLOR_KEY_MIN        0x884
#define SUNXI_DE_BE_COLOR_KEY_CONFIG     0x888
#define SUNXI_DE_BE_LAYER0_ATTR0_CTRL    0x890
#define SUNXI_DE_BE_LAYER1_ATTR0_CTRL    0x894
#define SUNXI_DE_BE_LAYER2_ATTR0_CTRL    0x898
#define SUNXI_DE_BE_LAYER3_ATTR0_CTRL    0x89c
#define SUNXI_DE_BE_LAYER0_ATTR1_CTRL    0x8a0
#define SUNXI_DE_BE_LAYER1_ATTR1_CTRL    0x8a4
#define SUNXI_DE_BE_LAYER2_ATTR1_CTRL    0x8a8
#define SUNXI_DE_BE_LAYER3_ATTR1_CTRL    0x8ac
#define SUNXI_DE_BE_OUTPUT_COLOR_CTRL    0x9c0

#define SUNXI_DE_BE_WIDTH(x)                    (((x) - 1) << 0)
#define SUNXI_DE_BE_HEIGHT(y)                   (((y) - 1) << 16)
#define SUNXI_DE_BE_MODE_ENABLE                 (1 << 0)
#define SUNXI_DE_BE_MODE_START                  (1 << 1)
#define SUNXI_DE_BE_MODE_DEFLICKER_ENABLE       (1 << 4)
#define SUNXI_DE_BE_MODE_LAYER0_ENABLE          (1 << 8)
#define SUNXI_DE_BE_MODE_INTERLACE_ENABLE       (1 << 28)
#define SUNXI_DE_BE_LAYER_STRIDE(x)             ((x) << 5)
#define SUNXI_DE_BE_REG_CTRL_LOAD_REGS          (1 << 0)
#define SUNXI_DE_BE_LAYER_ATTR0_SRC_FE0         0x00000002
#define SUNXI_DE_BE_LAYER_ATTR1_FMT_XRGB8888    (0x09 << 8)
#define SUNXI_DE_BE_OUTPUT_COLOR_CTRL_ENABLE    1
