#pragma once

#include <sys/types.h> 
#include <vector>
#include <set>
#include <map>
#include <bitset>

const int BITMAP_SIZE = 1024;
const int INT_SIZE = sizeof(int) * 8;
const int BITMAP_ELEMS = BITMAP_SIZE / INT_SIZE;


class ComplexObject;
typedef struct BitMapInput
{
    int Index;
    int BlocksAvailable;
    int BitMap[BITMAP_SIZE];
public:
    BitMapInput() :BlocksAvailable(BITMAP_SIZE)
    {
        // in the beginning all blocks are free and bit value 1 in the map denotes available block
        memset(BitMap, 0xff, BITMAP_SIZE / sizeof(char));
    }
    void SetBit(int position, bool flag);
    void SetMultipleBits(int position, bool flag, int count);
    void SetRangeOfInt(int* element, int msb, int lsb, bool flag);
    ComplexObject* FirstFreeBlock(size_t size);
    ComplexObject* ComplexObjectAddress(int pos);
    void* Head();
}
BitMapInput;


typedef struct ArrayInfo
{
    int MemPoolListIndex;
    int StartPosition;
    int Size;
}
ArrayMemoryInfo;

class IMemoryManager
{
public:
    virtual void* allocate(size_t) = 0;
    virtual void free(void*) = 0;
};

class MemoryManager : public IMemoryManager
{
    std::vector<void*> MemoryPoolList;
    std::vector<BitMapInput> BitMapInputList;
    std::set<BitMapInput*> FreeMapEntries;
    std::map<void*, ArrayMemoryInfo> ArrayMemoryList;

private:
    void* AllocateArrayMemory(size_t size);
    void* AllocateChunkAndInitBitMap();
    void SetBlockBit(void* object, bool flag);
    void SetMultipleBlockBits(ArrayMemoryInfo* info, bool flag);
public:
    MemoryManager() {}
    ~MemoryManager() {}
    void* allocate(size_t);
    void free(void*);
    std::vector<void*>& GetMemoryPoolList();
};