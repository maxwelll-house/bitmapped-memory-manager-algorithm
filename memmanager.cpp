#include "memmanager.h"
#include "complexpart.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

MemoryManager externMemManager;

void BitMapInput::SetBit(int position, bool flag)
{
    BlocksAvailable += flag ? 1 : -1;
    int elementNo = position / INT_SIZE;
    int bitNo = position % INT_SIZE;
    if (flag)
    {
        BitMap[elementNo] = BitMap[elementNo] | (1 << bitNo);
    }
    else
    {
        BitMap[elementNo] = BitMap[elementNo] & ~(1 << bitNo);
    }
}

void BitMapInput::SetMultipleBits(int position, bool flag, int count)
{
    BlocksAvailable += flag ? count : -count;
    int elementNo = position / INT_SIZE;
    int bitNo = position % INT_SIZE;

    int bitSize = (count <= INT_SIZE - bitNo) ? count : INT_SIZE - bitNo;
    SetRangeOfInt(&BitMap[elementNo], bitNo + bitSize - 1, bitNo, flag);
    count -= bitSize;
    if (!count)
    {
        return;
    }
    int i = ++elementNo;
    while (count >= 0)
    {
        if (count <= INT_SIZE)
        {
            SetRangeOfInt(&BitMap[i], count - 1, 0, flag);
            return;
        }
        else
        {
            BitMap[i] = flag ? unsigned(-1) : 0;
        }
        count -= 32;
        i++;
    }
}

void BitMapInput::SetRangeOfInt(int* element, int msb, int lsb, bool flag)
{
    if (flag)
    {
        int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> (INT_SIZE - msb - 1));
        *element |= mask;
    }
    else
    {
        int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> (INT_SIZE - msb - 1));
        *element &= ~mask;
    }
}

ComplexObject* BitMapInput::FirstFreeBlock(size_t size)
{
    for (int i = 0; i < BITMAP_ELEMS; ++i)
    {
        if (BitMap[i] == 0)
        { 
            continue; // no free bit was found 
        }
        int result = BitMap[i] & -(BitMap[i]); // this expression yields the first bit position which is 1 in an int from right.
        void* address = 0;
        int basePos = (INT_SIZE * i);
        switch (result)
        {
            //make the corresponding bit 0 meaning block is no longer free
            case 0x00000001: return ComplexObjectAddress(basePos + 0);
            case 0x00000002: return ComplexObjectAddress(basePos + 1);
            case 0x00000004: return ComplexObjectAddress(basePos + 2);
            case 0x00000008: return ComplexObjectAddress(basePos + 3);
            case 0x00000010: return ComplexObjectAddress(basePos + 4);
            case 0x00000020: return ComplexObjectAddress(basePos + 5);
            case 0x00000040: return ComplexObjectAddress(basePos + 6);
            case 0x00000080: return ComplexObjectAddress(basePos + 7);
            case 0x00000100: return ComplexObjectAddress(basePos + 8);
            case 0x00000200: return ComplexObjectAddress(basePos + 9);
            case 0x00000400: return ComplexObjectAddress(basePos + 10);
            case 0x00000800: return ComplexObjectAddress(basePos + 11);
            case 0x00001000: return ComplexObjectAddress(basePos + 12);
            case 0x00002000: return ComplexObjectAddress(basePos + 13);
            case 0x00004000: return ComplexObjectAddress(basePos + 14);
            case 0x00008000: return ComplexObjectAddress(basePos + 15);
            case 0x00010000: return ComplexObjectAddress(basePos + 16);
            case 0x00020000: return ComplexObjectAddress(basePos + 17);
            case 0x00040000: return ComplexObjectAddress(basePos + 18);
            case 0x00080000: return ComplexObjectAddress(basePos + 19);
            case 0x00100000: return ComplexObjectAddress(basePos + 20);
            case 0x00200000: return ComplexObjectAddress(basePos + 21);
            case 0x00400000: return ComplexObjectAddress(basePos + 22);
            case 0x00800000: return ComplexObjectAddress(basePos + 23);
            case 0x01000000: return ComplexObjectAddress(basePos + 24);
            case 0x02000000: return ComplexObjectAddress(basePos + 25);
            case 0x04000000: return ComplexObjectAddress(basePos + 26);
            case 0x08000000: return ComplexObjectAddress(basePos + 27);
            case 0x10000000: return ComplexObjectAddress(basePos + 28);
            case 0x20000000: return ComplexObjectAddress(basePos + 29);
            case 0x40000000: return ComplexObjectAddress(basePos + 30);
            case 0x80000000: return ComplexObjectAddress(basePos + 31);
            default: break;
        }
    }
    return 0;
}

