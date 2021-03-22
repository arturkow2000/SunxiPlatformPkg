#include "Spl.h"
#include "Dram.h"

STATIC SUN4I_DRAM_PARA mDramPara = {
    .Clock = FixedPcdGet32(DramClock),
    .MbusClock = FixedPcdGet32(DramMbusClock),
    .Type = FixedPcdGet64(DramType),
    .RankNum = FixedPcdGet32(DramRankNum),
    .Density = FixedPcdGet32(DramDensity),
    .Zq = FixedPcdGet32(DramZq),
    .OdtEnable = FixedPcdGet32(DramOdtEnable),
    .Cas = FixedPcdGet32(DramCas),
    .Tpr0 = FixedPcdGet32(DramTpr0),
    .Tpr1 = FixedPcdGet32(DramTpr1),
    .Tpr2 = FixedPcdGet32(DramTpr2),
    .Tpr3 = FixedPcdGet32(DramTpr3),
    .Tpr4 = FixedPcdGet32(DramTpr4),
    .Tpr5 = FixedPcdGet32(DramTpr5),
    .Emr1 = FixedPcdGet32(DramEmr1),
    .Emr2 = FixedPcdGet32(DramEmr2),
    .Emr3 = FixedPcdGet32(DramEmr3),
    .DqsGatingDelay = FixedPcdGet32(DramDqsGatingDelay),
    .ActiveWindowing = FixedPcdGet32(DramActiveWindowing),
};

UINT32 SplDramInit(VOID)
{
  UINT32 DramSize;
  UINT32 ActualDensity;

  if (mDramPara.Clock == 0 || mDramPara.MbusClock == 0 || mDramPara.Density == 0)
  {
    ASSERT(0);
    return 0;
  }

  if (mDramPara.IoWidth && mDramPara.BusWidth && mDramPara.Density)
    return DramInit(&mDramPara);

  /* try to autodetect the DRAM bus width and density */
  mDramPara.IoWidth = 16;
  mDramPara.BusWidth = 32;
  /* mDramPara.Density is already set to maximum supported density */

  DramSize = DramInit(&mDramPara);
  if (!DramSize)
  {
    DEBUG((EFI_D_INFO, "Trying with 16-bit bus width\n"));
    /* if 32-bit bus width failed, try 16-bit bus width instead */
    mDramPara.BusWidth = 16;
    DramSize = DramInit(&mDramPara);
    if (!DramSize)
    {
      /* if 16-bit bus width also failed, then bail out */
      DEBUG((EFI_D_ERROR, "DRAM initialization failed\n"));
      ASSERT(0);
      return 0;
    }
  }

  /* check if we need to adjust the density */
  ActualDensity = (DramSize >> 17) * mDramPara.IoWidth / mDramPara.BusWidth;

  if (ActualDensity != mDramPara.Density)
  {
    /* update the density and re-initialize DRAM again */
    mDramPara.Density = ActualDensity;
    DramSize = DramInit(&mDramPara);
  }

  return DramSize;
}
