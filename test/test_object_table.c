#include "test_utils.h"
#include "tier_allocator.h"
#include "object_table.h"

void test_MemObject() {
    uint8_t memory[4096];
    BlockAllocator allocator;
    initBlockAllocator(&allocator, 16, memory, sizeof(memory));
    MemmoryObjectTable table = {
        .allocator = &allocator,
        .objects = (MemObject[16]){},
        .size = 16,
        .used = 0
    };
    objectID object;
    uint8_t msg[] = "hello world";
    TEST_CASE("aquireMemObject") {
        int res = aquireMemObject(&table, 1200, &object);
        ASSERT_EQUAL_INT(res, 0, "aquireMemObject failed");
    } CASE_COMPLETE;

    TEST_CASE("writeMemObject") {
        int res = writeMemObject(&table, object, msg, 11);
        ASSERT_EQUAL_INT(res, 0, "writeMemObject failed");
    } CASE_COMPLETE;

    TEST_CASE("readMemObject") {
        uint8_t buffer[80];
        int res = readMemObject(&table, object, (void*)buffer, 80);
        ASSERT_EQUAL_INT(res, 0, "writeMemObject failed");
        ASSERT_EQUAL_STR(msg, buffer, 11, "readMemObject failed");
    } CASE_COMPLETE;

    TEST_CASE("getMemObjectHandle") {
        MemObject* obj = getMemObjectHandle(&table, object);
        ASSERT_NOT_NULL(obj, " failed to get handle to object");
    } CASE_COMPLETE;
    
}

int main() {
    TEST_EVAL(test_MemObject);
    return testGetStatus();
}
