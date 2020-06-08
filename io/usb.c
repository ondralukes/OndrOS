#include "usb.h"

struct trb_ring * createRing(uint64_t size){
  struct trb_ring * ring = malloc(sizeof(struct trb_ring));
  ring->base = mallocAligned(sizeof(union usb_trb)*size, 64);
  memset(ring->base, 0, sizeof(union usb_trb)*size);
  ring->ep = ring->base;
  ring->dp = ring->base;
  ring->pcs = 1;
  ring->ccs = 1;
  ring->base[size-1].cmd.flags = (1 << 1) | (1 << 4) | (6 << 10);
  ring->base[size-1].cmd.pointer = (uint64_t)ring->base;
  return ring;
}

union usb_trb* ringDequeue(struct trb_ring * ring){
  //Check if ring is empty
  if((ring->dp->cmd.flags & 1) != ring->ccs){
    return NULL;
  }

  return ring->dp++;
}

void readUSB(uint32_t bus, uint32_t slot, uint32_t func){
  print("Found USB at PCI ");
  printNum(bus);
  print(".");
  printNum(slot);
  print(".");
  printNum(func);
  print("\n");


  uint8_t progIf = pciReadWordConfig(bus, slot, func, 8) >> 8;

  if(progIf != 0x30){
    print("Only XHCI controllers are supported\n");
    return;
  }

  uint8_t headerType = pciReadWordConfig(bus, slot, func, 14) & 0x7f;

  if(headerType != 0x0){
    print("Invalid header type (");
    printHex(headerType);
    print(")\n");
    return;
  }
  uint32_t bar0 = pciReadDWordConfig(bus, slot, func, 0x10);
  uint32_t bar1 = pciReadDWordConfig(bus, slot, func, 0x14);

  uint64_t bar = (bar0 & 0xfffffff0) | ((uint64_t)(bar1 & 0xffffffff) << 32);

  pciWriteDWordConfig(bus, slot, func, 0x10, 0xffffffff);
  pciWriteDWordConfig(bus, slot, func, 0x14, 0xffffffff);

  uint32_t barsz0 = pciReadDWordConfig(bus, slot, func, 0x10);
  uint32_t barsz1 = pciReadDWordConfig(bus, slot, func, 0x14);

  pciWriteDWordConfig(bus, slot, func, 0x10, bar0);
  pciWriteDWordConfig(bus, slot, func, 0x14, bar1);

  barsz0 &= ~0b1111;
  barsz0 = ~barsz0;
  barsz0++;

  barsz1 = ~barsz1;

  struct usb_cap_reg * capRegs = (struct usb_cap_reg *) bar;
  struct usb_op_reg * opRegs = (struct usb_op_reg *) (bar + capRegs->caplength);

  uint8_t csz = (capRegs->hccparams1 >> 2)&1;
  uint8_t ac64 = capRegs->hccparams1&1;
  print("CSZ=");
  printNum(csz);
  print("\nAC64=");
  printNum(ac64);
  print("\n");

  print("Resetting XHCI...\n");
  opRegs->usbcmd = opRegs->usbcmd | (1 << 1);
  uint8_t maxPorts = (capRegs->hcsparams1 >> 24) & 0xff;
  uint8_t maxSlots = capRegs->hcsparams2 & 0xff;


  uint16_t selectedPort = 0;
  uint16_t portSpeed = 0;

  print("Searching for connected devices\n");
  for(uint64_t port = 1; port <= maxPorts; port++){
    struct usb_port_reg * portRegs = (struct usb_port_reg *) ((uint64_t) opRegs + 0x400 + (0x10 * (port-1)));
    uint8_t ccs = portRegs->portsc & 1;
    if(ccs == 1){
      print("Device connected at port");
      printHex(port);
      selectedPort = port;
      portSpeed = portRegs->portsc & 0b1111;
      print("\n");
    }
  }

  print("Port = ");
  printNum(selectedPort);
  print(" Speed = ");
  printHex(portSpeed);
  print("\n");
  print("Creating DCBAA\n");
  uint64_t dcbaaSize = (maxSlots+1)*sizeof(uint64_t);
  uint64_t * dcbaa = mallocAligned(dcbaaSize, 64);
  memset(dcbaa, 0, dcbaaSize);
  opRegs->dcbaap = (uint64_t)dcbaa;
  // for(uint64_t i = 1;i < maxSlots;i++){
  //   uint64_t dcba = dcbaa[i];
  //   if(dcba != 0){
  //     printNum(i);
  //     print(" : ");
  //     printHex(dcba);
  //     print("\n");
  //     struct usb_slot_context * slotContext = (struct usb_slot_context *)dcba;
  //     uint8_t isHub = slotContext->params1 >> 26;
  //     uint16_t contextCount = slotContext->params1 >> 27;
  //     print("Is hub=");
  //     printNum(isHub);
  //     print("\nContext entries=");
  //     printHex(slotContext->params1);
  //     printHex(slotContext->maxExitLatency);
  //     print("\n");
  //   }
  // }
  struct usb_int * interrupters = (struct usb_int *)(bar + capRegs->rtsoff + 0x20);
  print("Setting up event ring");
  struct trb_ring * eventRing = createRing(64);

  struct usb_erst_seg * segments = mallocAligned(sizeof(struct usb_erst_seg), 64);
  segments[0].base = eventRing->base;
  segments[0].size = 64;

  interrupters[0].erstsz = 1;
  interrupters[0].erdp = eventRing->dp;
  interrupters[0].erstba = (interrupters[0].erstba & 0b11111) | (uint64_t)segments;

  struct trb_ring* commandRing = createRing(32);
  uint64_t crcr = ((uint64_t)commandRing->base & (~(uint64_t) 0b111111)) | (commandRing->pcs&1);
  opRegs->crcr = crcr;

  print("Setting R/S to Run\n");
  opRegs->usbcmd |= 1;

  print("Issuing enable slot command\n");
  commandRing->ep->cmd.flags = (9 << 10) | (commandRing->pcs&1);
  commandRing->ep->cmd.pointer = 0;
  commandRing->ep->cmd.reserved = 0;
  commandRing->ep->cmd.slotType = 0;
  commandRing->ep++;
  print("Ringing doorbell\n");
  uint32_t* doorbell = (uint32_t *)(bar + capRegs->dboff);
  *doorbell = 0;

  print("Waiting for command completion event\n");
  union usb_trb * eventTrb;
  while((eventTrb = ringDequeue(eventRing)) == NULL);

  struct usb_cmd_compl_trb* event = (struct usb_cmd_compl_trb*)eventTrb;
  uint64_t completionCode = event->compl >> 24;
  uint64_t slotId = event->params >> 24;
  print("Event : code=");
  printNum(completionCode);
  print(" PCS=");
  printNum(event->params&1);
  print(" Slot=");
  printNum(slotId);
  print("\n");

  if(completionCode != 1){
    print("XHCI Enable Slot command has failed.\n");
    free(commandRing);
    return;
  } else {
    print("XHCI Enable Slot command executed successfully.\n");
  }

  print("Creating device contexts\n");

  struct usb_dev_in_context * inContext = mallocAligned(sizeof(struct usb_dev_in_context), 64);
  memset(inContext, 0, sizeof(struct usb_dev_in_context));

  inContext->inputCtrl.dropFlags = 0;
  inContext->inputCtrl.addFlags = 0b11;

  inContext->devContext.slotContext.slotdw0 = (1 << 27) | (portSpeed << 20); //Context entries, speed
  inContext->devContext.slotContext.slotdw1 = selectedPort; //Root port
  inContext->devContext.slotContext.slotdw2 = 0; // Interrupter 0
  inContext->devContext.slotContext.slotdw3 = 0; // Slot state, device addr


  struct trb_ring* transferRing = createRing(64);

  struct usb_ep_context * controlEp = &inContext->devContext.epContexts[0];
  controlEp->epdw0 = 0;
  controlEp->epdw1 = (3 << 1) | (4 << 3);
  controlEp->epqw2 = ((uint64_t)transferRing->dp) | 1;
  controlEp->epdw4 = 8;

  controlEp->epdw5 = 0;
  controlEp->epdw6 = 0;
  controlEp->epdw7 = 0;

  struct usb_dev_context * outContext = mallocAligned(sizeof(struct usb_dev_context), 64);

  memset(outContext, 0, sizeof(struct usb_dev_context));

  dcbaa[slotId] = (uint64_t)outContext;

  print("Issuing Address device command slot=");
  printNum(slotId);
  print("\n");
  commandRing->ep->cmd.flags = (11 << 10) | (commandRing->pcs&1);
  commandRing->ep->cmd.pointer = (uint64_t)inContext;
  commandRing->ep->cmd.reserved = 0;
  commandRing->ep->cmd.slotType = slotId << 8;
  commandRing->ep++;
  print("Ringing doorbell\n");
  *doorbell = 0;

  while(1){
  print("Waiting for command completion event\n");
  while((eventTrb = ringDequeue(eventRing)) == NULL);

  event = (struct usb_cmd_compl_trb*)eventTrb;
  completionCode = event->compl >> 24;
  slotId = event->params >> 24;
  print("Event : code=");
  printNum(completionCode);
  print(" PCS=");
  printNum(event->params&1);
  print(" Type = ");
  printNum((event->params >> 10)&0b111111);
  print(" Slot=");
  printNum(slotId);
  print(" TRB=");
  printHex(event->cmdTrb);
  print("\n");
  if(completionCode != 1){
    print("XHCI Address device command has failed.\n");
    //free(commandRing);
    //return;
  } else {
    print("XHCI Address device command executed successfully.\n");
  }
}
}
