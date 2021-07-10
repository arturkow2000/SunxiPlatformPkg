#pragma once

#define SUNXI_GPIO_PROTOCOL_GUID                                                   \
  {                                                                                \
    0x9810a737, 0x7a22, 0x4864, { 0xa1, 0x93, 0x08, 0xdb, 0xd0, 0x84, 0xfa, 0xc9 } \
  }

typedef struct _SUNXI_GPIO_PROTOCOL SUNXI_GPIO_PROTOCOL;

/**
 Obtains pin ID from pin name.

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Name                   Pin name in format Pxyy where x is bank letter and yy is pin number.
 @param OutPin                 Pin ID.

 @retval EFI_SUCCESS           Pin ID was written to OutPin.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER Either OutPin is NULl or pin Name format is invalid.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_PIN)(
  IN  SUNXI_GPIO_PROTOCOL *This,
  IN  CONST CHAR16 *Name,
  OUT UINT32 *OutPin
);

/**
 Get current pin function.

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param OutFunction            Pointer to variable to receive function name.

 @retval EFI_SUCCESS           Function returned.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER OutFunction is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_FUNCTION)(
  IN  SUNXI_GPIO_PROTOCOL *This,
  IN  UINT32 Pin,
  OUT CONST CHAR16 **OutFunction
);

/**
 Change pin function.

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
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
  IN SUNXI_GPIO_PROTOCOL *This,
  IN UINT32 Pin,
  IN CONST CHAR16 *Function
);

/**
 Get pin pull mode.

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param OutPullMode            Pointer to variable to receive pull mode.

 @retval EFI_SUCCESS           Pull mode returned.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER OutPullMode is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_PULL_MODE)(
  IN  SUNXI_GPIO_PROTOCOL *This,
  IN  UINT32 Pin,
  OUT UINT32 *OutPullMode
);

/**
 Change pin pull mode.

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param PullMode               Pull mode.

 @retval EFI_SUCCESS           Pull mode changed.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER Pull mode is invalid.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_SET_PULL_MODE)(
  IN SUNXI_GPIO_PROTOCOL *This,
  IN UINT32 Pin,
  IN UINT32 PullMode
);

/**
 Get pin drive strength.
 
 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param OutDriveStrength       Pointer to variable to receive current drive strength in mA.

 @retval EFI_SUCCESS           Drive strength returned.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER OutDriveStrength is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_DRIVE_STRENGTH)(
  IN  SUNXI_GPIO_PROTOCOL *This,
  IN  UINT32 Pin,
  OUT UINT32 *OutDriveStrength
);

/**
 Change pin drive strength.

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param DriveStrength          Drive strength in mA.

 @retval EFI_SUCCESS           Drive strength changed.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_UNSUPPORTED       Pin does not support specified drive strength.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_SET_DRIVE_STRENGTH)(
  IN SUNXI_GPIO_PROTOCOL *This,
  IN UINT32 Pin,
  IN UINT32 DriveStrength
);

/**
 Set pin direction to output

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.

 @retval EFI_SUCCESS           Pin was configured as output.
 @retval EFI_NOT_FOUND         Requested pin was not found.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_CONFIGURE_AS_OUTPUT)(
  IN SUNXI_GPIO_PROTOCOL *This,
  IN UINT32 Pin
);

/**
 Set pin direction to input

 @param This                   Pointer to SUNXI_GPIO_PROTOCOL instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.

 @retval EFI_SUCCESS           Pin was configured as input.
 @retval EFI_NOT_FOUND         Requested pin was not found.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_CONFIGURE_AS_INPUT)(
  IN SUNXI_GPIO_PROTOCOL *This,
  IN UINT32 Pin
);

/**
 Set pin output level

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param Level                  Level to set, 0 for low, > 0 for high.

 @retval EFI_SUCCESS           Output level set.
 @retval EFI_NOT_FOUND         Requested pin was not found.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_SET_LEVEL)(
  IN SUNXI_GPIO_PROTOCOL *This,
  IN UINT32 Pin,
  IN UINT8 Level
);

/**
 Read pin input level

 @param This                   Pointer to SUNXI_GPIO_PPI instance.
 @param Pin                    Pin ID obtained using SUNXI_GPIO_GET_PIN.
 @param OutLevel               Variable to receive level.

 @retval EFI_SUCCESS           Output level set.
 @retval EFI_NOT_FOUND         Requested pin was not found.
 @retval EFI_INVALID_PARAMETER OutLevel is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SUNXI_GPIO_GET_LEVEL)(
  IN SUNXI_GPIO_PROTOCOL *This,
  IN UINT32 Pin,
  OUT UINT8 *OutLevel
);

struct _SUNXI_GPIO_PROTOCOL {
  SUNXI_GPIO_GET_PIN GetPin;
  SUNXI_GPIO_GET_FUNCTION GetFunction;
  SUNXI_GPIO_SET_FUNCTION SetFunction;
  SUNXI_GPIO_GET_PULL_MODE GetPullMode;
  SUNXI_GPIO_SET_PULL_MODE SetPullMode;
  SUNXI_GPIO_GET_DRIVE_STRENGTH GetDriveStrength;
  SUNXI_GPIO_SET_DRIVE_STRENGTH SetDriveStrength;
  SUNXI_GPIO_CONFIGURE_AS_OUTPUT ConfigureAsOutput;
  SUNXI_GPIO_CONFIGURE_AS_INPUT ConfigureAsInput;
  SUNXI_GPIO_SET_LEVEL SetLevel;
  SUNXI_GPIO_GET_LEVEL GetLevel;
};

extern EFI_GUID gSunxiGpioProtocolGuid;
