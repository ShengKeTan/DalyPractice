#include "memory.h"
#include <iostream>

tsk::CMemoryManager mem;

int main(int argc, char **argv)
{

    std::cout << sizeof(tsk::memoryblock_t) << std::endl;
    std::cout << sizeof(int) << std::endl;
    std::cout << sizeof(unsigned long) << std::endl;
     std::cout << sizeof(void *) << std::endl;

    char *test_str = (char *)mem.GetClearMemory(21);
    for (int i = 0; i < 20; ++i)
    {
        test_str[i] = 'a' + i;
    }

    mem.PrintMemoryLabels();

    for (int i = 0; i < 20; ++i)
    {
        std::cout << test_str[i] << " ";
    }
    std::cout << std::endl;
    mem.FreeMemory(test_str);
    mem.PrintMemoryLabels();
    return 0;
}