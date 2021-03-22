#include "../Spl.h"
#include "../Sun4i/Dram.h"
#include "../Sun4i/Ccm.h"

VOID DramSetupClock(UINT32 Clock, UINT32 MbusClock)
{
  UINT32 Pll5Cfg;
  UINT32 Pll5pClk;
  UINT32 Pll6Clk;
  UINT32 Pll5pDiv;
  UINT32 Pll6Div;
  UINT32 Pll5pRate;
  UINT32 Pll6Rate;

  Pll5Cfg = MmioRead32(CCM_BASE + CCM_OFFSET_PLL5_CFG);
  Pll5Cfg &= ~CCM_PLL5_CTRL_M_MASK;
  Pll5Cfg &= ~CCM_PLL5_CTRL_K_MASK;
  Pll5Cfg &= ~CCM_PLL5_CTRL_N_MASK;
  Pll5Cfg &= ~CCM_PLL5_CTRL_P_MASK;

  if (Clock >= 540 && Clock <= 552)
  {
    /* dram = 540MHz */
    Pll5Cfg |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
    Pll5Cfg |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(3));
    Pll5Cfg |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(15));
  }
  else if (Clock >= 512 && Clock < 528)
  {
    /* dram = 512MHz */
    Pll5Cfg |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(3));
    Pll5Cfg |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(4));
    Pll5Cfg |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(16));
  }
  else if (Clock >= 496 && Clock < 504)
  {
    /* dram = 496MHz */
    Pll5Cfg |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(3));
    Pll5Cfg |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(2));
    Pll5Cfg |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(31));
  }
  else if (Clock >= 468 && Clock < 480)
  {
    /* dram = 468MHz */
    Pll5Cfg |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
    Pll5Cfg |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(3));
    Pll5Cfg |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(13));
  }
  else if (Clock >= 396 && Clock < 408)
  {
    /* dram = 396MHz */
    Pll5Cfg |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
    Pll5Cfg |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(3));
    Pll5Cfg |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(11));
  }
  else
  {
    /* any other frequency that is a multiple of 24 */
    Pll5Cfg |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
    Pll5Cfg |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(2));
    Pll5Cfg |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(Clock / 24));
  }
  Pll5Cfg &= ~CCM_PLL5_CTRL_VCO_GAIN; /* PLL VCO Gain off */
  Pll5Cfg |= CCM_PLL5_CTRL_EN;
  MmioWrite32(CCM_BASE + CCM_OFFSET_PLL5_CFG, Pll5Cfg);
  SplUdelay(5500);

  MmioOr32(CCM_BASE + CCM_OFFSET_PLL5_CFG, CCM_PLL5_CTRL_DDR_CLK);

  Pll6Clk = CcmGetPll6Freq() / 1000000;
  Pll5pClk = CcmGetPll5pFreq() / 1000000;
  DEBUG((EFI_D_INFO, "PLL5 frequency: %u Hz\n", Pll5pClk));
  DEBUG((EFI_D_INFO, "PLL6 frequency: %u Hz\n", Pll6Clk));
  Pll6Div = (Pll6Clk + MbusClock - 1) / MbusClock;
  Pll5pDiv = (Pll5pClk + MbusClock - 1) / MbusClock;
  Pll6Rate = Pll6Clk / Pll6Div;
  Pll5pRate = Pll5pClk / Pll5pDiv;

  if (Pll6Div <= 16 && Pll6Rate > Pll5pRate)
  {
    DEBUG((EFI_D_INFO, "MBUS frequency: %u Hz (from PLL6)\n", Pll6Rate));
    /* use PLL6 as the MBUS clock source */
    MmioWrite32(
        CCM_BASE + CCM_OFFSET_MBUS_CLK_CFG,
        CCM_MBUS_CTRL_GATE |
            CCM_MBUS_CTRL_CLK_SRC(CCM_MBUS_CTRL_CLK_SRC_PLL6) |
            CCM_MBUS_CTRL_N(CCM_MBUS_CTRL_N_X(1)) |
            CCM_MBUS_CTRL_M(CCM_MBUS_CTRL_M_X(Pll6Div)));
  }
  else if (Pll5pDiv <= 16)
  {
    DEBUG((EFI_D_INFO, "MBUS frequency: %u Hz (from PLL5p)\n", Pll5pRate));
    /* use PLL5P as the MBUS clock source */
    MmioWrite32(
        CCM_BASE + CCM_OFFSET_MBUS_CLK_CFG,
        CCM_MBUS_CTRL_GATE |
            CCM_MBUS_CTRL_CLK_SRC(CCM_MBUS_CTRL_CLK_SRC_PLL5) |
            CCM_MBUS_CTRL_N(CCM_MBUS_CTRL_N_X(1)) |
            CCM_MBUS_CTRL_M(CCM_MBUS_CTRL_M_X(Pll5pDiv)));
  }
  else
  {
    DEBUG((EFI_D_ERROR, "Failed to set MBUS clock.\n"));
    ASSERT(0);
    return;
  }

  MmioAnd32(CCM_BASE + CCM_OFFSET_AHB_GATING0, ~((1 << CCM_AHB_GATE_SDRAM) | (1 << CCM_AHB_GATE_DLL)));
  SplUdelay(22);

  MmioOr32(CCM_BASE + CCM_OFFSET_AHB_GATING0, (1 << CCM_AHB_GATE_SDRAM) | (1 << CCM_AHB_GATE_DLL));
  SplUdelay(22);
}

