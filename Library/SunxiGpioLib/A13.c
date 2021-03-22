/*
 * SPDX-License-Identifier: GPL-2.0+
 *
 * Copyright (C) 2021, Artur Kowalski. <arturkow2000@gmail.com>
 * 
 * Based on Linux driver
 * 
 * Copyright (C) 2014-2016 Maxime Ripard <maxime.ripard@free-electrons.com>
 * Copyright (C) 2016 Mylene Josserand <mylene.josserand@free-electrons.com>
 * 
 */
#include "SunxiGpioLib.h"

STATIC CONST SUNXI_GPIO_MUX mMuxI2c0[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"i2c0"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxPB002[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"pwm"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxIr0i17[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"ir0"),
    GPIO_MUX_IRQ(6, 17),
};

STATIC CONST SUNXI_GPIO_MUX mMuxIr0i18[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"ir0"),
    GPIO_MUX_IRQ(6, 18),
};

STATIC CONST SUNXI_GPIO_MUX mMuxSpi2i24[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"spi2"),
    GPIO_MUX_IRQ(6, 24),
};

STATIC CONST SUNXI_GPIO_MUX mMuxI2c1[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"i2c1"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxI2c2[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"i2c2"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxNand0Spi0[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"nand0"),
    GPIO_MUX(3, L"spi0"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxNand0[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"nand0"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxNand0Mmc2[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"nand0"),
    GPIO_MUX(3, L"mmc2"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxNand0Uart2Uart3[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"nand0"),
    GPIO_MUX(3, L"uart2"),
    GPIO_MUX(4, L"uart3"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxLcd0Uart2[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"lcd0"),
    GPIO_MUX(3, L"uart2"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxLcd0Emac[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"lcd0"),
    GPIO_MUX(3, L"emac"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxTs0Csi0Spi2i14NoGpioOut[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(2, L"ts0"),
    GPIO_MUX(3, L"csi0"),
    GPIO_MUX(4, L"spi2"),
    GPIO_MUX_IRQ(6, 14),
};

STATIC CONST SUNXI_GPIO_MUX mMuxTs0Csi0Spi2i15NoGpioOut[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(2, L"ts0"),
    GPIO_MUX(3, L"csi0"),
    GPIO_MUX(4, L"spi2"),
    GPIO_MUX_IRQ(6, 15),
};

STATIC CONST SUNXI_GPIO_MUX mMuxTs0Csi0Spi2NoGpioOut[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(2, L"ts0"),
    GPIO_MUX(3, L"csi0"),
    GPIO_MUX(4, L"spi2"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxTs0Csi0Spi2[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"ts0"),
    GPIO_MUX(3, L"csi0"),
    GPIO_MUX(4, L"spi2"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxTs0Csi0Mmc2[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"ts0"),
    GPIO_MUX(3, L"csi0"),
    GPIO_MUX(4, L"mmc2"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxTs0Csi0Uart1[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"ts0"),
    GPIO_MUX(3, L"csi0"),
    GPIO_MUX(4, L"uart1"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxMmc0Jtag[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"mmc0"),
    GPIO_MUX(4, L"jtag"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxMmc0Uart0[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"mmc0"),
    GPIO_MUX(4, L"uart0"),
};

STATIC CONST SUNXI_GPIO_MUX mMuxGpsi0NoGpioOut[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(2, L"gps"),
    GPIO_MUX_IRQ(6, 0),
};

STATIC CONST SUNXI_GPIO_MUX mMuxGpsi1NoGpioOut[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(2, L"gps"),
    GPIO_MUX_IRQ(6, 1),
};

STATIC CONST SUNXI_GPIO_MUX mMuxGpsi2NoGpioOut[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(2, L"gps"),
    GPIO_MUX_IRQ(6, 2),
};

STATIC CONST SUNXI_GPIO_MUX mMuxMmc1Uart1i3[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"mmc1"),
    GPIO_MUX(4, L"uart1"),
    GPIO_MUX_IRQ(6, 3),
};

STATIC CONST SUNXI_GPIO_MUX mMuxMmc1Uart1i4[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"mmc1"),
    GPIO_MUX(4, L"uart1"),
    GPIO_MUX_IRQ(6, 4),
};

STATIC CONST SUNXI_GPIO_MUX mMuxSpi1Uart3i9[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"spi1"),
    GPIO_MUX(3, L"uart3"),
    GPIO_MUX_IRQ(6, 9),
};

STATIC CONST SUNXI_GPIO_MUX mMuxSpi1Uart3i10[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"spi1"),
    GPIO_MUX(3, L"uart3"),
    GPIO_MUX_IRQ(6, 10),
};

STATIC CONST SUNXI_GPIO_MUX mMuxSpi1Uart3i11[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"spi1"),
    GPIO_MUX(3, L"uart3"),
    GPIO_MUX_IRQ(6, 11),
};

STATIC CONST SUNXI_GPIO_MUX mMuxSpi1Uart3i12[] = {
    GPIO_MUX(0, L"gpio_in"),
    GPIO_MUX(1, L"gpio_out"),
    GPIO_MUX(2, L"spi1"),
    GPIO_MUX(3, L"uart3"),
    GPIO_MUX_IRQ(6, 12),
};

CONST SUNXI_GPIO_PIN_INTERNAL gSunxiGpioPinList[] = {
    _GPIO_PIN_NONEXISTENT,                 /* PA00 */
    _GPIO_PIN_NONEXISTENT,                 /* PA01 */
    _GPIO_PIN_NONEXISTENT,                 /* PA02 */
    _GPIO_PIN_NONEXISTENT,                 /* PA03 */
    _GPIO_PIN_NONEXISTENT,                 /* PA04 */
    _GPIO_PIN_NONEXISTENT,                 /* PA05 */
    _GPIO_PIN_NONEXISTENT,                 /* PA06 */
    _GPIO_PIN_NONEXISTENT,                 /* PA07 */
    _GPIO_PIN_NONEXISTENT,                 /* PA08 */
    _GPIO_PIN_NONEXISTENT,                 /* PA09 */
    _GPIO_PIN_NONEXISTENT,                 /* PA10 */
    _GPIO_PIN_NONEXISTENT,                 /* PA11 */
    _GPIO_PIN_NONEXISTENT,                 /* PA12 */
    _GPIO_PIN_NONEXISTENT,                 /* PA13 */
    _GPIO_PIN_NONEXISTENT,                 /* PA14 */
    _GPIO_PIN_NONEXISTENT,                 /* PA15 */
    _GPIO_PIN_NONEXISTENT,                 /* PA16 */
    _GPIO_PIN_NONEXISTENT,                 /* PA17 */
    _GPIO_PIN_NONEXISTENT,                 /* PA18 */
    _GPIO_PIN_NONEXISTENT,                 /* PA19 */
    _GPIO_PIN_NONEXISTENT,                 /* PA20 */
    _GPIO_PIN_NONEXISTENT,                 /* PA21 */
    _GPIO_PIN_NONEXISTENT,                 /* PA22 */
    _GPIO_PIN_NONEXISTENT,                 /* PA23 */
    _GPIO_PIN_NONEXISTENT,                 /* PA24 */
    _GPIO_PIN_NONEXISTENT,                 /* PA25 */
    _GPIO_PIN_NONEXISTENT,                 /* PA26 */
    _GPIO_PIN_NONEXISTENT,                 /* PA27 */
    _GPIO_PIN_NONEXISTENT,                 /* PA28 */
    _GPIO_PIN_NONEXISTENT,                 /* PA29 */
    _GPIO_PIN_NONEXISTENT,                 /* PA30 */
    _GPIO_PIN_NONEXISTENT,                 /* PA31 */
    GPIO_PIN(mMuxI2c0),                    /* PB00 */
    GPIO_PIN(mMuxI2c0),                    /* PB01 */
    GPIO_PIN(mMuxPB002),                   /* PB02 */
    GPIO_PIN(mMuxIr0i17),                  /* PB03 */
    GPIO_PIN(mMuxIr0i18),                  /* PB04 */
    _GPIO_PIN_NONEXISTENT,                 /* PB05 */
    _GPIO_PIN_NONEXISTENT,                 /* PB06 */
    _GPIO_PIN_NONEXISTENT,                 /* PB07 */
    _GPIO_PIN_NONEXISTENT,                 /* PB08 */
    _GPIO_PIN_NONEXISTENT,                 /* PB09 */
    GPIO_PIN(mMuxSpi2i24),                 /* PB10 */
    _GPIO_PIN_NONEXISTENT,                 /* PB11 */
    _GPIO_PIN_NONEXISTENT,                 /* PB12 */
    _GPIO_PIN_NONEXISTENT,                 /* PB13 */
    _GPIO_PIN_NONEXISTENT,                 /* PB14 */
    GPIO_PIN(mMuxI2c1),                    /* PB15 */
    GPIO_PIN(mMuxI2c1),                    /* PB16 */
    GPIO_PIN(mMuxI2c2),                    /* PB17 */
    GPIO_PIN(mMuxI2c2),                    /* PB18 */
    _GPIO_PIN_NONEXISTENT,                 /* PB19 */
    _GPIO_PIN_NONEXISTENT,                 /* PB20 */
    _GPIO_PIN_NONEXISTENT,                 /* PB21 */
    _GPIO_PIN_NONEXISTENT,                 /* PB22 */
    _GPIO_PIN_NONEXISTENT,                 /* PB23 */
    _GPIO_PIN_NONEXISTENT,                 /* PB24 */
    _GPIO_PIN_NONEXISTENT,                 /* PB25 */
    _GPIO_PIN_NONEXISTENT,                 /* PB26 */
    _GPIO_PIN_NONEXISTENT,                 /* PB27 */
    _GPIO_PIN_NONEXISTENT,                 /* PB28 */
    _GPIO_PIN_NONEXISTENT,                 /* PB29 */
    _GPIO_PIN_NONEXISTENT,                 /* PB30 */
    _GPIO_PIN_NONEXISTENT,                 /* PB31 */
    GPIO_PIN(mMuxNand0Spi0),               /* PC00 */
    GPIO_PIN(mMuxNand0Spi0),               /* PC01 */
    GPIO_PIN(mMuxNand0Spi0),               /* PC02 */
    GPIO_PIN(mMuxNand0Spi0),               /* PC03 */
    GPIO_PIN(mMuxNand0),                   /* PC04 */
    GPIO_PIN(mMuxNand0),                   /* PC05 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC06 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC07 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC08 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC09 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC10 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC11 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC12 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC13 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC14 */
    GPIO_PIN(mMuxNand0Mmc2),               /* PC15 */
    _GPIO_PIN_NONEXISTENT,                 /* PC16 */
    _GPIO_PIN_NONEXISTENT,                 /* PC17 */
    _GPIO_PIN_NONEXISTENT,                 /* PC18 */
    GPIO_PIN(mMuxNand0Uart2Uart3),         /* PC19 */
    _GPIO_PIN_NONEXISTENT,                 /* PC20 */
    _GPIO_PIN_NONEXISTENT,                 /* PC21 */
    _GPIO_PIN_NONEXISTENT,                 /* PC22 */
    _GPIO_PIN_NONEXISTENT,                 /* PC23 */
    _GPIO_PIN_NONEXISTENT,                 /* PC24 */
    _GPIO_PIN_NONEXISTENT,                 /* PC25 */
    _GPIO_PIN_NONEXISTENT,                 /* PC26 */
    _GPIO_PIN_NONEXISTENT,                 /* PC27 */
    _GPIO_PIN_NONEXISTENT,                 /* PC28 */
    _GPIO_PIN_NONEXISTENT,                 /* PC29 */
    _GPIO_PIN_NONEXISTENT,                 /* PC30 */
    _GPIO_PIN_NONEXISTENT,                 /* PC31 */
    _GPIO_PIN_NONEXISTENT,                 /* PD00 */
    _GPIO_PIN_NONEXISTENT,                 /* PD01 */
    GPIO_PIN(mMuxLcd0Uart2),               /* PD02 */
    GPIO_PIN(mMuxLcd0Uart2),               /* PD03 */
    GPIO_PIN(mMuxLcd0Uart2),               /* PD04 */
    GPIO_PIN(mMuxLcd0Uart2),               /* PD05 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD06 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD07 */
    _GPIO_PIN_NONEXISTENT,                 /* PD08 */
    _GPIO_PIN_NONEXISTENT,                 /* PD09 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD10 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD11 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD12 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD13 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD14 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD15 */
    _GPIO_PIN_NONEXISTENT,                 /* PD16 */
    _GPIO_PIN_NONEXISTENT,                 /* PD17 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD18 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD19 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD20 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD21 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD22 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD23 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD24 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD25 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD26 */
    GPIO_PIN(mMuxLcd0Emac),                /* PD27 */
    _GPIO_PIN_NONEXISTENT,                 /* PD28 */
    _GPIO_PIN_NONEXISTENT,                 /* PD29 */
    _GPIO_PIN_NONEXISTENT,                 /* PD30 */
    _GPIO_PIN_NONEXISTENT,                 /* PD31 */
    GPIO_PIN(mMuxTs0Csi0Spi2i14NoGpioOut), /* PE00 */
    GPIO_PIN(mMuxTs0Csi0Spi2i15NoGpioOut), /* PE01 */
    GPIO_PIN(mMuxTs0Csi0Spi2NoGpioOut),    /* PE02 */
    GPIO_PIN(mMuxTs0Csi0Spi2),             /* PE03 */
    GPIO_PIN(mMuxTs0Csi0Mmc2),             /* PE04 */
    GPIO_PIN(mMuxTs0Csi0Mmc2),             /* PE05 */
    GPIO_PIN(mMuxTs0Csi0Mmc2),             /* PE06 */
    GPIO_PIN(mMuxTs0Csi0Mmc2),             /* PE07 */
    GPIO_PIN(mMuxTs0Csi0Mmc2),             /* PE08 */
    GPIO_PIN(mMuxTs0Csi0Mmc2),             /* PE09 */
    GPIO_PIN(mMuxTs0Csi0Uart1),            /* PE10 */
    GPIO_PIN(mMuxTs0Csi0Uart1),            /* PE11 */
    _GPIO_PIN_NONEXISTENT,                 /* PE12 */
    _GPIO_PIN_NONEXISTENT,                 /* PE13 */
    _GPIO_PIN_NONEXISTENT,                 /* PE14 */
    _GPIO_PIN_NONEXISTENT,                 /* PE15 */
    _GPIO_PIN_NONEXISTENT,                 /* PE16 */
    _GPIO_PIN_NONEXISTENT,                 /* PE17 */
    _GPIO_PIN_NONEXISTENT,                 /* PE18 */
    _GPIO_PIN_NONEXISTENT,                 /* PE19 */
    _GPIO_PIN_NONEXISTENT,                 /* PE20 */
    _GPIO_PIN_NONEXISTENT,                 /* PE21 */
    _GPIO_PIN_NONEXISTENT,                 /* PE22 */
    _GPIO_PIN_NONEXISTENT,                 /* PE23 */
    _GPIO_PIN_NONEXISTENT,                 /* PE24 */
    _GPIO_PIN_NONEXISTENT,                 /* PE25 */
    _GPIO_PIN_NONEXISTENT,                 /* PE26 */
    _GPIO_PIN_NONEXISTENT,                 /* PE27 */
    _GPIO_PIN_NONEXISTENT,                 /* PE28 */
    _GPIO_PIN_NONEXISTENT,                 /* PE29 */
    _GPIO_PIN_NONEXISTENT,                 /* PE30 */
    _GPIO_PIN_NONEXISTENT,                 /* PE31 */
    GPIO_PIN(mMuxMmc0Jtag),                /* PF00 */
    GPIO_PIN(mMuxMmc0Jtag),                /* PF01 */
    GPIO_PIN(mMuxMmc0Uart0),               /* PF02 */
    GPIO_PIN(mMuxMmc0Jtag),                /* PF03 */
    GPIO_PIN(mMuxMmc0Uart0),               /* PF04 */
    GPIO_PIN(mMuxMmc0Jtag),                /* PF05 */
    _GPIO_PIN_NONEXISTENT,                 /* PF06 */
    _GPIO_PIN_NONEXISTENT,                 /* PF07 */
    _GPIO_PIN_NONEXISTENT,                 /* PF08 */
    _GPIO_PIN_NONEXISTENT,                 /* PF09 */
    _GPIO_PIN_NONEXISTENT,                 /* PF10 */
    _GPIO_PIN_NONEXISTENT,                 /* PF11 */
    _GPIO_PIN_NONEXISTENT,                 /* PF12 */
    _GPIO_PIN_NONEXISTENT,                 /* PF13 */
    _GPIO_PIN_NONEXISTENT,                 /* PF14 */
    _GPIO_PIN_NONEXISTENT,                 /* PF15 */
    _GPIO_PIN_NONEXISTENT,                 /* PF16 */
    _GPIO_PIN_NONEXISTENT,                 /* PF17 */
    _GPIO_PIN_NONEXISTENT,                 /* PF18 */
    _GPIO_PIN_NONEXISTENT,                 /* PF19 */
    _GPIO_PIN_NONEXISTENT,                 /* PF20 */
    _GPIO_PIN_NONEXISTENT,                 /* PF21 */
    _GPIO_PIN_NONEXISTENT,                 /* PF22 */
    _GPIO_PIN_NONEXISTENT,                 /* PF23 */
    _GPIO_PIN_NONEXISTENT,                 /* PF24 */
    _GPIO_PIN_NONEXISTENT,                 /* PF25 */
    _GPIO_PIN_NONEXISTENT,                 /* PF26 */
    _GPIO_PIN_NONEXISTENT,                 /* PF27 */
    _GPIO_PIN_NONEXISTENT,                 /* PF28 */
    _GPIO_PIN_NONEXISTENT,                 /* PF29 */
    _GPIO_PIN_NONEXISTENT,                 /* PF30 */
    _GPIO_PIN_NONEXISTENT,                 /* PF31 */
    GPIO_PIN(mMuxGpsi0NoGpioOut),          /* PG00 */
    GPIO_PIN(mMuxGpsi1NoGpioOut),          /* PG01 */
    GPIO_PIN(mMuxGpsi2NoGpioOut),          /* PG02 */
    GPIO_PIN(mMuxMmc1Uart1i3),             /* PG03 */
    GPIO_PIN(mMuxMmc1Uart1i4),             /* PG04 */
    _GPIO_PIN_NONEXISTENT,                 /* PG05 */
    _GPIO_PIN_NONEXISTENT,                 /* PG06 */
    _GPIO_PIN_NONEXISTENT,                 /* PG07 */
    _GPIO_PIN_NONEXISTENT,                 /* PG08 */
    GPIO_PIN(mMuxSpi1Uart3i9),             /* PG09 */
    GPIO_PIN(mMuxSpi1Uart3i10),            /* PG10 */
    GPIO_PIN(mMuxSpi1Uart3i11),            /* PG11 */
    GPIO_PIN(mMuxSpi1Uart3i12),            /* PG12 */
};

CONST UINTN gSunxiGpioPinListLength = ARRAY_SIZE(gSunxiGpioPinList);
