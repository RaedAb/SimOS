// Raed Abuzaid

#include "MemoryManager.hpp"
#include <iostream>

// Constructor
MemoryManager::MemoryManager(unsigned long long amountOfRAM, unsigned int pageSize)
    : pageSize_(pageSize), remainingMemory_(amountOfRAM / pageSize) {}

/**
 * Allocates memory for process
 * @param pid : process pid
 * @param address : process logical address
 */
void MemoryManager::accessAddress(int pid, unsigned long long address)
{
    int pageNumber = address / pageSize_;

    // Page table lookup
    auto pageKey = std::make_pair(pid, pageNumber);
    auto it = pageTable_.find(pageKey);

    // Check if page is already in memory
    if (it != pageTable_.end())
    {
        // If found, update the frame to recently used
        unsigned long long frame = it->second;
        frames_.remove(frame);
        frames_.push_front(frame);
        return;
    }

    // If memory is full, replace the least recently used frame
    if (remainingMemory_ == 0)
    {
        unsigned long long frameToReplace = frames_.back();
        frames_.pop_back();

        // Remove the old page entry from the page table
        auto oldPageKey = std::make_pair(memory_[frameToReplace].PID, memory_[frameToReplace].pageNumber);
        pageTable_.erase(oldPageKey);

        // Update the memory frame with the new page
        memory_[frameToReplace] = MemoryItem(pid, pageNumber, frameToReplace);

        // Mark the frame as recently used
        frames_.push_front(frameToReplace);

        // add to page table
        pageTable_[pageKey] = frameToReplace;
    }
    else
    {
        // If there is free memory, allocate a new frame
        unsigned long long frameNum = memory_.size();
        memory_.push_back(MemoryItem(pid, pageNumber, frameNum));

        // Mark the new frame as recently used
        frames_.push_front(frameNum);
        remainingMemory_--;

        // add to page table
        pageTable_[pageKey] = frameNum;
    }
}

/**
 * Deallocates all memory associated with process pid
 * @param pid : process pid
 */
void MemoryManager::deallocateMemory(int pid)
{
    auto it = memory_.begin();

    // Iterate through the memory items and remove those with the pid
    while (it != memory_.end())
    {
        if (it->PID == pid)
        {
            // Remove the frame from the frames list
            frames_.remove(it->frameNumber);

            // Remove entry from the page table
            auto pageKey = std::make_pair(it->PID, it->pageNumber);
            pageTable_.erase(pageKey);

            // Erase the memory item and increment the remaining memory count
            it = memory_.erase(it);
            remainingMemory_++;
        }
        else
        {
            ++it;
        }
    }
}

/**
 * @return : memory vector
 */
MemoryUsage MemoryManager::getMemoryUsage()
{
    return memory_;
}