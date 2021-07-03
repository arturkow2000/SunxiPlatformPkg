#pragma once

#define SUNXI_GPIO_PPI_GUID                                                         \
  {                                                                                 \
    0xaecc4fdd, 0x9762, 0x4775, { 0xbc, 0xab, 0xca, 0xf2, 0x34, 0x62, 0x3f, 0xef }  \
  }

typedef struct _SUNXI_GPIO_PPI SUNXI_GPIO_PPI;
typedef UINT32 SUNXI_GPIO_PIN;

// FIXME: workaround to allow using both SunxiGpioLib and Gpio protocol headers
#ifdef SUNXI_GPIO_PULL_DISABLE
#undef SUNXI_GPIO_PULL_DISABLE
#endif

#ifdef SUNXI_GPIO_PULL_UP
#undef SUNXI_GPIO_PULL_UP
#endif

#ifdef SUNXI_GPIO_PULL_DOWN
#undef SUNXI_GPIO_PULL_DOWN
#endif

typedef enum {
  SUNXI_GPIO_PULL_DISABLE = 0,
  SUNXI_GPIO_PULL_UP = 1,
  SUNXI_GPIO_PULL_DOWN = 2,
} SUNXI_GPIO_PULL_MODE;

/**
 Obtains SUNXI_GPIO_PIN from pin name.

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Name                   Pin name in format Pxyy where x is bank letter and yy is pin number.
 @param OutPin                 Pin ID.

 @retval EFI_SUCCESS           Pin ID was written to OutPin.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER Either OutPin is NULl or pin Name format is invalid.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_PIN)(
  IN  SUNXI_GPIO_PPI *This,
  IN  CONST CHAR16 *Name,
  OUT SUNXI_GPIO_PIN *OutPin
);

/**
 Get current pin function.

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param OutFunction            Pointer to variable to receive function name.

 @retval EFI_SUCCESS           Function returned.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER OutFunction is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_FUNCTION)(
  IN  SUNXI_GPIO_PPI *This,
  IN  SUNXI_GPIO_PIN Pin,
  OUT CONST CHAR16 **OutFunction
);

/**
 Change pin function.

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param Function               Function name.

 @retval EFI_SUCCESS           Function was changed.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_UNSUPPORTED       Pin does not support requested function.
 @retval EFI_INVALID_PARAMETER Function is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_SET_FUNCTION)(
  IN SUNXI_GPIO_PPI *This,
  IN SUNXI_GPIO_PIN Pin,
  IN CONST CHAR16 *Function
);

/**
 Get pin pull mode.

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param OutPullMode            Pointer to variable to receive pull mode.

 @retval EFI_SUCCESS           Pull mode returned.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER OutPullMode is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_PULL_MODE)(
  IN  SUNXI_GPIO_PPI *This,
  IN  SUNXI_GPIO_PIN Pin,
  OUT SUNXI_GPIO_PULL_MODE *OutPullMode
);

/**
 Change pin pull mode.

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param PullMode               Pull mode.

 @retval EFI_SUCCESS           Pull mode changed.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER Pull mode is invalid.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_SET_PULL_MODE)(
  IN SUNXI_GPIO_PPI *This,
  IN SUNXI_GPIO_PIN Pin,
  IN SUNXI_GPIO_PULL_MODE Mode
);

/**
 Get pin drive strength.
 
 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param OutDriveStrength       Pointer to variable to receive current drive strength in mA.

 @retval EFI_SUCCESS           Drive strength returned.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER OutDriveStrength is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_DRIVE_STRENGTH)(
  IN  SUNXI_GPIO_PPI *This,
  IN  SUNXI_GPIO_PIN Pin,
  OUT UINT32 *OutDriveStrength
);

/**
 Change pin drive strength.

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param DriveStrength          Drive strength in mA.

 @retval EFI_SUCCESS           Drive strength changed.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_UNSUPPORTED       Pin does not support specified drive strength.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_SET_DRIVE_STRENGTH)(
  IN SUNXI_GPIO_PPI *This,
  IN SUNXI_GPIO_PIN Pin,
  IN UINT32 DriveStrength
);

struct _SUNXI_GPIO_PPI {
  SUNXI_GPIO_GET_PIN GetPin;
  SUNXI_GPIO_GET_FUNCTION GetFunction;
  SUNXI_GPIO_SET_FUNCTION SetFunction;
  SUNXI_GPIO_GET_PULL_MODE GetPullMode;
  SUNXI_GPIO_SET_PULL_MODE SetPullMode;
  SUNXI_GPIO_GET_DRIVE_STRENGTH GetDriveStrength;
  SUNXI_GPIO_SET_DRIVE_STRENGTH SetDriveStrength;
};

extern EFI_GUID gSunxiGpioPpiGuid;