/*
 * If the dram->ppwrsctl (SDR_DPCR) register has the lowest bit set to 1, this
 * means that DRAM is currently in self-refresh mode and retaining the old
 * data. Since we have no idea what to do in this situation yet, just set this
 * register to 0 and initialize DRAM in the same way as on any normal reboot
 * (discarding whatever was stored there).
 *
 * Note: on sun7i hardware, the highest 16 bits need to be set to 0x1651 magic
 * value for this write operation to have any effect. On sun5i hadware this
 * magic value is not necessary. And on sun4i hardware the writes to this
 * register seem to have no effect at all.
 */
STATIC VOID DisablePowerSave(VOID)
{
  MmioWrite32(DRAMC_PPWRSCTL, 0x16510000);
}

STATIC VOID SetDrive(VOID)
{
  MmioAndThenOr32(DRAMC_MCR, ~(DRAM_MCR_MODE_NORM(3)), DRAM_MCR_MODE_EN(3) | 0xffc);
}

STATIC UINT32 GetNumberOfLanes(VOID)
{
  if ((MmioRead32(DRAMC_DCR) & DRAM_DCR_BUS_WIDTH_MASK) == DRAM_DCR_BUS_WIDTH(DRAM_DCR_BUS_WIDTH_32BIT))
    return 4;
  else
    return 2;
}

STATIC VOID EnableDll0(UINT32 Phase)
{
  MmioAndThenOr32(DRAMC_DLLCR0, ~(0x3f << 6), ((Phase >> 16) & 0x3f) << 6);
  MmioAndThenOr32(DRAMC_DLLCR0, ~DRAM_DLLCR_NRESET, DRAM_DLLCR_DISABLE);
  SplUdelay(2);

  MmioAnd32(DRAMC_DLLCR0, ~(DRAM_DLLCR_NRESET | DRAM_DLLCR_DISABLE));
  SplUdelay(22);

  MmioAndThenOr32(DRAMC_DLLCR0, ~DRAM_DLLCR_DISABLE, DRAM_DLLCR_NRESET);
  SplUdelay(22);
}

