#pragma once

#include <Uefi.h>

#define RNDIS_ENC_MAX 512

#define RNDIS_STATUS_SUCCESS            0x00000000
#define RNDIS_STATUS_FAILURE            0xC0000001
#define RNDIS_STATUS_INVALID_DATA       0xC0010015
#define RNDIS_STATUS_NOT_SUPPORTED      0xC00000BB
#define RNDIS_STATUS_MEDIA_CONNECT      0x4001000B
#define RNDIS_STATUS_MEDIA_DISCONNECT   0x4001000C

#define REMOTE_NDIS_PACKET_MSG          0x00000001
#define REMOTE_NDIS_INITIALIZE_MSG      0x00000002
#define REMOTE_NDIS_HALT_MSG            0x00000003
#define REMOTE_NDIS_QUERY_MSG           0x00000004
#define REMOTE_NDIS_SET_MSG             0x00000005
#define REMOTE_NDIS_RESET_MSG           0x00000006
#define REMOTE_NDIS_INDICATE_STATUS_MSG 0x00000007
#define REMOTE_NDIS_KEEPALIVE_MSG       0x00000008
#define REMOTE_NDIS_INITIALIZE_CMPLT    0x80000002
#define REMOTE_NDIS_QUERY_CMPLT         0x80000004
#define REMOTE_NDIS_SET_CMPLT           0x80000005
#define REMOTE_NDIS_RESET_CMPLT         0x80000006
#define REMOTE_NDIS_KEEPALIVE_CMPLT     0x80000008

#define RNDIS_DF_CONNECTIONLESS         0x00000001
#define RNDIS_DF_CONNECTION_ORIENTED    0x00000002

#define RNDIS_MEDIUM_802_3              0x00000000

// Required OIDs
#define OID_GEN_SUPPORTED_LIST          0x00010101
#define OID_GEN_HARDWARE_STATUS         0x00010102
#define OID_GEN_MEDIA_SUPPORTED         0x00010103
#define OID_GEN_MEDIA_IN_USE            0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD       0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE      0x00010106
#define OID_GEN_LINK_SPEED              0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE   0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE    0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE     0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE      0x0001010B
#define OID_GEN_VENDOR_ID               0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION      0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER   0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD       0x0001010F
#define OID_GEN_DRIVER_VERSION          0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE      0x00010111
#define OID_GEN_PROTOCOL_OPTIONS        0x00010112
#define OID_GEN_MAC_OPTIONS             0x00010113
#define OID_GEN_MEDIA_CONNECT_STATUS    0x00010114
#define OID_GEN_MAXIMUM_SEND_PACKETS    0x00010115

#define OID_GEN_VENDOR_DRIVER_VERSION   0x00010116
#define OID_GEN_SUPPORTED_GUIDS         0x00010117
#define OID_GEN_NETWORK_LAYER_ADDRESSES 0x00010118
#define OID_GEN_TRANSPORT_HEADER_OFFSET 0x00010119

#define OID_802_3_PERMANENT_ADDRESS     0x01010101
#define OID_802_3_CURRENT_ADDRESS       0x01010102
#define OID_802_3_MULTICAST_LIST        0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE     0x01010104
#define OID_802_3_MAC_OPTIONS           0x01010105

typedef enum
{
  NdisHardwareStatusReady,
  NdisHardwareStatusInitializing,
  NdisHardwareStatusReset,
  NdisHardwareStatusClosing,
  NdisHardwareStatusNotReady
} NDIS_HARDWARE_STATUS;

typedef enum
{
  NdisMediaStateConnected,
  NdisMediaStateDisconnected
} NDIS_MEDIA_STATE;

typedef struct {
  UINT32 Type;
  UINT32 Length;
} RNDIS_MESSAGE_HEADER;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
  UINT32 Major;
  UINT32 Minor;
  UINT32 MaxTransferSize;
} RNDIS_INITIALIZE_MSG;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
  UINT32 Status;
  UINT32 Major;
  UINT32 Minor;
  UINT32 DeviceFlags;
  UINT32 Medium;
  UINT32 MaxPacketsPerMessage;
  UINT32 MaxTransferSize;
  UINT32 PacketAlignmentFactor;
  UINT32 AFListOffset;
  UINT32 AFListSize;
} RNDIS_INITIALIZE_CMPLT;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
  UINT32 Oid;
  UINT32 InformationBufferLength;
  UINT32 InformationBufferOffset;
  UINT32 DeviceVcHandle;
} RNDIS_QUERY_MSG;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
  UINT32 Status;
  UINT32 InformationBufferLength;
  UINT32 InformationBufferOffset;
} RNDIS_QUERY_CMPLT;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
  UINT32 Oid;
  UINT32 InformationBufferLength;
  UINT32 InformationBufferOffset;
  UINT32 DeviceVcHandle;
} RNDIS_SET_MSG;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
  UINT32 Status;
} RNDIS_SET_CMPLT;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 Reserved;
} RNDIS_RESET_MSG;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 Status;
  UINT32 AddressingReset;
} RNDIS_RESET_CMPLT;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
} RNDIS_KEEPALIVE_MSG;

typedef struct {
  RNDIS_MESSAGE_HEADER Header;
  UINT32 RequestId;
  UINT32 Status;
} RNDIS_KEEPALIVE_CMPLT;

typedef struct {
  UINT32 MessageType;
  UINT32 MessageLength;
  UINT32 DataOffset;
  UINT32 DataLength;
  UINT32 OOBDataOffset;
  UINT32 OOBDataLength;
  UINT32 NumOOBDataElements;
  UINT32 PerPacketInfoOffset;
  UINT32 PerPacketInfoLength;
  UINT32 DeviceVcHandle;
  UINT32 Reserved;
} RNDIS_DATA_HEADER;
