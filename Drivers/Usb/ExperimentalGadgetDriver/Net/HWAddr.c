#include "../Driver.h"

STATIC CONST UINT8 MacAddress[6] = {0xAC, 0xAB, 0xCD, 0xde, 0xad, 0xF0};

UINT8* NetGetMacAddress() {
  return (UINT8*)MacAddress;
}
