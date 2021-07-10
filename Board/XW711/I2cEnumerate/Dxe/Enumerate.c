#include "Driver.h"

/**
  Enumerate the I2C devices

  This function enables the caller to traverse the set of I2C devices
  on an I2C bus.

  @param[in]  This              The platform data for the next device on
                                the I2C bus was returned successfully.
  @param[in, out] Device        Pointer to a buffer containing an
                                EFI_I2C_DEVICE structure.  Enumeration is
                                started by setting the initial EFI_I2C_DEVICE
                                structure pointer to NULL.  The buffer
                                receives an EFI_I2C_DEVICE structure pointer
                                to the next I2C device.

  @retval EFI_SUCCESS           The platform data for the next device on
                                the I2C bus was returned successfully.
  @retval EFI_INVALID_PARAMETER Device is NULL
  @retval EFI_NO_MAPPING        *Device does not point to a valid
                                EFI_I2C_DEVICE structure returned in a
                                previous call Enumerate().

**/
STATIC
EFI_STATUS
EFIAPI
Enumerate(
  IN CONST EFI_I2C_ENUMERATE_PROTOCOL *This,
  IN OUT CONST EFI_I2C_DEVICE         **Device
  )
{
  I2C_ENUMERATOR_DRIVER *Driver;
  I2C_DEVICE *I2cDevice;

  Driver = (I2C_ENUMERATOR_DRIVER*)This;

  if (Device == NULL)
    return EFI_INVALID_PARAMETER;

  if (*Device == NULL) {
    *Device = &Driver->Head->Device;
    return EFI_SUCCESS;
  }
  
  I2cDevice = EFI_I2C_DEVICE_TO_I2C_DEVICE(*Device);
  if (I2cDevice->Signature != I2C_DEVICE_SIGNATURE)
    return EFI_NO_MAPPING;

  if (I2cDevice->Next)
    *Device = &I2cDevice->Next->Device;
  else
    *Device = NULL;

  return EFI_SUCCESS;
}

/**
  Get the requested I2C bus frequency for a specified bus configuration.

  This function returns the requested I2C bus clock frequency for the
  I2cBusConfiguration.  This routine is provided for diagnostic purposes
  and is meant to be called after calling Enumerate to get the
  I2cBusConfiguration value.

  @param[in] This                 Pointer to an EFI_I2C_ENUMERATE_PROTOCOL
                                  structure.
  @param[in] I2cBusConfiguration  I2C bus configuration to access the I2C
                                  device
  @param[out] *BusClockHertz      Pointer to a buffer to receive the I2C
                                  bus clock frequency in Hertz

  @retval EFI_SUCCESS           The I2C bus frequency was returned
                                successfully.
  @retval EFI_INVALID_PARAMETER BusClockHertz was NULL
  @retval EFI_NO_MAPPING        Invalid I2cBusConfiguration value

**/
STATIC
EFI_STATUS
EFIAPI
GetBusFrequency(
  IN CONST EFI_I2C_ENUMERATE_PROTOCOL *This,
  IN UINTN                            I2cBusConfiguration,
  OUT UINTN                           *BusClockHertz
  )
{
  // TODO: implement
  ASSERT(0);
  return EFI_DEVICE_ERROR;
}

EFI_I2C_ENUMERATE_PROTOCOL gI2cEnumerateProtocol = {
  Enumerate,
  GetBusFrequency
};
