#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(__GNUC__) || defined(__GNUG__)
    /* GNU GCC/G++. */
#pragma pack(4)
#elif defined(__clang__)
    /* Cland/LLVM. */
#elif defined(_MSC_VER)
    /* Microsoft Visual Studio. */
#endif

namespace tsk {

//#undef MEMDEBUG
#define MEMDEBUG

// total 64 bytes
struct memoryblock_t
{
#ifndef MEMDEBUG
    memoryblock_t() : id(0), ptr(NULL), size(0), 
               prev(NULL), next(NULL) { }
#else
    memoryblock_t() : id(0), ptr(NULL), size(0), 
               label(NULL), file(NULL), line(0), 
               prev(NULL), next(NULL) { }
#endif

    unsigned long int id;
    void *ptr;
    int size;

#ifdef MEMDEBUG
    const char *label;
    const char *file;
    int line;
#endif

    struct memoryblock_t *prev;
    struct memoryblock_t *next;
};

class CMemoryManager
{
public:
    CMemoryManager();
    ~CMemoryManager();

#ifndef MEMDEBUG

    void *GetClearMemory(int size);
    void *GetMemory(unsigned long size);

#else

#define GetMemory(size)     GetMemoryDebug(size, #size, __FILE__, __LINE__);
#define GetClearMemory(size)        GetClearMemoryDebug(size, #size, __FILE__, __LINE__);

//allocate a memory block of the given size
void *GetMemoryDebug(unsigned long size, const char *label, const char *file, int line);
//allocate a memory block of the given size and clear it
void *GetClearMemoryDebug(unsigned long size, const char *label, const char *file, int line);

void PrintMemoryLabels(void);

#endif //MEMDEBUG

    void DumpMemory(void);
    void FreeMemory(void *ptr);

    int MenorySize(void *ptr);
    void PrintMemorySize(unsigned long size);
    int TotalAllocatedMemory(void);

private:
#ifdef MEMDEBUG

    void LinkMemoryBlock(memoryblock_t *block);
    void UnlinkMemoryBlock(memoryblock_t *block);
    memoryblock_t *BlockFromPointer(void *ptr, const char *str);

#endif

private:
    int m_totalAllocMemSize;
    memoryblock_t *memory;

#ifdef MEMDEBUG

    int m_totalMemSize;
    int m_blocknums;

#endif //MEMDEBUG
}; // CMemoryManager

} // namespace tsk

#endif //__MEMORY_H__