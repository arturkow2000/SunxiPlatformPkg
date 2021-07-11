#include "Driver.h"
#include "Hw.h"

UINT32 UsbPhyRead(USB_PHY_DRIVER *Driver, UINT32 Phy, UINT32 Addr, UINT32 Len) {
  UINT32 PhyCtl = Driver->SocConfig->PhyCtrlBase + Driver->SocConfig->PhyCtlOffset;
  UINT32 i;
  UINT32 Temp;
  UINT32 Value = 0;

  if (Driver->SocConfig->PhyCtlOffset == REG_PHYCTL_A33) {
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

VOID UsbPhyWrite(USB_PHY_DRIVER *Driver, UINT32 Phy, UINT32 Addr, UINT32 Data, UINT32 Len) {
  UINT32 PhyCtl = Driver->SocConfig->PhyCtrlBase + Driver->SocConfig->PhyCtlOffset;
  UINT32 i;
  UINT32 Temp;
  UINT32 UsbcBit = 1 << (Phy * 2);
  
  if (Driver->SocConfig->PhyCtlOffset == REG_PHYCTL_A33) {
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

VOID UsbPhyPassby(USB_PHY_DRIVER *Driver, UINT32 Phy, BOOLEAN Enable) {
  UINT32 Bits;
  UINT32 Pmu;

  ASSERT(Driver->SocConfig->Pmu);

  if (!Driver->SocConfig->Pmu[Phy])
    return;

  Pmu = Driver->SocConfig->Pmu[Phy];

  Bits = SUNXI_AHB_ICHR8_EN | SUNXI_AHB_INCR4_BURST_EN | SUNXI_AHB_INCRX_ALIGN_EN | SUNXI_ULPI_BYPASS_EN;

  /* A83T USB2 is HSIC */
  if (Driver->SocConfig->Type == PHY_TYPE_SUN8I_A83T && Phy == 2)
    Bits |= SUNXI_EHCI_HS_FORCE | SUNXI_HSIC_CONNECT_INT | SUNXI_HSIC;

  if (Enable)
    MmioOr32(Pmu, Bits);
  else
    MmioAnd32(Pmu, ~Bits);
}

VOID UsbPhy0Route(USB_PHY_DRIVER *Driver, BOOLEAN Gadget) {
  UINT32 Reg;

  Reg = MmioRead32(Driver->SocConfig->PhyCtrlBase + REG_PHY_OTGCTL);
  DEBUG((EFI_D_INFO, "phy0 reg=0x%x mode %s -> %s\n", Reg, Reg & OTGCTL_ROUTE_MUSB ? L"gadget" : L"host", Gadget ? L"gadget" : L"host"));

  if (Gadget)
    Reg |= OTGCTL_ROUTE_MUSB;
  else
    Reg &= ~OTGCTL_ROUTE_MUSB;
  
  MmioWrite32(Driver->SocConfig->PhyCtrlBase + REG_PHY_OTGCTL, Reg);
}

EFI_STATUS UsbPhyInitHw(USB_PHY_DRIVER *Driver, UINT32 Phy) {
  if (Driver->SocConfig->Type == PHY_TYPE_SUN8I_A83T || Driver->SocConfig->Type == PHY_TYPE_SUN50I_H6) {
    if (Phy == 0) {
      MmioAndThenOr32(
        Driver->SocConfig->PhyCtrlBase + Driver->SocConfig->PhyCtlOffset,
        ~PHY_CTL_SIDDQ,
        PHY_CTL_VBUSVLDEXT
      );
    }
  } else {
    ASSERT(Driver->SocConfig->Pmu);
    if (Driver->SocConfig->EnablePmuUnk1 && Driver->SocConfig->Pmu[Phy])
      MmioAnd32(Driver->SocConfig->Pmu[Phy] + REG_PMU_UNK1, ~2);

    if (Phy == 0)
      UsbPhyWrite(Driver, Phy, PHY_RES45_CAL_EN, 1, 1);

    /* Adjust PHY's magnitude and rate */
    UsbPhyWrite(Driver, Phy, PHY_TX_AMPLITUDE_TUNE, PHY_TX_MAGNITUDE | PHY_TX_RATE, 5);

    /* Disconnect threshold adjustment */
    UsbPhyWrite(Driver, Phy, PHY_DISCON_TH_SEL, Driver->SocConfig->DisconnectThreshold, 2);
  }

  UsbPhyPassby(Driver, Phy, TRUE);
  UsbPhy0Route(Driver, TRUE);

  return EFI_SUCCESS;
}