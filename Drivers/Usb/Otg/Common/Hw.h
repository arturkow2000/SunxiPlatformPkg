#pragma once

/*
 * Sunxi specific registers
 */

#define USBC_REG_o_VEND0 0x0043

/*
 * Common USB registers
 */

#define MUSB_FADDR      0x0098
#define MUSB_POWER      0x0040

#define MUSB_INTRTX     0x0044
#define MUSB_INTRRX     0x0046
#define MUSB_INTRTXE    0x0048
#define MUSB_INTRRXE    0x004A
#define MUSB_INTRUSB    0x004C
#define MUSB_INTRUSBE   0x0050
#define MUSB_FRAME      0x0054
#define MUSB_INDEX      0x0042
#define MUSB_TESTMODE   0x007C

/* Get offset for a given FIFO from musb->mregs */
#define MUSB_FIFO_OFFSET(epnum) (0x00 + ((epnum) * 4))

/*
 * Additional Control Registers
 */

#define MUSB_DEVCTL     0x0041

/* These are always controlled through the INDEX register */
#define MUSB_TXFIFOSZ   0x0090
#define MUSB_RXFIFOSZ   0x0094
#define MUSB_TXFIFOADD  0x0092
#define MUSB_RXFIFOADD  0x0096

#define MUSB_EPINFO     0x0078
#define MUSB_RAMINFO    0x0079
#define MUSB_LINKINFO   0x007A
#define MUSB_VPLEN      0x007B
#define MUSB_HS_EOF1    0x007C
#define MUSB_FS_EOF1    0x007D
#define MUSB_LS_EOF1    0x007E

/* Offsets to endpoint registers */
#define MUSB_TXMAXP     0x0080
#define MUSB_TXCSR      0x0082
#define MUSB_CSR0       0x0082
#define MUSB_RXMAXP     0x0084
#define MUSB_RXCSR      0x0086
#define MUSB_RXCOUNT    0x0088
#define MUSB_COUNT0     0x0088
#define MUSB_TXTYPE     0x008C
#define MUSB_TYPE0      0x008C
#define MUSB_TXINTERVAL 0x008D
#define MUSB_NAKLIMIT0  0x008D
#define MUSB_RXTYPE     0x008E
#define MUSB_RXINTERVAL 0x008F

#define MUSB_CONFIGDATA 0x00c0
#define MUSB_FIFOSIZE   0x0090

/* Offsets to endpoint registers in indexed model (using INDEX register) */
#define MUSB_INDEXED_OFFSET(_epnum, _offset) (_offset)

#define MUSB_TXCSR_MODE 0x2000

/* "bus control"/target registers, for host side multipoint (external hubs) */
#define MUSB_TXFUNCADDR 0x0098
#define MUSB_TXHUBADDR  0x009A
#define MUSB_TXHUBPORT  0x009B

#define MUSB_RXFUNCADDR 0x009C
#define MUSB_RXHUBADDR  0x009E
#define MUSB_RXHUBPORT  0x009F

/*
 * MUSB Register bits
 */

/* POWER */
#define MUSB_POWER_ISOUPDATE 0x80
#define MUSB_POWER_SOFTCONN  0x40
#define MUSB_POWER_HSENAB    0x20
#define MUSB_POWER_HSMODE    0x10
#define MUSB_POWER_RESET     0x08
#define MUSB_POWER_RESUME    0x04
#define MUSB_POWER_SUSPENDM  0x02
#define MUSB_POWER_ENSUSPEND 0x01

/* INTRUSB */
#define MUSB_INTR_SUSPEND    0x01
#define MUSB_INTR_RESUME     0x02
#define MUSB_INTR_RESET      0x04
#define MUSB_INTR_BABBLE     0x04
#define MUSB_INTR_SOF        0x08
#define MUSB_INTR_CONNECT    0x10
#define MUSB_INTR_DISCONNECT 0x20
#define MUSB_INTR_SESSREQ    0x40
#define MUSB_INTR_VBUSERROR  0x80  /* For SESSION end */

/* DEVCTL */
#define MUSB_DEVCTL_BDEVICE  0x80
#define MUSB_DEVCTL_FSDEV    0x40
#define MUSB_DEVCTL_LSDEV    0x20
#define MUSB_DEVCTL_VBUS     0x18
#define MUSB_DEVCTL_VBUS_SHIFT  3
#define MUSB_DEVCTL_HM       0x04
#define MUSB_DEVCTL_HR       0x02
#define MUSB_DEVCTL_SESSION  0x01

/* CSR0 */
#define MUSB_CSR0_FLUSHFIFO 0x0100
#define MUSB_CSR0_TXPKTRDY  0x0002
#define MUSB_CSR0_RXPKTRDY  0x0001

/* CSR0 in Peripheral mode */
#define MUSB_CSR0_P_SVDSETUPEND 0x0080
#define MUSB_CSR0_P_SVDRXPKTRDY 0x0040
#define MUSB_CSR0_P_SENDSTALL   0x0020
#define MUSB_CSR0_P_SETUPEND    0x0010
#define MUSB_CSR0_P_DATAEND     0x0008
#define MUSB_CSR0_P_SENTSTALL   0x0004