STATIC VOID EnableDllx(UINT32 Phase)
{
  UINT32 NumberOfLanes;
  UINT32 i;

  NumberOfLanes = GetNumberOfLanes();
  for (i = 1; i <= NumberOfLanes; i++)
  {
    MmioAndThenOr32(DRAMC_DLLCR0 + 4 * i, ~(0xf << 14), (Phase & 0xf) << 14);
    MmioAndThenOr32(DRAMC_DLLCR0 + 4 * i, ~(DRAM_DLLCR_NRESET), DRAM_DLLCR_DISABLE);
    Phase >>= 4;
  }
  SplUdelay(2);

  for (i = 1; i <= NumberOfLanes; i++)
    MmioAnd32(DRAMC_DLLCR0 + 4 * i, ~(DRAM_DLLCR_NRESET | DRAM_DLLCR_DISABLE));
  SplUdelay(22);

  for (i = 1; i <= NumberOfLanes; i++)
    MmioAndThenOr32(DRAMC_DLLCR0 + 4 * i, ~DRAM_DLLCR_DISABLE, DRAM_DLLCR_NRESET);
  SplUdelay(22);
}

STATIC VOID SetImpedance(UINT32 Zq, BOOLEAN OdtEn)
{
  UINT32 Reg;
  UINT32 Zprog;
  UINT32 Zdata;

  Zprog = Zq & 0xff;
  Zdata = (Zq >> 8) & 0xffff;

  /* Appears that some kind of automatically initiated default
   * ZQ calibration is already in progress at this point on sun4i/sun5i
   * hardware, but not on sun7i. So it is reasonable to wait for its
   * completion before doing anything else.
   */
  while ((MmioRead32(DRAMC_ZQSR) & DRAM_ZQSR_ZDONE) == 0)
  {
  }

  /* ZQ calibration is not really useful unless ODT is enabled */
  if (!OdtEn)
    return;

  /* Needed at least for sun5i, because it does not self clear there */
  MmioAnd32(DRAMC_ZQCR0, ~DRAM_ZQCR0_ZCAL);

  if (Zdata)
  {
    /* Set the user supplied impedance data */
    Reg = DRAM_ZQCR0_ZDEN | Zdata;
    MmioWrite32(DRAMC_ZQCR0, Reg);
    /* no need to wait, this takes effect immediately */
  }
  else
  {
    Reg = DRAM_ZQCR0_ZCAL | DRAM_ZQCR0_IMP_DIV(Zprog);
    MmioWrite32(DRAMC_ZQCR0, Reg);
    /* Wait for the new impedance configuration to settle */
    while ((MmioRead32(DRAMC_ZQSR) & DRAM_ZQSR_ZDONE) == 0)
    {
    }
  }

  /* Needed at least for sun5i, because it does not self clear there */
  MmioAnd32(DRAMC_ZQCR0, ~DRAM_ZQCR0_ZCAL);

  /* Set I/O configure register */
  MmioWrite32(DRAMC_IOCR, DRAM_IOCR_ODT_EN);
}

/*
 * After the DRAM is powered up or reset, the DDR3 spec requires to wait at
 * least 500 us before driving the CKE pin (Clock Enable) high. The dram->idct
 * (SDR_IDCR) register appears to configure this delay, which gets applied
 * right at the time when the DRAM initialization is activated in the
 * 'mctl_ddr3_initialize' function.
 */
STATIC VOID SetCkeDelay(VOID)
{
  /* The CKE delay is represented in DRAM clock cycles, multiplied by N
   * (where N=2 for sun4i/sun5i and N=3 for sun7i). Here it is set to
   * the maximum possible value 0x1ffff, just like in the Allwinner's
   * boot0 bootloader. The resulting delay value is somewhere between
   * ~0.4 ms (sun5i with 648 MHz DRAM clock speed) and ~1.1 ms (sun7i
   * with 360 MHz DRAM clock speed).
   */
  MmioOr32(DRAMC_IDCR, 0x1ffff);
}

/*
 * This performs the external DRAM reset by driving the RESET pin low and
 * then high again. According to the DDR3 spec, the RESET pin needs to be
 * kept low for at least 200 us.
 */
