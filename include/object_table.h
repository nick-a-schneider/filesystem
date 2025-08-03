#pragma once

#include "block_allocator.h"
#include <stdint.h>
#include <stdbool.h>

typedef int8_t objectID;

typedef struct {
    bool in_use;
    objectID id;
    uint16_t size;
    uint16_t used; // in bytes
    void* head;
} MemObject;

typedef struct {
    MemObject* objects;
    uint16_t size;
    uint64_t used; // bitmap flag
    BlockAllocator* allocator;
} MemmoryObjectTable;

int aquireMemObject(MemmoryObjectTable* table, uint16_t size, objectID* id_out);
int releaseMemObject(MemmoryObjectTable* table, objectID id);
int writeMemObject(MemmoryObjectTable* table, objectID id, void* data, uint16_t size);
int appendMemObject(MemmoryObjectTable* table, objectID id, void* data, uint16_t size);
int readMemObject(MemmoryObjectTable* table, objectID id, void* data, uint16_t size);
int verifyMemObjectSize(MemmoryObjectTable* table, objectID id, uint16_t size);
MemObject* getMemObjectHandle(MemmoryObjectTable* table, objectID id);
int lockMemObject(MemmoryObjectTable* table, objectID id);
int unlockMemObject(MemmoryObjectTable* table, objectID id);
