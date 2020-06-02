#ifndef ATA_H
#define ATA_H

#include "../cpu/port.h"
#include "console.h"

struct bpb {
  uint16_t sectorSize;
  uint8_t clusterSize;
  uint16_t reservedSectors;
  uint8_t fatCount;
  uint16_t maxRootDirectories;
  uint16_t totalSectors;
  uint8_t mediaDescriptor;
  uint16_t sectorsPerFat;
}__attribute__((packed));

struct fat_dir {
  uint8_t name[8];
  uint8_t extension[3];
  uint8_t attr;
  uint8_t userAttr;
  uint8_t r;
  uint16_t createTime;
  uint16_t createDate;
  uint16_t lastAccessDate;
  uint16_t accessRights;
  uint16_t lastModifiedTime;
  uint16_t lastModifiedDate;
  uint16_t cluster;
  uint32_t size;
};

struct mbr_partition{
  uint8_t status;

  uint8_t first_head;
  uint8_t first_sec_hcyl;
  uint8_t first_lcyl;

  uint8_t type;

  uint8_t last_head;
  uint8_t last_sec_hcyl;
  uint8_t last_lcyl;

  uint32_t lba;
  uint32_t count;
}__attribute__((packed));

struct mbr{
  uint8_t jump[3];
  uint8_t oem[8];
  struct bpb bpb;

  uint8_t bootcode[422];
  struct mbr_partition partitions[4];
  uint16_t signature;
} __attribute__((packed));



void AtaIdentify();

void AtaRead();

void AtaReadSectors(uint64_t lba, uint8_t count, uint8_t* buffer);

void readMbr(struct mbr* mbr);
#endif
