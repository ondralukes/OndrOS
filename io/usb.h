#ifndef USB_H
#define USB_H

#include "../kernel/types.h"
#include "console.h"
#include "pci.h"

struct usb_cap_reg {
  uint8_t caplength;
  uint8_t reserved;
  uint16_t hciver;
  uint32_t hcsparams1;
  uint32_t hcsparams2;
  uint32_t hcsparams3;
  uint32_t hccparams1;
  uint32_t dboff;
  uint32_t rtsoff;
  uint32_t hccparams2;
} __attribute__((packed));

struct usb_op_reg {
  uint32_t usbcmd;
  uint32_t usbsts;
  uint32_t pagesize;
  uint64_t reserved1;
  uint32_t dnctrl;
  uint64_t crcr;
  uint64_t dcbaap;
  uint32_t config;
}  __attribute__((packed));

struct usb_port_reg {
  uint32_t portsc;
  uint32_t portpmsc;
  uint32_t portli;
}  __attribute__((packed));

struct usb_ring_ptr {
  uint64_t dequeue;
  uint64_t enqueue;
} __attribute__((packed));

struct usb_cmd_trb {
  uint64_t reserved1;
  uint32_t reserved2;
  uint16_t flags;
  uint16_t slotType;
} __attribute__((packed));

struct usb_int {
  uint32_t iman;
  uint32_t imod;
  uint16_t erstsz;
  uint16_t reserved1;
  uint32_t reserved2;
  uint64_t erstba;
  uint64_t erdp;
} __attribute__((packed));

struct usb_erst_seg {
  uint64_t base;
  uint64_t size;
} __attribute__((packed));

struct usb_cmd_compl_trb {
  uint64_t cmdTrb;
  uint32_t compl;
  uint32_t params;
} __attribute__((packed));

union usb_trb{
  struct usb_cmd_trb cmd;
}  __attribute__((packed));

void readUSB(uint32_t bus, uint32_t slot, uint32_t func);
#endif