STATIC VOID Ddr3Reset(VOID)
{
  MmioAnd32(DRAMC_MCR, ~DRAM_MCR_RESET);
  SplUdelay(200);
  MmioOr32(DRAMC_MCR, DRAM_MCR_RESET);

  /* After the RESET pin is de-asserted, the DDR3 spec requires to wait
   * for additional 500 us before driving the CKE pin (Clock Enable)
   * high. The duration of this delay can be configured in the SDR_IDCR
   * (Initialization Delay Configuration Register) and applied
   * automatically by the DRAM controller during the DDR3 initialization
   * step. But SDR_IDCR has limited range on sun4i/sun5i hardware and
   * can't provide sufficient delay at DRAM clock frequencies higher than
   * 524 MHz (while Allwinner A13 supports DRAM clock frequency up to
   * 533 MHz according to the datasheet). Additionally, there is no
   * official documentation for the SDR_IDCR register anywhere, and
   * there is always a chance that we are interpreting it wrong.
   * Better be safe than sorry, so add an explicit delay here.
   */
  SplUdelay(500);
}

/* tRFC in nanoseconds for different densities (from the DDR3 spec) */
STATIC CONST UINT16 tRFC_DDR3_table[6] = {
    /* 256Mb    512Mb    1Gb      2Gb      4Gb      8Gb */
    90, 90, 110, 160, 300, 350};

STATIC VOID SetAutoRefreshCycle(UINT32 Clk, UINT32 Density)
{
  UINT32 tRFC;
  UINT32 tREFI;

  tRFC = (tRFC_DDR3_table[Density] * Clk + 999) / 1000;
  tREFI = (7987 * Clk) >> 10; /* <= 7.8us */

  MmioWrite32(DRAMC_DRR, DRAM_DRR_TREFI(tREFI) | DRAM_DRR_TRFC(tRFC));
}

/* Calculate the value for A11, A10, A9 bits in MR0 (write recovery) */
STATIC UINT32 Ddr3WriteRecovery(UINT32 Clk)
{
  UINT32 TwrNs;
  UINT32 TwrCk;

  TwrNs = 15; /* DDR3 spec says that it is 15ns for all speed bins */
  TwrCk = (TwrNs * Clk + 999) / 1000;

  if (TwrCk < 5)
    return 1;
  else if (TwrCk <= 8)
    return TwrCk - 4;
  else if (TwrCk <= 10)
    return 5;
  else
    return 6;
}

STATIC INT32 ScanReadPipe()
{
  /* data training trigger */
  MmioAnd32(DRAMC_CSR, ~DRAM_CSR_FAILED);
  MmioOr32(DRAMC_CCR, DRAM_CCR_DATA_TRAINING);

  /* check whether data training process has completed */
  while (MmioRead32(DRAMC_CCR) & DRAM_CCR_DATA_TRAINING)
  {
  }

  if (MmioRead32(DRAMC_CSR) & DRAM_CSR_FAILED)
    return -1;

  return 0;
}

STATIC CONST UINT32 HpcrValue[32] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0x1031, 0x1031, 0x0735, 0x1035,
    0x1035, 0x0731, 0x1031, 0,
    0x0301, 0x0301, 0x0301, 0x0301,
    0x0301, 0x0301, 0x0301, 0};

