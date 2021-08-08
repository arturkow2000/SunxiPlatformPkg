#include "Driver.h"

#define USBC_REG_o_ISCR     0x0400
#define USBC_REG_o_PHYCTL   0x0404
#define USBC_REG_o_PHYBIST  0x0408
#define USBC_REG_o_PHYTUNE  0x040c

/* Interface Status and Control */
#define  USBC_BP_ISCR_VBUS_VALID_FROM_DATA  30
#define  USBC_BP_ISCR_VBUS_VALID_FROM_VBUS  29
#define  USBC_BP_ISCR_EXT_ID_STATUS         28
#define  USBC_BP_ISCR_EXT_DM_STATUS         27
#define  USBC_BP_ISCR_EXT_DP_STATUS         26
#define  USBC_BP_ISCR_MERGED_VBUS_STATUS    25
#define  USBC_BP_ISCR_MERGED_ID_STATUS      24

#define  USBC_BP_ISCR_ID_PULLUP_EN		17
#define  USBC_BP_ISCR_DPDM_PULLUP_EN		16
#define  USBC_BP_ISCR_FORCE_ID			14
#define  USBC_BP_ISCR_FORCE_VBUS_VALID		12
#define  USBC_BP_ISCR_VBUS_VALID_SRC		10

#define  USBC_BP_ISCR_HOSC_EN			7
#define  USBC_BP_ISCR_VBUS_CHANGE_DETECT	6
#define  USBC_BP_ISCR_ID_CHANGE_DETECT		5
#define  USBC_BP_ISCR_DPDM_CHANGE_DETECT	4
#define  USBC_BP_ISCR_IRQ_ENABLE		3
#define  USBC_BP_ISCR_VBUS_CHANGE_DETECT_EN	2
#define  USBC_BP_ISCR_ID_CHANGE_DETECT_EN	1
#define  USBC_BP_ISCR_DPDM_CHANGE_DETECT_EN	0

STATIC inline UINT32 USBC_WakeUp_ClearChangeDetect(UINT32 Reg)
{
  Reg &= ~(1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT);
  Reg &= ~(1 << USBC_BP_ISCR_ID_CHANGE_DETECT);
  Reg &= ~(1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT);

  return Reg;
}

VOID USBC_EnableIdPullUp(USB_DRIVER *Driver)
{
  UINT32 Reg;

  Reg = MmioRead32(Driver->Base + USBC_REG_o_ISCR);
  Reg |= 1 << USBC_BP_ISCR_ID_PULLUP_EN;
  MmioWrite32(Driver->Base + USBC_REG_o_ISCR, USBC_WakeUp_ClearChangeDetect(Reg));
}

VOID USBC_EnableDpDmPullUp(USB_DRIVER *Driver)
{
  UINT32 Reg;

  Reg = MmioRead32(Driver->Base + USBC_REG_o_ISCR);
  Reg |= 1 << USBC_BP_ISCR_DPDM_PULLUP_EN;
  MmioWrite32(Driver->Base + USBC_REG_o_ISCR, USBC_WakeUp_ClearChangeDetect(Reg));
}

VOID USBC_ForceIdToLow(USB_DRIVER *Driver)
{
  UINT32 Reg;

  Reg = MmioRead32(Driver->Base + USBC_REG_o_ISCR);
  Reg &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
  Reg |= (0x02 << USBC_BP_ISCR_FORCE_ID);
  MmioWrite32(Driver->Base + USBC_REG_o_ISCR, USBC_WakeUp_ClearChangeDetect(Reg));
}

VOID USBC_ForceIdToHigh(USB_DRIVER *Driver)
{
  UINT32 Reg;

  Reg = MmioRead32(Driver->Base + USBC_REG_o_ISCR);
  Reg &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
  Reg |= (0x03 << USBC_BP_ISCR_FORCE_ID);
  MmioWrite32(Driver->Base + USBC_REG_o_ISCR, USBC_WakeUp_ClearChangeDetect(Reg));
}

VOID USBC_ForceVbusValidToLow(USB_DRIVER *Driver)
{
  UINT32 Reg;

  Reg = MmioRead32(Driver->Base + USBC_REG_o_ISCR);
  Reg &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
  Reg |= (0x02 << USBC_BP_ISCR_FORCE_VBUS_VALID);
  MmioWrite32(Driver->Base + USBC_REG_o_ISCR, USBC_WakeUp_ClearChangeDetect(Reg));
}

VOID USBC_ForceVbusValidToHigh(USB_DRIVER *Driver)
{
  UINT32 Reg;

  Reg = MmioRead32(Driver->Base + USBC_REG_o_ISCR);
  Reg &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
  Reg |= (0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
  MmioWrite32(Driver->Base + USBC_REG_o_ISCR, USBC_WakeUp_ClearChangeDetect(Reg));
}

// FIXME: SUNXI_SRAMC_BASE valid only for sun4i
#define SUNXI_SRAMC_BASE          0x01c00000

VOID USBC_ConfigFIFO_Base(VOID)
{
  /* config usb fifo, 8kb mode */
  MmioAndThenOr32(
    SUNXI_SRAMC_BASE + 0x04,
    ~(0x03 << 0),
    1
  );
}