/* CSR0 in Host mode */
#define MUSB_CSR0_H_DIS_PING      0x0800
#define MUSB_CSR0_H_WR_DATATOGGLE 0x0400  /* Set to allow setting: */
#define MUSB_CSR0_H_DATATOGGLE    0x0200  /* Data toggle control */
#define MUSB_CSR0_H_NAKTIMEOUT    0x0080
#define MUSB_CSR0_H_STATUSPKT     0x0040
#define MUSB_CSR0_H_REQPKT        0x0020
#define MUSB_CSR0_H_ERROR         0x0010
#define MUSB_CSR0_H_SETUPPKT      0x0008
#define MUSB_CSR0_H_RXSTALL       0x0004

/* CONFIGDATA */
#define MUSB_CONFIGDATA_MPRXE     0x80 /* Auto bulk pkt combining */
#define MUSB_CONFIGDATA_MPTXE     0x40 /* Auto bulk pkt splitting */
#define MUSB_CONFIGDATA_BIGENDIAN 0x20
#define MUSB_CONFIGDATA_HBRXE     0x10 /* HB-ISO for RX */
#define MUSB_CONFIGDATA_HBTXE     0x08 /* HB-ISO for TX */
#define MUSB_CONFIGDATA_DYNFIFO   0x04 /* Dynamic FIFO sizing */
#define MUSB_CONFIGDATA_SOFTCONE  0x02 /* SoftConnect */
#define MUSB_CONFIGDATA_UTMIDW    0x01 /* Data width 0/1 => 8/16bits */

/* TXCSR in Peripheral and Host mode */
#define MUSB_TXCSR_AUTOSET        0x8000
#define MUSB_TXCSR_DMAENAB        0x1000
#define MUSB_TXCSR_FRCDATATOG     0x0800
#define MUSB_TXCSR_DMAMODE        0x0400
#define MUSB_TXCSR_CLRDATATOG     0x0040
#define MUSB_TXCSR_FLUSHFIFO      0x0008
#define MUSB_TXCSR_FIFONOTEMPTY   0x0002
#define MUSB_TXCSR_TXPKTRDY       0x0001

/* TXCSR in Peripheral mode */
#define MUSB_TXCSR_P_ISO          0x4000
#define MUSB_TXCSR_P_INCOMPTX     0x0080
#define MUSB_TXCSR_P_SENTSTALL    0x0020
#define MUSB_TXCSR_P_SENDSTALL    0x0010
#define MUSB_TXCSR_P_UNDERRUN     0x0004

/* TXCSR in Host mode */
#define MUSB_TXCSR_H_WR_DATATOGGLE  0x0200
#define MUSB_TXCSR_H_DATATOGGLE     0x0100
#define MUSB_TXCSR_H_NAKTIMEOUT     0x0080
#define MUSB_TXCSR_H_RXSTALL        0x0020
#define MUSB_TXCSR_H_ERROR          0x0004

/* TXCSR bits to avoid zeroing (write zero clears, write 1 ignored) */
#define MUSB_TXCSR_P_WZC_BITS \
  (MUSB_TXCSR_P_INCOMPTX | MUSB_TXCSR_P_SENTSTALL \
  | MUSB_TXCSR_P_UNDERRUN | MUSB_TXCSR_FIFONOTEMPTY)
#define MUSB_TXCSR_H_WZC_BITS \
  (MUSB_TXCSR_H_NAKTIMEOUT | MUSB_TXCSR_H_RXSTALL \
  | MUSB_TXCSR_H_ERROR | MUSB_TXCSR_FIFONOTEMPTY)

/* RXCSR in Peripheral and Host mode */
#define MUSB_RXCSR_AUTOCLEAR  0x8000
#define MUSB_RXCSR_DMAENAB    0x2000
#define MUSB_RXCSR_DISNYET    0x1000
#define MUSB_RXCSR_PID_ERR    0x1000
#define MUSB_RXCSR_DMAMODE    0x0800
#define MUSB_RXCSR_INCOMPRX   0x0100
#define MUSB_RXCSR_CLRDATATOG 0x0080
#define MUSB_RXCSR_FLUSHFIFO  0x0010
#define MUSB_RXCSR_DATAERROR  0x0008
#define MUSB_RXCSR_FIFOFULL   0x0002
#define MUSB_RXCSR_RXPKTRDY   0x0001

/* RXCSR in Peripheral mode */
#define MUSB_RXCSR_P_ISO        0x4000
#define MUSB_RXCSR_P_SENTSTALL  0x0040
#define MUSB_RXCSR_P_SENDSTALL  0x0020
#define MUSB_RXCSR_P_OVERRUN    0x0004

/* RXCSR in Host mode */
#define MUSB_RXCSR_H_AUTOREQ        0x4000
#define MUSB_RXCSR_H_WR_DATATOGGLE  0x0400
#define MUSB_RXCSR_H_DATATOGGLE     0x0200
#define MUSB_RXCSR_H_RXSTALL        0x0040
#define MUSB_RXCSR_H_REQPKT         0x0020
#define MUSB_RXCSR_H_ERROR          0x0004

/* RXCSR bits to avoid zeroing (write zero clears, write 1 ignored) */
#define MUSB_RXCSR_P_WZC_BITS \
  (MUSB_RXCSR_P_SENTSTALL | MUSB_RXCSR_P_OVERRUN \
  | MUSB_RXCSR_RXPKTRDY)
#define MUSB_RXCSR_H_WZC_BITS \
  (MUSB_RXCSR_H_RXSTALL | MUSB_RXCSR_H_ERROR \
  | MUSB_RXCSR_DATAERROR | MUSB_RXCSR_RXPKTRDY)

inline VOID UsbSelectEndpoint(USB_DRIVER *Driver, UINT8 Endpoint) {
  MmioWrite8(Driver->Base + MUSB_INDEX, Endpoint);
}