UINT32 DramInit(SUN4I_DRAM_PARA *Para)
{
  UINT32 Dcr;
  UINT32 Density;
  UINT32 i;

  if (Para->Type != DRAM_MEMORY_TYPE_DDR3 || Para->RankNum != 1 || Para->MbusClock == 0)
  {
    ASSERT(0);
    return 0;
  }

  /* setup DRAM relative clock */
  DramSetupClock(Para->Clock, Para->MbusClock);

  /* Disable any pad power save control */
  DisablePowerSave();

  SetDrive();

  /* disable DRAM clock */
  MmioAnd32(DRAMC_MCR, ~DRAM_MCR_DCLK_OUT);

  MmioAndThenOr32(DRAMC_CCR, ~DRAM_CCR_INIT, DRAM_CCR_ITM_OFF);
  EnableDll0(Para->Tpr3);

  Dcr = DRAM_DCR_TYPE_DDR3 | DRAM_DCR_IO_WIDTH(Para->IoWidth >> 3);
  switch (Para->Density)
  {
  case 256:
    Density = DRAM_DCR_CHIP_DENSITY_256M;
    break;
  case 512:
    Density = DRAM_DCR_CHIP_DENSITY_512M;
    break;
  case 1024:
    Density = DRAM_DCR_CHIP_DENSITY_1024M;
    break;
  case 2048:
    Density = DRAM_DCR_CHIP_DENSITY_2048M;
    break;
  case 4096:
    Density = DRAM_DCR_CHIP_DENSITY_4096M;
    break;
  case 8192:
    Density = DRAM_DCR_CHIP_DENSITY_8192M;
    break;
  default:
    Density = DRAM_DCR_CHIP_DENSITY_256M;
    break;
  }
  Dcr |= DRAM_DCR_CHIP_DENSITY(Density);
  Dcr |= DRAM_DCR_BUS_WIDTH((Para->BusWidth >> 3) - 1);
  Dcr |= DRAM_DCR_RANK_SEL(Para->RankNum - 1);
  Dcr |= DRAM_DCR_CMD_RANK_ALL;
  Dcr |= DRAM_DCR_MODE(DRAM_DCR_MODE_INTERLEAVE);
  MmioWrite32(DRAMC_DCR, Dcr);

  MmioOr32(DRAMC_MCR, DRAM_MCR_DCLK_OUT);

  SetImpedance(Para->Zq, Para->OdtEnable);

  SetCkeDelay();

  Ddr3Reset();

  SplUdelay(1);

  while ((MmioRead32(DRAMC_CCR) & DRAM_CCR_INIT) != 0)
  {
  }

  EnableDllx(Para->Tpr3);

  /* set refresh period */
  SetAutoRefreshCycle(Para->Clock, Density);

  /* set timing parameters */
  MmioWrite32(DRAMC_TPR0, Para->Tpr0);
  MmioWrite32(DRAMC_TPR1, Para->Tpr1);
  MmioWrite32(DRAMC_TPR2, Para->Tpr2);

  MmioWrite32(
      DRAMC_MR,
      DRAM_MR_BURST_LENGTH(0x0) | DRAM_MR_POWER_DOWN |
          DRAM_MR_CAS_LAT(Para->Cas - 4) | DRAM_MR_WRITE_RECOVERY(Ddr3WriteRecovery(Para->Clock)));

  MmioWrite32(DRAMC_EMR, Para->Emr1);
  MmioWrite32(DRAMC_EMR2, Para->Emr2);
  MmioWrite32(DRAMC_EMR3, Para->Emr3);

  /* disable drift compensation and set passive DQS window mode */
  MmioAndThenOr32(DRAMC_CCR, ~DRAM_CCR_DQS_DRIFT_COMP, DRAM_CCR_DQS_GATE);

  /* initialize external DRAM */
  MmioOr32(DRAMC_CCR, DRAM_CCR_INIT);
  while (MmioRead32(DRAMC_CCR) & DRAM_CCR_INIT)
  {
  }

  MmioAnd32(DRAMC_CCR, ~DRAM_CCR_ITM_OFF);

  if (ScanReadPipe() < 0)
    return 0;

  /* allow to override the DQS training results with a custom delay */
  if (Para->DqsGatingDelay)
  {
    ASSERT(0);
    return 0;
  }

  /* set the DQS gating window type */
  if (Para->ActiveWindowing)
    MmioAnd32(DRAMC_CCR, ~DRAM_CCR_DQS_GATE);
  else
    MmioOr32(DRAMC_CCR, DRAM_CCR_DQS_GATE);

  MmioAndThenOr32(DRAMC_CCR, ~DRAM_CCR_INIT, DRAM_CCR_ITM_OFF);
  SplUdelay(1);
  MmioAnd32(DRAMC_CCR, ~DRAM_CCR_ITM_OFF);
  SplUdelay(1);

  /* configure all host port */
  for (i = 0; i < 32; i++)
    MmioWrite32(DRAMC_HPCR(i), HpcrValue[i]);

  return DramDetect((UINT64 *)0x40000000, 0x80000000);
}