ComplexObject* BitMapInput::ComplexObjectAddress(int pos)
{
    SetBit(pos, false);
    return &((static_cast<ComplexObject*>(Head()) + (pos / INT_SIZE))[INT_SIZE - ((pos % INT_SIZE) + 1)]);
}

void* BitMapInput::Head()
{
    return externMemManager.GetMemoryPoolList()[Index];
}

void* MemoryManager::allocate(size_t size)
{
    if (size == sizeof(ComplexObject))
    {
        std::set<BitMapInput*>::iterator freeMapI = FreeMapEntries.begin();
        if (freeMapI != FreeMapEntries.end())
        {
            BitMapInput* mapEntry = *freeMapI;
            return mapEntry->FirstFreeBlock(size);
        }
        else
        {
            AllocateChunkAndInitBitMap();
            FreeMapEntries.insert(&(BitMapInputList[BitMapInputList.size() - 1]));
            return BitMapInputList[BitMapInputList.size() - 1].FirstFreeBlock(size);
        }
    }
    else  // array version
    {
        if (ArrayMemoryList.empty())
        {
            return AllocateArrayMemory(size);
        }
        else
        {
            std::map<void*, ArrayMemoryInfo>::iterator infoI = ArrayMemoryList.begin();
            std::map<void*, ArrayMemoryInfo>::iterator infoEndI = ArrayMemoryList.end();
            while (infoI != infoEndI)
            {
                ArrayMemoryInfo info = (*infoI).second;
                if (info.StartPosition != 0)
                {
                    continue; // only the blocks where allocation is done from first byte
                }
                else
                {
                    BitMapInput* entry = &BitMapInputList[info.MemPoolListIndex];
                    if (entry->BlocksAvailable < (size / sizeof(ComplexObject)))
                        return AllocateArrayMemory(size);
                    else
                    {
                        info.StartPosition = BITMAP_SIZE - entry->BlocksAvailable;
                        info.Size = size / sizeof(ComplexObject);
                        ComplexObject* baseAddress = static_cast<ComplexObject*>(MemoryPoolList[info.MemPoolListIndex]) + info.StartPosition;

                        ArrayMemoryList[baseAddress] = info;
                        SetMultipleBlockBits(&info, false);

                        return baseAddress;
                    }
                }
            }
        }
    }
    return 0;
}

void* MemoryManager::AllocateArrayMemory(size_t size)
{
    void* chunkAddress = AllocateChunkAndInitBitMap();
    ArrayMemoryInfo info;
    info.MemPoolListIndex = MemoryPoolList.size() - 1;
    info.StartPosition = 0;
    info.Size = size / sizeof(ComplexObject);
    ArrayMemoryList[chunkAddress] = info;
    SetMultipleBlockBits(&info, false);
    return chunkAddress;
}

void* MemoryManager::AllocateChunkAndInitBitMap()
{
    BitMapInput mapEntry;
    ComplexObject* memoryBeginAddress = reinterpret_cast<ComplexObject*>(new char[sizeof(ComplexObject) * BITMAP_SIZE]);
    MemoryPoolList.push_back(memoryBeginAddress);
    mapEntry.Index = MemoryPoolList.size() - 1;
    BitMapInputList.push_back(mapEntry);
    return memoryBeginAddress;
}

void MemoryManager::free(void* object)
{
    if (ArrayMemoryList.find(object) == ArrayMemoryList.end())
        SetBlockBit(object, true);
    else
    {
        ArrayMemoryInfo* info = &ArrayMemoryList[object];
        SetMultipleBlockBits(info, true);
    }
}

void MemoryManager::SetBlockBit(void* object, bool flag)
{
    int i = BitMapInputList.size() - 1;
    for (; i >= 0; i--)
    {
        BitMapInput* bitMap = &BitMapInputList[i];
        if ((bitMap->Head() <= object) && (&(static_cast<ComplexObject*>(bitMap->Head()))[BITMAP_SIZE - 1] >= object))
        {
            int position = static_cast<ComplexObject*>(object) - static_cast<ComplexObject*>(bitMap->Head());
            bitMap->SetBit(position, flag);
            flag ? bitMap->BlocksAvailable++ : bitMap->BlocksAvailable--;
        }
    }
}

void MemoryManager::SetMultipleBlockBits(ArrayMemoryInfo* info, bool flag)
{
    BitMapInput* mapEntry = &BitMapInputList[info->MemPoolListIndex];
    mapEntry->SetMultipleBits(info->StartPosition, flag, info->Size);
}

std::vector<void*>& MemoryManager::GetMemoryPoolList()
{
    return MemoryPoolList;
}