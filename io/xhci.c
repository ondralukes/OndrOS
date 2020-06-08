#include "xhci.h"

struct trb_ring * createRing(uint64_t size){
  struct trb_ring * ring = malloc(sizeof(struct trb_ring));
  ring->base = mallocAligned(sizeof(union xhci_trb)*size, 64);
  memset(ring->base, 0, sizeof(union xhci_trb)*size);
  ring->ep = ring->base;
  ring->dp = ring->base;
  ring->pcs = 1;
  ring->ccs = 1;
  ring->base[size-1].cmd.params = (1 << 1) | (1 << 4) | (6 << 10);
  ring->base[size-1].cmd.pointer = (uint64_t)ring->base;
  return ring;
}

void destroyRing(struct trb_ring * ring){
  free(ring->base);
  free(ring);
}

union xhci_trb* ringDequeue(struct trb_ring * ring){
  //Check if ring is empty
  if((ring->dp->cmd.params & 1) != ring->ccs){
    return NULL;
  }

  return ring->dp++;
}

bool xhci_read_cmd_out(struct xhci* xhci, struct xhci_cmd_compl_trb** out){
  union xhci_trb * eventTrb;
  while(((eventTrb->cmd.params >> 10) & 0b111111) != 33){
    while((eventTrb = ringDequeue(xhci->eventRing)) == NULL);
  }

  struct xhci_cmd_compl_trb* event = (struct xhci_cmd_compl_trb*)eventTrb;

  uint64_t completionCode = event->compl >> 24;
  uint64_t slotId = event->params >> 24;

  print("Command completion code ");
  printNum(completionCode);
  print("\n");

  *out = event;
  return completionCode == 1;
}

void checkXHCI(uint32_t pci_bus, uint32_t pci_slot, uint32_t pci_func){
  //Check if device is XHCI and is supported
  uint8_t progIf = pciReadWordConfig(pci_bus, pci_slot, pci_func, 8) >> 8;
  if(progIf != 0x30){
    print("Only XHCI controllers are supported\n");
    return;
  }

  uint8_t headerType = pciReadWordConfig(pci_bus, pci_slot, pci_func, 14) & 0x7f;
  if(headerType != 0x0){
    print("Invalid header type (");
    printHex(headerType);
    print(")\n");
    return;
  }

  struct xhci xhci;

  uint32_t bar0 = pciReadDWordConfig(pci_bus, pci_slot, pci_func, 0x10);
  uint32_t bar1 = pciReadDWordConfig(pci_bus, pci_slot, pci_func, 0x14);

  uint64_t bar = (bar0 & 0xfffffff0) | ((uint64_t)(bar1 & 0xffffffff) << 32);

  pciWriteDWordConfig(pci_bus, pci_slot, pci_func, 0x10, 0xffffffff);
  pciWriteDWordConfig(pci_bus, pci_slot, pci_func, 0x14, 0xffffffff);

  uint32_t barsz0 = pciReadDWordConfig(pci_bus, pci_slot, pci_func, 0x10);
  uint32_t barsz1 = pciReadDWordConfig(pci_bus, pci_slot, pci_func, 0x14);

  pciWriteDWordConfig(pci_bus, pci_slot, pci_func, 0x10, bar0);
  pciWriteDWordConfig(pci_bus, pci_slot, pci_func, 0x14, bar1);

  barsz0 &= ~0b1111;
  barsz0 = ~barsz0;
  barsz0++;

  barsz1 = ~barsz1;

  xhci.capRegs = (struct xhci_cap_reg *) bar;
  xhci.opRegs = (struct xhci_op_reg *) (bar + xhci.capRegs->caplength);

  uint8_t csz = (xhci.capRegs->hccparams1 >> 2)&1;
  if(csz != 0){
    print("CSZ=1 is not yet implemented!\n");
    return;
  }

  xhci.ac64 = (xhci.capRegs->hccparams1&1) == 1;

  xhci.maxPorts = (xhci.capRegs->hcsparams1 >> 24) & 0xff;
  xhci.maxSlots = xhci.capRegs->hcsparams2 & 0xff;
  xhci_reset(&xhci);

  print("Creating DCBAA\n");
  uint64_t dcbaaSize = (xhci.maxSlots+1)*sizeof(uint64_t);
  xhci.dcbaa = mallocAligned(dcbaaSize, 64);
  memset(xhci.dcbaa, 0, dcbaaSize);
  xhci_write64(
    &xhci,
    &xhci.opRegs->dcbaap,
    (uint64_t)xhci.dcbaa
  );

  struct xhci_int * interrupters = (struct xhci_int *)(bar + xhci.capRegs->rtsoff + 0x20);

  xhci.eventRing = createRing(64);

  struct xhci_erst_seg * segments = mallocAligned(sizeof(struct xhci_erst_seg), 64);
  segments[0].base = xhci.eventRing->base;
  segments[0].size = 64;

  interrupters[0].erstsz = 1;
  xhci_write64(
    &xhci,
    &interrupters[0].erdp,
    xhci.eventRing->dp
  );
  xhci_write64(
    &xhci,
    &interrupters[0].erstba,
    (interrupters[0].erstba & 0b11111) | (uint64_t)segments
  );

  xhci.commandRing = createRing(32);
  uint64_t crcr = ((uint64_t)xhci.commandRing->base & (~(uint64_t) 0b111111)) | (xhci.commandRing->pcs&1);
  xhci_write64(
    &xhci,
    &xhci.opRegs->crcr,
    crcr
  );

  //Run the XHCI
  xhci.opRegs->xhcicmd |= 1;

  print("Issuing noop command\n");
  xhci.commandRing->ep->cmd.params = (23 << 10) | (xhci.commandRing->pcs&1);
  xhci.commandRing->ep->cmd.pointer = 0;
  xhci.commandRing->ep->cmd.reserved = 0;
  xhci.commandRing->ep++;
  print("Ringing doorbell\n");
  xhci_ring_host(&xhci);

  if(!xhci_read_cmd_out(&xhci, NULL)){
    print("No op command failed\n");
    return;
  }
  xhci_scan(&xhci);
}

