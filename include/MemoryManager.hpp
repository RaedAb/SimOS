// Raed Abuzaid

#ifndef MEMORY_MANAGER_HPP_
#define MEMORY_MANAGER_HPP_

#include <list>
#include <map>
#include <vector>

struct MemoryItem
{
    unsigned long long pageNumber;
    unsigned long long frameNumber;
    int PID; // PID of the process using this frame of memory

    // Default Constructor
    MemoryItem() : pageNumber(0), frameNumber(0), PID(-1) {}

    MemoryItem(int pid, unsigned long long page, unsigned long long frame)
        : pageNumber(page), frameNumber(frame), PID(pid) {}
};

using MemoryUsage = std::vector<MemoryItem>;

class MemoryManager
{
private:
    unsigned long long pageSize_;
    unsigned long long remainingMemory_;   // number of unsused frames left
    std::list<unsigned long long> frames_; // from recent to least recent
    std::map<std::pair<int, unsigned long long>, unsigned long long> pageTable_;
    MemoryUsage memory_;

public:
    // Constructor
    MemoryManager(unsigned long long amountOfRAM, unsigned int pageSize);

    /**
     * Allocates memory for process
     * @param pid : proces pid
     * @param address : process logical address
     */
    void accessAddress(int pid, unsigned long long address);

    /**
     * Deallocates all memory accociated with process pid
     * @param pid : process pid
     */
    void deallocateMemory(int pid);

    /**
     * @return : memory vector
     */
    MemoryUsage getMemoryUsage();
};

#endif // MEMORY_MANAGER_HPP_