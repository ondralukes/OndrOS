#ifndef PAGING_H
#define PAGING_H
#include "../kernel/types.h"
#include "../io/console.h"

void allowPageAccess(uint64_t addr);

#endif
