#include "usb.h"

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
  
  uint8_t maxPorts = (capRegs->hcsparams1 >> 24) & 0xff;

  print("Searching for connected devices\n");
  for(uint64_t port = 0; port < maxPorts; port++){
    struct usb_port_reg * portRegs = (struct usb_port_reg *) ((uint64_t) opRegs + 0x400 + (0x10 * port));
    uint8_t ccs = portRegs->portsc & 1;
    if(ccs == 1){
      print("Device connected at port");
      printHex(port);
      print("\n");
    }
  }

  struct usb_int * interrupters = (struct usb_int *)(bar + capRegs->rtsoff + 0x20);

  uint64_t eventRingSize = sizeof(struct usb_cmd_compl_trb) * 64;
  struct usb_cmd_compl_trb * eventRing = mallocAligned(eventRingSize, 64);
  memset(eventRing, 0, eventRingSize);

  struct usb_erst_seg * segments = mallocAligned(sizeof(struct usb_erst_seg), 64);
  segments[0].base = eventRing;
  segments[0].size = 64;

   interrupters[0].erstsz = 1;
   interrupters[0].erdp = eventRing;
   interrupters[0].erstba = (interrupters[0].erstba & 0b11111) | (uint64_t)segments;

  union usb_trb * commandRing = mallocAligned(sizeof(union usb_trb) * 32, 64);
  memset(commandRing, 0, sizeof(union usb_trb) * 32);

  uint8_t pcs = 1;

  uint64_t crcr = ((uint64_t)commandRing & (~(uint64_t) 0b111111)) | (pcs&1);
  opRegs->crcr = crcr;

  print("Setting R/S to Run\n");
  opRegs->usbcmd |= 1;

  commandRing[0].cmd.flags = (23 << 10) | pcs&1;
  commandRing[0].cmd.reserved1 = 0;
  commandRing[0].cmd.reserved2 = 0;
  commandRing[0].cmd.slotType = 0;
  print("Ringing doorbell\n");
  uint32_t* doorbell = (uint32_t *)(bar + capRegs->dboff);
  *doorbell = 0;

  struct usb_cmd_compl_trb * event = eventRing;
  while(event->params == 0);

  uint64_t completionCode = event->compl >> 24;
  print("Event : code=");
  printNum(completionCode);
  print(" PCS=");
  printNum(event->params&1);
  print("\n");

  if(completionCode != 1){
    print("XHCI No Op command has failed.\n");
  } else {
    print("XHCI No Op command executed successfully.\n");
  }

  free(commandRing);
}
