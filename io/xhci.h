#ifndef XHCI_H
#define XHCI_H

#include <stddef.h>
#include <stdbool.h>

#include "../kernel/types.h"
#include "console.h"
#include "pci.h"

struct trb_ring{
  uint8_t pcs;
  uint8_t ccs;
  union xhci_trb* dp;
  union xhci_trb* ep;
  union xhci_trb* base;
} __attribute__((packed));

struct xhci_cap_reg {
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

struct xhci_op_reg {
  uint32_t xhcicmd;
  uint32_t xhcists;
  uint32_t pagesize;
  uint64_t reserved1;
  uint32_t dnctrl;
  uint64_t crcr;
  uint64_t reserved2[2];
  uint64_t dcbaap;
  uint32_t config;
}  __attribute__((packed));

struct xhci_port_reg {
  uint32_t portsc;
  uint32_t portpmsc;
  uint32_t portli;
}  __attribute__((packed));

struct xhci_cmd_trb {
  uint64_t pointer;
  uint32_t reserved;
  uint32_t params;
} __attribute__((packed));

struct xhci_int {
  uint32_t iman;
  uint32_t imod;
  uint16_t erstsz;
  uint16_t reserved1;
  uint32_t reserved2;
  uint64_t erstba;
  uint64_t erdp;
} __attribute__((packed));

struct xhci_erst_seg {
  uint64_t base;
  uint64_t size;
} __attribute__((packed));

struct xhci_cmd_compl_trb {
  uint64_t cmdTrb;
  uint32_t compl;
  uint32_t params;
} __attribute__((packed));

struct xhci_slot_context {
  uint32_t slotdw0;
  uint32_t slotdw1;
  uint32_t slotdw2;
  uint32_t slotdw3;
  uint32_t slotdw4;
  uint32_t slotdw5;
  uint32_t slotdw6;
  uint32_t slotdw7;
} __attribute__((packed));

struct xhci_ep_context {
  uint32_t epdw0;
  uint32_t epdw1;
  uint64_t epqw2;
  uint32_t epdw4;

  uint32_t epdw5;
  uint32_t epdw6;
  uint32_t epdw7;
} __attribute__((packed));

struct xhci_dev_in_ctrl_context {
  uint32_t dropFlags;
  uint32_t addFlags;
  uint32_t dummy[6];
} __attribute__((packed));

struct xhci_dev_context {
   struct xhci_slot_context slotContext;
   struct xhci_ep_context epContexts[31];
} __attribute__((packed));

struct xhci_dev_in_context {
   struct xhci_dev_in_ctrl_context inputCtrl;
   struct xhci_dev_context devContext;
} __attribute__((packed));

union xhci_trb{
   struct xhci_cmd_trb cmd;
   struct xhci_cmd_compl_trb cmdCompl;
} __attribute__((packed));

struct xhci{
  bool ac64;
  uint8_t maxSlots;
  uint8_t maxPorts;
  struct xhci_cap_reg* capRegs;
  struct xhci_op_reg* opRegs;
  struct trb_ring* commandRing;
  struct trb_ring* eventRing;
  uint64_t * dcbaa;
};

struct xhci_dev{
  uint16_t slot;
  uint16_t port;
  struct xhci_dev_in_context * inContext;
  struct xhci_dev_context * outContext;
};

struct trb_ring * createRing(uint64_t size);
void destroyRing(struct trb_ring * ring);
union xhci_trb* ringDequeue(struct trb_ring * ring);

void checkXHCI(uint32_t pci_bus, uint32_t pci_slot, uint32_t pci_func);

void xhci_write64(struct xhci *xhci, uint64_t * dest, uint64_t val);

void xhci_reset(struct xhci* xhci);

void xhci_scan(struct xhci* xhci);

void xhci_dev_init(struct xhci* xhci, uint8_t port);

bool xhci_read_cmd_out(struct xhci* xhci, struct xhci_cmd_compl_trb** out);

uint8_t xhci_get_portSpeed(struct xhci* xhci, uint8_t port);

void xhci_ring_host(struct xhci* xhci);
#endif
