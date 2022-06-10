#include "Phy.h"
#include "Hw.h"

#if 0
STATIC UINT32 UsbPhyRead(UINT32 Phy, UINT32 Addr, UINT32 Len) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  UINT32 PhyCtl = Config->PhyCtrlBase + Config->PhyCtlOffset;
  UINT32 i;
  UINT32 Temp;
  UINT32 Value = 0;

  if (Config->PhyCtlOffset == REG_PHYCTL_A33) {
    /* SoCs newer than A33 need us to set phyctl to 0 explicitly */
    MmioWrite32(PhyCtl, 0);
  }

  for (i = Len; i > 0; i--) {
    Temp = MmioRead32(PhyCtl);

    /* clear the address portion */
    Temp &= ~(0xff << 8);

    /* set the address */
    Temp |= ((Addr + i - 1) << 8);
    MmioWrite32(PhyCtl, Temp);

    Temp = MmioRead32(PhyCtl);
    Value <<= 1;
    Value |= (Temp >> (16 + Phy)) & 1;
  }

  return Value;
}
#endif

STATIC VOID UsbPhyWrite(UINT32 Phy, UINT32 Addr, UINT32 Data, UINT32 Len) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  UINT32 PhyCtl = Config->PhyCtrlBase + Config->PhyCtlOffset;
  UINT32 i;
  UINT32 Temp;
  UINT32 UsbcBit = 1 << (Phy * 2);
  
  if (Config->PhyCtlOffset == REG_PHYCTL_A33) {
    /* SoCs newer than A33 need us to set phyctl to 0 explicitly */
    MmioWrite32(PhyCtl, 0);
  }

  for (i = 0; i < Len; i++) {
    Temp = MmioRead32(PhyCtl);

    /* clear the address portion */
    Temp &= ~(0xff << 8);

    /* set the address */
    Temp |= ((Addr + i) << 8);
    MmioWrite32(PhyCtl, Temp);

    /* set the data bit and clear usbc bit */
    Temp = MmioRead8(PhyCtl);
    if (Data & 1)
      Temp |= PHYCTL_DATA;
    else
      Temp &= ~PHYCTL_DATA;

    Temp &= ~UsbcBit;
    MmioWrite8(PhyCtl, Temp);

    /* pulse usbc_bit */
    MmioOr8(PhyCtl, UsbcBit);
    MmioAnd8(PhyCtl, ~UsbcBit);

    Data >>= 1;
  }
}

STATIC VOID UsbPhyPassby(UINT32 Phy, BOOLEAN Enable) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  UINT32 Bits;
  UINT32 Pmu;

  ASSERT(Config->Pmu);

  if (!Config->Pmu[Phy])
    return;

  Pmu = Config->Pmu[Phy];

  Bits = SUNXI_AHB_ICHR8_EN | SUNXI_AHB_INCR4_BURST_EN | SUNXI_AHB_INCRX_ALIGN_EN | SUNXI_ULPI_BYPASS_EN;

  /* A83T USB2 is HSIC */
  if (Config->Type == PHY_TYPE_SUN8I_A83T && Phy == 2)
    Bits |= SUNXI_EHCI_HS_FORCE | SUNXI_HSIC_CONNECT_INT | SUNXI_HSIC;

  if (Enable)
    MmioOr32(Pmu, Bits);
  else
    MmioAnd32(Pmu, ~Bits);
}

STATIC VOID UsbPhy0Route(BOOLEAN Gadget) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  UINT32 Reg;

  Reg = MmioRead32(Config->PhyCtrlBase + REG_PHY_OTGCTL);
  DEBUG((EFI_D_INFO, "phy0 reg=0x%x mode %s -> %s\n", Reg, Reg & OTGCTL_ROUTE_MUSB ? L"gadget" : L"host", Gadget ? L"gadget" : L"host"));

  if (Gadget)
    Reg |= OTGCTL_ROUTE_MUSB;
  else
    Reg &= ~OTGCTL_ROUTE_MUSB;
  
  MmioWrite32(Config->PhyCtrlBase + REG_PHY_OTGCTL, Reg);
}

