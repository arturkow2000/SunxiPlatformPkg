#pragma once

#define USB_PPI_GUID                                                                \
  {                                                                                 \
    0xcf9c73d1, 0x24f6, 0x458c, { 0xb3, 0x50, 0x11, 0x4c, 0xa7, 0x8d, 0xf9, 0xe0 }  \
  }

typedef struct _USB_PPI USB_PPI;
typedef struct _USB_GADGET_DRIVER USB_GADGET_DRIVER;
typedef struct _USB_REQUEST_BLOCK USB_REQUEST_BLOCK;

typedef
VOID
(EFIAPI *USB_PPI_REQ_COMPLETE_CALLBACK)(
  USB_PPI *Usb,
  UINT32 Endpoint,
  VOID *Buffer,
  UINT32 Length,
  VOID *UserData,
  EFI_STATUS Status
);

/**
 Registers gadget driver and enables USB.
 Only single gadget driver is supported.

 @param This                        Pointer to USB_PPI instance
 @param Gadget                      Pointer to USB_GADGET_DRIVER instance
 @param UserData                    Pointer to user defined data to pass to callbacks

 @retval EFI_SUCCESS                Driver successfully registered and USB started.
 @retval EFI_ALREADY_STARTED        Another driver was registered already.
 @retval EFI_DEVICE_ERROR           Failed to enable USB.
 @retval EFI_INVALID_PARAMETER      This or Gadget is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_REGISTER_GADGET_DRIVER)(
  IN USB_PPI *This,
  IN USB_GADGET_DRIVER *Gadget,
  IN VOID *UserData
);

/**
 Unregisters gadget driver and disables USB.

 @param This                        Pointer to USB_PPI instance
 @param Gadget                      Pointer to USB_GADGET_DRIVER instance as used in call to USB_PPI_REGISTER_GADGET_DRIVER

 @retval EFI_SUCCESS                Driver unregistered, USB stopped.
 @retval EFI_NOT_FOUND              Driver is not registered.
 @retval EFI_INVALID_PARAMETER      This or Gadget is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_UNREGISTER_GADGET_DRIVER)(
  IN USB_PPI *This,
  IN USB_GADGET_DRIVER *Gadget
);

/**
 Triggers interrupt handler to poll for interrupts and perform data transfers.

 This is used because in PEI phase we have no timer interrupts.

 @param This                        Pointer to USB_PPI instance

 @retval EFI_SUCCESS                Interrupt handler done.
 @retval EFI_INVALID_PARAMETER      This is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_HANDLE_INTERRUPTS)(
  IN USB_PPI *This
);

/**
 Returns number of available endpoints, including EP0.

 @param This                        Pointer to USB_PPI instance
 @param OutNumEndpoints             Pointer to variable to receive number of endpoints available

 @retval EFI_SUCCESS                Number of endpointer returned.
 @retval EFI_INVALID_PARAMETER      This is NULL or OutNumEndpoints is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_GET_NUMBER_OF_ENDPOINTS)(
  IN USB_PPI *This,
  OUT UINT32 *OutNumEndpoints
);

/**
 Retrieves endpoint information, currently maximum packet size only.

 @param This                        Pointer to USB_PPI instance
 @param Endpoint                    Number of endpoint.
 @param OutMaxPacketSizeTx          Pointer to variable to receive maximum packet size we are able to send.
 @param OutMaxPacketSizeRx          Pointer to variable to receive maximum packet size we are able to receive.

 @retval EFI_SUCCESS                Max packet size returned
 @retval EFI_INVALID_PARAMETER      This is NULL or both OutMaxPacketSizeTx and OutMaxPacketSizeRx are NULL
 @retval EFI_NOT_FOUND              Endpoint not found.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_GET_ENDPOINT_INFO)(
  IN USB_PPI *This,
  IN UINT32 Endpoint,
  OUT UINT32 *OutMaxPacketSizeTx,
  OUT UINT32 *OutMaxPacketSizeRx
);

/**
 Allocates driver's internal structures for keeping track of queued transfers.
 These are initialized using USB_PPI_INIT_URB and then passed to
 USB_PPI_QUEUE

 @param This                        Pointer to USB_PPI instance
 @param NumUrbs                     Number of URB structures to allocate
 @param OutUrbs                     Variable to receive pointer to structure array

 @retval EFI_SUCCESS                Allocation successful.
 @retval EFI_INVALID_PARAMETER      This == NULL or OutUrbs == NULL or NumUrbs == 0
 @retval EFI_OUT_OF_RESOURCES       Out of memory.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_ALLOCATE_URBS)(
  IN USB_PPI *This,
  IN UINT32 NumUrbs,
  OUT USB_REQUEST_BLOCK **OutUrbs
);

#define USB_PPI_FLAG_ZLP (1 << 0)
#define USB_PPI_FLAG_TX (1 << 1)

/**
 Initializes URB previously allocated with USB_PPI_ALLOCATE_URBS.

 @param This                        Pointer to USB_PPI instance
 @param Urb                         Pointer to USB_REQUEST_BLOCK structure allocated with USB_PPI_ALLOCATE_URBS
 @param Buffer                      Pointer to packet data buffer
 @param Length                      Length of data in buffer
 @param Flags                       Reserved, should be 0
 @param Callback                    Callback function called upon completion
 @param UserData                    Optional data passed to callback

 @retval EFI_SUCCESS                Pointer to USB_PPI instance.
 @retval EFI_INVALID_PARAMETER      At least one of parameters is invalid:
                                     - This or Urb is NULL 
                                     - Buffer is NULL or Length == 0, currently zero length packets are not supported
                                     - Invalid flag passed
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_INIT_URB)(
  IN USB_PPI *This,
  OUT USB_REQUEST_BLOCK *Urb,
  IN VOID *Buffer,
  IN UINT32 Length,
  IN UINT32 Flags,
  IN USB_PPI_REQ_COMPLETE_CALLBACK Callback OPTIONAL,
  IN VOID *UserData OPTIONAL
);

/**
 Queues control packet on EP0, this accepts pointer to USB_REQUEST_BLOCK initialized with
 USB_PPI.

 @param This                        Pointer to USB_PPI instance.
 @param Endpoint                    Endpoint to queue onto.
 @param Urb                         Pointer to USB_REQUEST_BLOCK structure.

 @retval EFI_SUCCESS                Packet queued.
 @retval EFI_INVALID_PARAMETER      This or Urb is NULL, Urb not initialized, or endpoint not enabled.
 @retval EFI_NOT_FOUND              No such endpoint.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_QUEUE)(
  IN USB_PPI *This,
  IN UINT32 Endpoint,
  IN USB_REQUEST_BLOCK *Urb
);

/**
 Halts endpoint

 @param This                        Pointer to USB_PPI instance.
 @param Endpoint                    Number of endpoint to halt.

 @retval EFI_SUCCESS                Endpoint halted.
 @retval EFI_INVALID_PARAMETER      This is NULL.
 @retval EFI_NOT_FOUND              Endpoint not found.
 @retval EFI_DEVICE_ERROR           Unknown internal device or driver error.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_HALT)(
  IN USB_PPI *This,
  IN UINT32 Endpoint
);

/**
 Enables endpoint

 @param This                        Pointer to USB_PPI instance.
 @param Descriptor                  Pointer to USB_ENDPOINT_DESCRIPTOR

 @retval EFI_SUCCESS                Endpoint enabled.
 @retval EFI_INVALID_PARAMETER      This is NULL, Descriptor is NULL or invalid
 @retval EFI_NOT_FOUND              No such Endpoint.
 @retval EFI_DEVICE_ERROR           Unknown internal error.
 @retval EFI_ALREADY_STARTED        Endpoint is already used.
**/
typedef
EFI_STATUS
(EFIAPI *USB_PPI_ENABLE_ENDPOINT)(
  IN USB_PPI *This,
  IN USB_ENDPOINT_DESCRIPTOR *Descriptor
);

struct _USB_PPI {
  USB_PPI_REGISTER_GADGET_DRIVER RegisterGadgetDriver;
  USB_PPI_UNREGISTER_GADGET_DRIVER UnregisterGadgetDriver;
  USB_PPI_HANDLE_INTERRUPTS HandleInterrupts;
  USB_PPI_GET_NUMBER_OF_ENDPOINTS GetNumberOfEndpoints;
  USB_PPI_GET_ENDPOINT_INFO GetEndpointInfo;
  USB_PPI_ALLOCATE_URBS AllocateUrbs;
  USB_PPI_INIT_URB InitUrb;
  USB_PPI_QUEUE Queue;
  USB_PPI_HALT Halt;
  USB_PPI_ENABLE_ENDPOINT EnableEndpoint;
};

extern EFI_GUID gUsbPpiGuid;
