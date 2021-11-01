/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#include "Driver.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

/**
  Returns information for an available graphics mode that the graphics device
  and the set of active video output devices supports.

  @param  This                  The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param  ModeNumber            The mode number to return information on.
  @param  SizeOfInfo            A pointer to the size, in bytes, of the Info buffer.
  @param  Info                  A pointer to callee allocated buffer that returns information about ModeNumber.

  @retval EFI_SUCCESS           Valid mode information was returned.
  @retval EFI_DEVICE_ERROR      A hardware error occurred trying to retrieve the video mode.
  @retval EFI_INVALID_PARAMETER ModeNumber is not valid.

**/
STATIC EFI_STATUS EFIAPI DisplayQueryMode(
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **OutInfo
  )
{
  SUNXI_DISPLAY_DRIVER *Driver = GOP_TO_DISPLAY_DRIVER(This);
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;

  if (!This || ModeNumber != 0)
    return EFI_INVALID_PARAMETER;

  if (OutInfo) {
    Info = AllocateCopyPool(sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION), &Driver->GopModeInfo);
    if (!Info)
      return EFI_OUT_OF_RESOURCES;

    *OutInfo = Info;
  }

  if (SizeOfInfo)
    *SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);

  return EFI_SUCCESS;
}

/**
  Set the video device into the specified mode and clears the visible portions of
  the output display to black.

  @param  This              The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param  ModeNumber        Abstraction that defines the current video mode.

  @retval EFI_SUCCESS       The graphics mode specified by ModeNumber was selected.
  @retval EFI_DEVICE_ERROR  The device had an error and could not complete the request.
  @retval EFI_UNSUPPORTED   ModeNumber is not supported by this device.

**/
STATIC EFI_STATUS EFIAPI DisplaySetMode(
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  IN  UINT32                       ModeNumber
  )
{
  if (!This)
    return EFI_INVALID_PARAMETER;

  if (ModeNumber == 0)
    return EFI_SUCCESS;

  return EFI_UNSUPPORTED;
}

/**
  Blt a rectangle of pixels on the graphics screen. Blt stands for BLock Transfer.

  @param  This         Protocol instance pointer.
  @param  BltBuffer    The data to transfer to the graphics screen.
                       Size is at least Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL).
  @param  BltOperation The operation to perform when copying BltBuffer on to the graphics screen.
  @param  SourceX      The X coordinate of source for the BltOperation.
  @param  SourceY      The Y coordinate of source for the BltOperation.
  @param  DestinationX The X coordinate of destination for the BltOperation.
  @param  DestinationY The Y coordinate of destination for the BltOperation.
  @param  Width        The width of a rectangle in the blt rectangle in pixels.
  @param  Height       The height of a rectangle in the blt rectangle in pixels.
  @param  Delta        Not used for EfiBltVideoFill or the EfiBltVideoToVideo operation.
                       If a Delta of zero is used, the entire BltBuffer is being operated on.
                       If a subrectangle of the BltBuffer is being used then Delta
                       represents the number of bytes in a row of the BltBuffer.

  @retval EFI_SUCCESS           BltBuffer was drawn to the graphics screen.
  @retval EFI_INVALID_PARAMETER BltOperation is not valid.
  @retval EFI_DEVICE_ERROR      The device had an error and could not complete the request.

**/
STATIC EFI_STATUS EFIAPI DisplayBlt(
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer,   OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
  IN  UINTN                                   SourceX,
  IN  UINTN                                   SourceY,
  IN  UINTN                                   DestinationX,
  IN  UINTN                                   DestinationY,
  IN  UINTN                                   Width,
  IN  UINTN                                   Height,
  IN  UINTN                                   Delta         OPTIONAL
  )
{
  UINT8 *VidBuf, *BltBuf, *VidBuf1;
  UINT32 i;

  if (!This)
    return EFI_INVALID_PARAMETER;

  switch(BltOperation) {
    case EfiBltVideoFill:
      BltBuf = (UINT8 *)BltBuffer;
      for(i = 0; i < Height; i++) {
        VidBuf = (UINT8 *)((UINT32)This->Mode->FrameBufferBase + \
          (DestinationY + i) * This->Mode->Info->PixelsPerScanLine * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) + \
          DestinationX * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        gBS->SetMem((VOID *)VidBuf, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width, *BltBuf);
      }
      break;

    case EfiBltVideoToBltBuffer:
      if(Delta == 0)
        Delta = Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

      for(i = 0; i < Height; i++) {
        VidBuf = (UINT8 *)((UINT32)This->Mode->FrameBufferBase + \
          (SourceY + i) * This->Mode->Info->PixelsPerScanLine * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) + \
          SourceX * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        BltBuf = (UINT8 *)((UINT32)BltBuffer + (DestinationY + i) * Delta + DestinationX * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        gBS->CopyMem((VOID *)BltBuf, (VOID *)VidBuf, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
      }
      break;

    case EfiBltBufferToVideo:
      if(Delta == 0)
        Delta = Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

      for(i = 0; i < Height; i++) {
        VidBuf = (UINT8 *)((UINT32)This->Mode->FrameBufferBase + \
          (DestinationY + i) * This->Mode->Info->PixelsPerScanLine * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) + \
          DestinationX * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        BltBuf = (UINT8 *)((UINT32)BltBuffer + (SourceY + i) * Delta + SourceX * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        gBS->CopyMem((VOID *)VidBuf, (VOID *)BltBuf, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
      }
      break;

    case EfiBltVideoToVideo:
      for(i = 0; i < Height; i++) {
        VidBuf = (UINT8 *)((UINT32)This->Mode->FrameBufferBase + \
          (SourceY + i) * This->Mode->Info->PixelsPerScanLine * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) + \
          SourceX * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

        VidBuf1 = (UINT8 *)((UINT32)This->Mode->FrameBufferBase + \
          (DestinationY + i) * This->Mode->Info->PixelsPerScanLine * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) + \
          DestinationX * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        gBS->CopyMem((VOID *)VidBuf1, (VOID *)VidBuf, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
      }
      break;

    default:
      break;
  }

  return EFI_SUCCESS;
}

typedef struct {
    VENDOR_DEVICE_PATH DisplayDevicePath;
    EFI_DEVICE_PATH EndDevicePath;
} DISPLAY_DEVICE_PATH;

STATIC DISPLAY_DEVICE_PATH mDisplayDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
      }
    },
    EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID
  },
  { 
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      sizeof(EFI_DEVICE_PATH_PROTOCOL),
      0
    }
  }
};

