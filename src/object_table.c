
#include "object_table.h"
#include "block_allocator.h"
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define CHECK_BIT(bit)  (table->used & (1UL << bit))
#define SET_BIT(bit)    (table->used |= (1UL << bit))
#define CLR_BIT(bit)    (table->used &= ~(1UL << bit))

#define MEM_SET(src, dest, size) for (uint16_t i = 0; i < size; i++) ((uint8_t*)dest)[i] = ((uint8_t*)src)[i]

static inline int verifyID(MemmoryObjectTable* table, objectID id) {
    if (!table || !id) return -EINVAL;
    if (id >= table->size) return -EBADF;
    if (!CHECK_BIT(id)) return -ENOFILE;
    return 0;
}

int aquireMemObject(MemmoryObjectTable* table, uint16_t size, objectID* id_out) {
    if (!table || !size) return -EINVAL;

    uint8_t id;
    for (id= 0; id <= table->size; id++) {
        if (!CHECK_BIT(id)) break;
    }
    if (id == table->size) return -ENFILE;
    uint16_t num_blocks = size / table->allocator->block_size;
    void* block = blockAllocate(table->allocator, num_blocks);
    if (!block) return -ENOSPC;
    
    SET_BIT(id);  
    MemObject* object = &table->objects[id];
    object->head = block;
    object->id = table->used;
    object->size = size;
    *id_out = id;
    return 0;
}

int releaseMemObject(MemmoryObjectTable* table, objectID id) {
    int err = verifyID(table, id);
    if (err) return err;
    MemObject* object = &table->objects[id];
    if (!blockDeallocate(table->allocator, object->head)) return -EDEADLOCK;
    CLR_BIT(id);
    return 0;
}

int writeMemObject(MemmoryObjectTable* table, objectID id, void* data, uint16_t size) {
    int err = verifyMemObjectSize(table, id, size);
    if (err) return err;
    if (!data) return -EINVAL;
    MemObject* object = &table->objects[id];
    uint8_t* head = ((uint8_t*)object->head);
    MEM_SET((uint8_t*)data, head, size);
    object->used = size;
    return 0;
}

int appendMemObject(MemmoryObjectTable* table, objectID id, void* data, uint16_t size) {
    int err = verifyMemObjectSize(table, id, size);
    if (err) return err;
    if (!data) return -EINVAL;
    MemObject* object = &table->objects[id];
    uint8_t* head = ((uint8_t*)object->head) + object->used;
    MEM_SET((uint8_t*)data, head, size);
    object->used += size;
    return 0;
}

int readMemObject(MemmoryObjectTable* table, objectID id, void* data, uint16_t size) {
    int err = verifyMemObjectSize(table, id, size);
    if (err && err != -EFBIG) return err; // okay to read into a larger buffer
    if (!data) return -EINVAL;
    MemObject* object = &table->objects[id];
    if (object->used < size) size = object->used;
    uint8_t* head = (uint8_t*)object->head;
    MEM_SET(head, (uint8_t*)data, size);
    return 0;
}

int verifyMemObjectSize(MemmoryObjectTable* table, objectID id, uint16_t size) {
    int err = verifyID(table, id);
    if (err) return err;
    MemObject* object = &table->objects[id];
    if (object->in_use) return -EBUSY;
    if (object->size < size) return -EFBIG;
    return 0;
}

MemObject* getMemObjectHandle(MemmoryObjectTable* table, objectID id) {
    int err = verifyID(table, id);
    if (err) return NULL;
    MemObject* object = &table->objects[id];
    if (object->in_use) return NULL;
    return object;
}

int lockMemObject(MemmoryObjectTable* table, objectID id) {
    int err = verifyID(table, id);
    if (err) return err;
    MemObject* object = &table->objects[id];
    object->in_use = true;
    return 0;
}

int unlockMemObject(MemmoryObjectTable* table, objectID id) {
    int err = verifyID(table, id);
    if (err) return err;
    MemObject* object = &table->objects[id];
    object->in_use = false;
    return 0;
}
