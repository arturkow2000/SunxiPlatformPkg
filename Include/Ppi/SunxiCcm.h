#pragma once

#define SUNXI_CCM_PPI_GUID                                                      \
  {                                                                                 \
    0xa5ac9733, 0xa2f0, 0x4c97, { 0xb2, 0x74, 0xe6, 0x9b, 0xee, 0x58, 0xee, 0xf6 }  \
  }

typedef struct _SUNXI_CCM_PPI SUNXI_CCM_PPI;

/**
 Obtain gate ID from gate name.

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  Name                  Gate name.
 @param  OutGateId             Variable to receive gate ID.

 @retval EFI_SUCCESS           Gate ID returned.
 @retval EFI_NOT_FOUND         Gate not found.
 @retval EFI_INVALID_PARAMETER Name is NULL or OutGateId is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_GET_GATE)(
  IN  SUNXI_CCM_PPI           *This,
  IN  CONST CHAR16            *Name,
  OUT UINT32                  *OutGateId
);

/**
 Iterate over available gates.

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  Index                 Gate index.
 @param  OutGateId             Variable to receive gate ID.
 @param  OutGateName           Variable to receive gate name.

 @retval EFI_SUCCESS           Gate ID returned.
 @retval EFI_NOT_FOUND         No more gates.
 @retval EFI_INVALID_PARAMETER OutGateId is NULL
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_GET_NEXT_GATE)(
  IN  SUNXI_CCM_PPI           *This,
  IN  UINT32                  Index,
  OUT UINT32                  *OutGateId,
  OUT CONST CHAR16            **OutGateName OPTIONAL
);

/**
 Unmask gate (enable clock)

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  GateId                Gate ID obtained using SUNXI_CCM_GET_GATE.

 @retval EFI_SUCCESS           Gate unmasked.
 @retval EFI_INVALID_PARAMETER Invalid gate ID, eg. attempt to pass PLL ID as gate ID.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_GATE_UNMASK)(
  IN  SUNXI_CCM_PPI           *This,
  IN  UINT32                  GateId
);

/**
 Mask gate (enable clock)

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  GateId                Gate ID obtained using SUNXI_CCM_GET_GATE.

 @retval EFI_SUCCESS           Gate masked.
 @retval EFI_INVALID_PARAMETER Invalid gate ID, eg. attempt to pass PLL ID as gate ID.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_GATE_MASK)(
  IN  SUNXI_CCM_PPI           *This,
  IN  UINT32                  GateId
);

/**
 Check if gate is masked.
 
 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  GateId                Gate ID obtained using SUNXI_CCM_GET_GATE.
 @param  OutIsMasked           Variable to obtain gate status.

 @retval EFI_SUCCESS           Gate status returned.
 @retval EFI_INVALID_PARAMETER Invalid gate ID, eg. attempt to pass PLL ID as gate ID,
                               OutIsMasked is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_GATE_IS_MASKED)(
  IN  SUNXI_CCM_PPI           *This,
  IN  UINT32                  GateId,
  OUT BOOLEAN                 *OutIsMasked
);

/**
 Obtain PLL id from PLL name.

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  Name                  PLL name.
 @param  OutPllId              Variable to receive PLL ID.

 @retval EFI_SUCCESS           PLL ID returned.
 @retval EFI_NOT_FOUND         PLL not found.
 @retval EFI_INVALID_PARAMETER Name is NULL or OutPllId is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_GET_PLL)(
  IN  SUNXI_CCM_PPI           *This,
  IN  CONST CHAR16            *Name,
  OUT UINT32                  *OutPllId
);

/**
 Iterate over available PLLs.

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  Index                 PLL index.
 @param  OutPllId              Variable to receive PLL ID.
 @param  OutPllName            Variable to receive PLL name.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_GET_NEXT_PLL)(
  IN  SUNXI_CCM_PPI           *This,
  IN  UINT32                  Index,
  OUT UINT32                  *OutPllId,
  OUT CONST CHAR16            **OutPllName OPTIONAL
);

/**
 Get PLL frequency.

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  PllId                 PLL id obtained using SUNXI_CCM_GET_PLL.
 @param  OutPllFreq            Variable to receive PLL frequency in Hz.

 @retval EFI_NOT_FOUND         PLL not found.
 @retval EFI_INVALID_PARAMETER OutPllFreq is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_PLL_GET_FREQ)(
  IN  SUNXI_CCM_PPI           *This,
  IN  UINT32                  PllId,
  OUT UINT32                  *OutPllFreq
);

/**
 Change PLL frequency.

 @param  This                  Pointer to SUNXI_CCM_PPI instance.
 @param  PllId                 PLL id obtained using SUNXI_CCM_GET_PLL.
 @param  Freq                  Requested frequency in Hz, if frequency is 0 clock is disabled.
 
 @retval EFI_NOT_FOUND         PLL not found.
 @retval EFI_UNSUPPORTED       Requested frequency is not supported.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_CCM_PLL_SET_FREQ)(
  IN  SUNXI_CCM_PPI            *This,
  IN  UINT32                   PllId,
  IN  UINT32                   Freq
);

struct _SUNXI_CCM_PPI {
  SUNXI_CCM_GET_GATE GetGate;
  SUNXI_CCM_GET_NEXT_GATE GetNextGate;
  SUNXI_CCM_GATE_UNMASK GateUnmask;
  SUNXI_CCM_GATE_MASK GateMask;
  SUNXI_CCM_GATE_IS_MASKED GateIsMasked;
  SUNXI_CCM_GET_PLL GetPll;
  SUNXI_CCM_GET_NEXT_PLL GetNextPll;
  SUNXI_CCM_PLL_GET_FREQ PllGetFreq;
  SUNXI_CCM_PLL_SET_FREQ PllSetFreq;
};

extern EFI_GUID gSunxiCcmPpiGuid;