STATIC EFI_STATUS UsbPhyInitHw(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  if (Config->Type == PHY_TYPE_SUN8I_A83T || Config->Type == PHY_TYPE_SUN50I_H6) {
    if (Phy == 0) {
      MmioAndThenOr32(
        Config->PhyCtrlBase + Config->PhyCtlOffset,
        ~PHY_CTL_SIDDQ,
        PHY_CTL_VBUSVLDEXT
      );
    }
  } else {
    ASSERT(Config->Pmu);
    if (Config->EnablePmuUnk1 && Config->Pmu[Phy])
      MmioAnd32(Config->Pmu[Phy] + REG_PMU_UNK1, ~2);

    if (Phy == 0)
      UsbPhyWrite(Phy, PHY_RES45_CAL_EN, 1, 1);

    /* Adjust PHY's magnitude and rate */
    UsbPhyWrite(Phy, PHY_TX_AMPLITUDE_TUNE, PHY_TX_MAGNITUDE | PHY_TX_RATE, 5);

    /* Disconnect threshold adjustment */
    UsbPhyWrite(Phy, PHY_DISCON_TH_SEL, Config->DisconnectThreshold, 2);
  }

  // Force peripheral mode, we don't support host mode
  UsbPhyPassby(Phy, FALSE);
  if (Config->Type == PHY_TYPE_SUN8I_H3
    || Config->Type == PHY_TYPE_SUN8I_R40
    || Config->Type == PHY_TYPE_SUN8I_V3S
    || Config->Type == PHY_TYPE_SUN50I_A64
    || Config->Type == PHY_TYPE_SUN50I_H6) {
    UsbPhy0Route(TRUE);
  }

  return EFI_SUCCESS;
}

EFI_STATUS SunxiUsbPhyInit() {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  EFI_STATUS Status;
  UINT32 i;

  for (i = 0; i < Config->NumPhys; i++) {
    if (Config->MissingPhys & (1 << i))
      continue;

    Status = UsbPhyInitGpio(i);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "phy%u GPIO init failed\n", i));
      return Status;
    }

    Status = UsbPhyInitClocks(i);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "phy%u clocks init failed\n", i));
      return Status;
    }

    Status = UsbPhyInitHw(i);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "phy%u init failed\n", i));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

#define PHY_CHANGE_DETECT ((1 << PHY_ISCR_VBUS_CHANGE_DETECT) | (1 << PHY_ISCR_ID_CHANGE_DETECT) | (1 << PHY_ISCR_DPDM_CHANGE_DETECT))

STATIC inline UINT32 UsbPhyClearChangeDetect(UINT32 Reg)
{
  Reg &= ~(1 << PHY_ISCR_VBUS_CHANGE_DETECT);
  Reg &= ~(1 << PHY_ISCR_ID_CHANGE_DETECT);
  Reg &= ~(1 << PHY_ISCR_DPDM_CHANGE_DETECT);

  return Reg;
}

#define PHY_VERIFY(Phy)                                           \
  if (Phy >= Config->NumPhys || Config->MissingPhys & (1 << Phy)) \
    return EFI_NOT_FOUND;

EFI_STATUS SunxiUsbPhyEnableIdPullup(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;

  PHY_VERIFY(Phy);
  MmioAndThenOr32(
    Config->PhyCtrlBase,
    ~PHY_CHANGE_DETECT,
    1 << PHY_ISCR_ID_PULLUP_EN
  );

  return EFI_SUCCESS;
}

EFI_STATUS SunxiUsbPhyEnableDmDpPullup(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;

  PHY_VERIFY(Phy);
  MmioAndThenOr32(
    Config->PhyCtrlBase,
    ~PHY_CHANGE_DETECT,
    1 << PHY_ISCR_DPDM_PULLUP_EN
  );

  return EFI_SUCCESS;
}

EFI_STATUS SunxiUsbPhyForceIdToLow(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;

  PHY_VERIFY(Phy);
  MmioAndThenOr32(
    Config->PhyCtrlBase,
    ~(PHY_CHANGE_DETECT | (3 << PHY_ISCR_FORCE_ID)),
    2 << PHY_ISCR_FORCE_ID
  );

  return EFI_SUCCESS;
}

EFI_STATUS SunxiUsbPhyForceIdToHigh(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;

  PHY_VERIFY(Phy);
  MmioAndThenOr32(
    Config->PhyCtrlBase,
    ~(PHY_CHANGE_DETECT | (3 << PHY_ISCR_FORCE_ID)),
    3 << PHY_ISCR_FORCE_ID
  );

  return EFI_SUCCESS;
}

EFI_STATUS SunxiUsbPhyForceVbusValidToLow(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;

  PHY_VERIFY(Phy);
  MmioAndThenOr32(
    Config->PhyCtrlBase,
    ~(PHY_CHANGE_DETECT | (3 << PHY_ISCR_FORCE_VBUS_VALID)),
    2 << PHY_ISCR_FORCE_VBUS_VALID
  );

  return EFI_SUCCESS;
}

EFI_STATUS SunxiUsbPhyForceVbusValidToHigh(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;

  PHY_VERIFY(Phy);
  MmioAndThenOr32(
    Config->PhyCtrlBase,
    ~(PHY_CHANGE_DETECT | (3 << PHY_ISCR_FORCE_VBUS_VALID)),
    3 << PHY_ISCR_FORCE_VBUS_VALID
  );

  return EFI_SUCCESS;
}