void xhci_write64(struct xhci *xhci, uint64_t * dest, uint64_t val){
  //64-bit write
  if(xhci->ac64){
    *dest = val;
    return;
  }

  //32-bit write
  *((uint32_t*)dest) = val & 0xffffffff;
}

void xhci_reset(struct xhci* xhci){
  xhci->opRegs->xhcicmd = xhci->opRegs->xhcicmd | (1 << 1);
}

void xhci_scan(struct xhci* xhci){
  print("Searching for connected devices\n");
  for(uint64_t port = 1; port <= xhci->maxPorts; port++){
    struct xhci_port_reg * portRegs = (struct xhci_port_reg *) ((uint64_t) xhci->opRegs + 0x400 + (0x10 * (port-1)));
    uint8_t ccs = portRegs->portsc & 1;
    if(ccs == 1){
      print("Device connected at port ");
      printHex(port);
      print("\n");
      xhci_dev_init(xhci, port);
    }
  }
}

void xhci_dev_init(struct xhci* xhci, uint8_t port){
  print("Initializing device at XHCI port ");
  printNum(port);
  print("\n");

  print("Issuing enable slot command");
  print(" trb=");
  printHex(xhci->commandRing->ep);
  print("\n");
  xhci->commandRing->ep->cmd.params = (9 << 10) | (xhci->commandRing->pcs&1);
  xhci->commandRing->ep->cmd.pointer = 0;
  xhci->commandRing->ep->cmd.reserved = 0;
  xhci->commandRing->ep++;
  print("Ringing doorbell\n");
  xhci_ring_host(xhci);

  struct xhci_cmd_compl_trb* event;

  if(!xhci_read_cmd_out(xhci, &event)){
    print("XHCI Enable slot command has failed.\n");
    return;
  } else {
    print("XHCI Enable slot command executed successfully.\n");
  }
  uint8_t slotId = event->params >> 24;
  print("Allocated slot ");
  printNum(slotId);
  print("\n");
  print("Creating device contexts\n");

  struct xhci_dev_in_context * inContext = mallocAligned(sizeof(struct xhci_dev_in_context), 64);
  memset(inContext, 0, sizeof(struct xhci_dev_in_context));

  inContext->inputCtrl.dropFlags = 0;
  inContext->inputCtrl.addFlags = 0b11;

  uint8_t portSpeed = xhci_get_portSpeed(xhci, port);
  inContext->devContext.slotContext.slotdw0 = (1 << 27) | (portSpeed << 20); //Context entries, speed
  inContext->devContext.slotContext.slotdw1 = port; //Root port
  inContext->devContext.slotContext.slotdw2 = 0; // Interrupter 0
  inContext->devContext.slotContext.slotdw3 = 0; // pci_slot state, device addr


  struct trb_ring* transferRing = createRing(64);

  struct xhci_ep_context * controlEp = &inContext->devContext.epContexts[0];
  controlEp->epdw0 = 0;
  controlEp->epdw1 = (3 << 1) | (4 << 3);
  controlEp->epqw2=((uint64_t)transferRing->dp) | 1;
  controlEp->epdw4 = 8;

  controlEp->epdw5 = 0;
  controlEp->epdw6 = 0;
  controlEp->epdw7 = 0;

  struct xhci_dev_context * outContext = mallocAligned(sizeof(struct xhci_dev_context), 64);

  memset(outContext, 0, sizeof(struct xhci_dev_context));

  xhci->dcbaa[slotId] = (uint64_t)outContext;

  print("Issuing Address device command\n");
  xhci->commandRing->ep->cmd.params = (slotId << 24) | (11 << 10) | (xhci->commandRing->pcs&1) | (1 << 9);
  xhci->commandRing->ep->cmd.pointer=(uint64_t) inContext;
  xhci->commandRing->ep->cmd.reserved = 0;
  xhci->commandRing->ep++;

  print("Ringing doorbell\n");
  xhci_ring_host(xhci);

  if(!xhci_read_cmd_out(xhci, &event)){
    print("XHCI Address device command failed\n");
    free(inContext);
    free(outContext);
    destroyRing(transferRing);
    return;
  }
  print("Device initialized successfully\n");

  //TODO: save the device and use it
  free(inContext);
  free(outContext);
  destroyRing(transferRing);
}

void xhci_ring_host(struct xhci* xhci){
  uint32_t* doorbell = (uint32_t *)((uint64_t)xhci->capRegs + xhci->capRegs->dboff);
  *doorbell = 0;
}

uint8_t xhci_get_portSpeed(struct xhci* xhci, uint8_t port){
  struct xhci_port_reg * portRegs = (struct xhci_port_reg *) ((uint64_t) xhci->opRegs + 0x400 + (0x10 * (port-1)));
  return portRegs->portsc & 0b1111;
}
