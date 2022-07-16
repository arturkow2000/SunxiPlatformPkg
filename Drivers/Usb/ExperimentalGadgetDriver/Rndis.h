#pragma once

#include <Uefi.h>

#define RNDIS_ENC_MAX 512

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
