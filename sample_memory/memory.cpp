#include "list.h"
#include "memory.h"
#include "Log.h"

#define MAX_FILE_SIZE	(10 * 1024)
#define MAX_BACKUP_NUM	2
#define IO_BUFFER_SIZE	(10 * 1024)

log_handle_t* filehandle = NULL;
log_handle_t* streamhandle = NULL;
list<log_handle_t *> hlist;

#define Log_Info(format, ...)   log_info(hlist, format, ##__VA_ARGS__)
#define Log_Err(format, ...)  log_err(hlist, format, ##__VA_ARGS__)

void InitLog(void)
{
    //log_handle_t* fh1 = file_handle_create(data->file, MAX_FILE_SIZE, MAX_BACKUP_NUM, IO_BUFFER_SIZE, NORMALIZE, NULL);
	log_handle_t* sh1 = stream_handle_create(ERR_STRERR);

    sh1 = (log_handle_t *)set_stream_param((void *)sh1, (log_level_t)LOG_ERR, FRED, NULL, UNDERLINE);
	sh1 = (log_handle_t *)set_stream_param((void *)sh1, (log_level_t)LOG_WARN, FBLUE, NULL, UNDERLINE);
	sh1 = (log_handle_t *)set_stream_param((void *)sh1, (log_level_t)LOG_INFO, FGREEN, NULL, NULL);

   // hlist.push_back(fh1);
    hlist.push_back(sh1);
}

tsk::CMemoryManager::CMemoryManager() : m_totalAllocMemSize(0), memory(NULL)
{
    InitLog();

#ifdef MEMDEBUG

    m_totalMemSize = 0;
    m_blocknums = 0;

#endif //MEMDEBUG
}

tsk::CMemoryManager::~CMemoryManager()
{
#ifdef MEMDEBUG
    DumpMemory();
#else
    m_totalAllocMemSize = 0;
#endif

    log_destory(hlist);
}

void tsk::CMemoryManager::PrintMemorySize(unsigned long size)
{
    unsigned long number1 = 0;
    unsigned long number2 = 0;
    unsigned long number3 = 0;
    number1 = size >> 20;
    number2 = (size & 0xFFFFF) >> 10;
    number3 = (size & 0x3FF);
    if (number1) Log_Info("%ld MB", number1);
    if (number1 && number2)  Log_Info(" and ");
    if (number2)  Log_Info("%ld KB", number2);
    if (number2 && number3) Log_Info(" and ");
    if (number3)  Log_Info("%ld bytes\n", number3);
}

#ifndef MEMDEBUG

int tsk::CMemoryManager::MenorySize(void *ptr)
{
#if defined(WIN32) || defined(_WIN32)
    #ifdef __WATCOMC__
    return (_msize(ptr) + 15) >> 4 << 4;
    #else
    return _msize(ptr);
    #endif
#else
    return 0;
#endif
}

void * tsk::CMemoryManager::GetClearMemory(int size)
{
    void *ptr = (void *)malloc(size);
    if (!ptr)
    {
        Log_Err("out of memory");
    }
    memset(ptr, 0, size);
    m_totalAllocMemSize += MenorySize(ptr);
    return ptr;
}

void *tsk::CMemoryManager::GetMemory(unsigned long size)
{
    void *ptr = (void *)malloc(size);
    if (!ptr)
    {
        Log_Err("out of memory");
    }
    m_totalAllocMemSize += MenorySize(ptr);
    return ptr;
}

void tsk::CMemoryManager::FreeMemory(void *ptr)
{
    m_totalAllocMemSize -= MenorySize(ptr);
    free(ptr);
}

int tsk::CMemoryManager::TotalAllocatedMemory(void)
{
    return m_totalAllocMemSize;
}

#else

#define MEM_ID  0x12345678l

void tsk::CMemoryManager::LinkMemoryBlock(memoryblock_t *block)
{
    block->prev = NULL;
    block->next = memory;
    if (memory)
    {
        memory->prev = block;
    }
    memory = block;
}