EFI_STATUS SunxiDisplayInitGop(SUNXI_DISPLAY_DRIVER *Driver, VOID *Framebuffer, SUNXI_VIDEO_MODE *Mode) {
  EFI_STATUS Status;

  Driver->GraphicsOutputProtocol.QueryMode = DisplayQueryMode;
  Driver->GraphicsOutputProtocol.SetMode = DisplaySetMode;
  Driver->GraphicsOutputProtocol.Blt = DisplayBlt;
  Driver->GraphicsOutputProtocol.Mode = &Driver->GopMode;

  Driver->GopMode.FrameBufferBase = (EFI_PHYSICAL_ADDRESS)(UINT32)Framebuffer;
  Driver->GopMode.FrameBufferSize = Mode->Width * 4 * Mode->Height;
  Driver->GopMode.MaxMode = 1;
  Driver->GopMode.Mode = 0;
  Driver->GopMode.Info = &Driver->GopModeInfo;
  Driver->GopMode.SizeOfInfo = sizeof(Driver->GopModeInfo);
  
  Driver->GopModeInfo.Version = 0;
  Driver->GopModeInfo.HorizontalResolution = Mode->Width;
  Driver->GopModeInfo.VerticalResolution = Mode->Height;
  Driver->GopModeInfo.PixelsPerScanLine = Mode->Width;
  Driver->GopModeInfo.PixelFormat = PixelRedGreenBlueReserved8BitPerColor;
  Driver->GopModeInfo.PixelInformation.ReservedMask = 0xFF000000;
  Driver->GopModeInfo.PixelInformation.RedMask = 0x00FF0000;
  Driver->GopModeInfo.PixelInformation.GreenMask = 0x0000FF00;
  Driver->GopModeInfo.PixelInformation.BlueMask = 0x000000FF;

  Driver->GopDevicePath = (EFI_DEVICE_PATH_PROTOCOL*)&mDisplayDevicePath;

  Driver->GopHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces(
    &Driver->GopHandle,
    &gEfiDevicePathProtocolGuid, Driver->GopDevicePath,
    &gEfiGraphicsOutputProtocolGuid, &Driver->GraphicsOutputProtocol,
    NULL
  );
  
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "GOP installation failed: %r\n", Status));
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  return EFI_SUCCESS;
}