void tsk::CMemoryManager::UnlinkMemoryBlock(memoryblock_t *block)
{
    if (block->prev)
    {
        block->prev->next = block->next;
    }
    else
    {
        memory = block->next;
    }

    if (block->next)
    {
        block->next->prev = block->prev;
    }
}

#ifdef MEMDEBUG

void *tsk::CMemoryManager::GetMemoryDebug(unsigned long size, const char *label, const char *file, int line)
#else
void *tsk::CMemoryManager::GetMemory(unsigned long size)
#endif
{
    void *ptr = malloc(size + sizeof(memoryblock_t));
    if (!ptr)
    {
        Log_Err("out of memory!\n");
        return NULL;
    }

    memoryblock_t *block = (memoryblock_t *)ptr;
    block->id = MEM_ID;
    block->ptr = (char *)ptr + sizeof(memoryblock_t);
    block->size = size + sizeof(memoryblock_t);

#ifdef MEMDEBUG
    block->label = label;
    block->file = file;
    block->line = line;
#endif

    LinkMemoryBlock(block);
    m_totalMemSize += block->size;
    m_blocknums++;
    return block->ptr;
}

#ifdef MEMDEBUG
void *tsk::CMemoryManager::GetClearMemoryDebug(unsigned long size, const char *label, const char *file, int line)
#else
void *tsk::CMemotyManager::GetClearMmoey(unsigned long size)
#endif
{
#ifdef MEMDEBUG
    void *ptr = GetMemoryDebug(size, label, file, line);
#else
    void *ptr = GetMemory(size);
#endif

    memset(ptr, 0, size);
    return ptr;
}

tsk::memoryblock_t *tsk::CMemoryManager::BlockFromPointer(void *ptr, const char *str)
{
    if (!ptr)
    {
#ifdef MEMDEBUG
        Log_Err("%s: NULL pointer\n", str);
#endif // MEMDEBUG
        return NULL;
    }

    memoryblock_t *block = (memoryblock_t *)((char *)ptr - sizeof(memoryblock_t));
    if (block->id != MEM_ID)
    {
        Log_Err("%s: incalid memory block\n", str);
    }
    if (block->ptr != ptr)
    {
        Log_Err("%s: memory block pointer invalid\n", str);
    }
    return block;
}

void tsk::CMemoryManager::FreeMemory(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    memoryblock_t *block = BlockFromPointer(ptr, "FreeMemory");
    if (!block)
    {
        return;
    }

    UnlinkMemoryBlock(block);
    m_totalMemSize -= (block)->size;
    m_blocknums--;
    free(block);
    block = NULL;
}

int tsk::CMemoryManager::MenorySize(void *ptr)
{
    memoryblock_t *block = BlockFromPointer(ptr, "MemoryByteSize");
    if (!block)
    {
        return 0;
    }
    return block->size;
}

void tsk::CMemoryManager::PrintMemoryLabels(void)
{
    Log_Info("total botlib memory: %d Bytes\n", m_totalMemSize);
    Log_Info("total memory blocks: %d\n", m_blocknums);

    int nums = 0;
    memoryblock_t *block = memory;
    while (block != NULL)
    {
#ifdef MEMDEBUG
        Log_Info("%6d, %p, %8d: %24s line %6d: %s\n", nums, block->ptr, 
            block->size, block->file, block->line, block->label);
#endif
        nums++;
        block = block->next;
    }
}

void tsk::CMemoryManager::DumpMemory(void)
{
    for (memoryblock_t *block = memory; block != NULL; block = memory)
    {
        FreeMemory(block->ptr);
    }
    memory = NULL;
    m_totalMemSize = 0;
}

int tsk::CMemoryManager::TotalAllocatedMemory(void)
{
    return m_totalMemSize;
}

#endif //MEMDEBUF