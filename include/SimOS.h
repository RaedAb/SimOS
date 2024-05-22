
// Raed Abuzaid

#ifndef SIM_OS_H_
#define SIM_OS_H_

#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include "ProcessManager.hpp"
#include "DiskManager.hpp"
#include "MemoryManager.hpp"
#include "CPU.hpp"

class SimOS
{
private:
    ProcessManager processManager_;
    DiskManager diskManager_;
    MemoryManager memoryManager_;
    CPU cpu_;

public:
    /**
     * Creates a SimOS Object.
     *
     * @param numberOfDisks : number of hard disks in the simulated computer.
     * @param amountOfRAM : amount of memory
     * @param pageSize : page size
     */
    SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize);

    /**
     * Creates a new process and adds it to the ready queue. Every process in the simulated system has a PID.
     * The sim assigns PIDs to new processes starting from 1 and increments it by one for each new process.
     * PIDs are never recycled
     */
    void NewProcess();

    /**
     * The currently running process forks a child. The child is placed in the end of the ready-queue.
     */
    void SimFork();

    /**
     * The process that is currently using the CPU terminates.
     * Make sure you release the memory used by this process immediately.
     * If its parent is already waiting, the process terminates immediately and the parent becomes runnable (goes to the ready-queue).
     * If its parent hasn't called wait yet, the process turns into zombie.
     * To avoid the appearance of orphans, the system implements cascading termination.
     * Cascading termination means that if a process terminates, all its descendants terminate with it.
     */
    void SimExit();

    /**
     * The process wants to pause and wait for any of its child processes to terminate.
     * Once the wait is over, the process goes to the end of the ready-queue or the CPU.
     * If the zombie-child already exists, the process proceeds right away (keeps using the CPU) and the zombie-child disappears.
     * If more than one zombie-child exists, the system uses one of them (any!) to immediately resume the parent, while other zombies keep waiting for the next wait from the parent.
     */
    void SimWait();

    /**
     * Interrupt arrives from the timer signaling that the time slice of the currently running process is over.
     */
    void TimerInterrupt();

    /**
     * Currently running process requests to read the specified file from the disk with a given number.
     * The process issuing disk reading requests immediately stops using the CPU, even if the ready-queue is empty.
     *
     * @param diskNumber : the number of the disk to read from.
     * @param fileName : the name of the file to read.
     */
    void DiskReadRequest(int diskNumber, std::string fileName);

    /**
     * A disk with a specified number reports that a single job is completed.
     * The served process should return to the ready-queue.
     *
     * @param diskNumber : the number of the disk that completed a job.
     */
    void DiskJobCompleted(int diskNumber);

    /**
     * Currently running process wants to access the specified logical memory address.
     * System makes sure the corresponding page is loaded in the RAM.
     * If the corresponding page is already in the RAM, its “recently used” information is updated.
     *
     * @param address : the logical memory address to access.
     */
    void AccessMemoryAddress(unsigned long long address);

    /**
     * @return : GetCPU returns the PID of the process currently using the CPU.
     *           If CPU is idle it returns NO_PROCESS.
     */
    int GetCPU();

    /**
     * @return : GetReadyQueue returns the std::deque with PIDs of processes in the ready-queue where element in front corresponds start of the ready-queue.
     */
    std::deque<int> GetReadyQueue();

    /**
     * @return : GetMemory returns MemoryUsage vector describing all currently used frames of RAM.
     *           Terminated “zombie” processes don’t use memory, so they don’t contribute to memory usage.
     *           MemoryItems appear in the MemoryUsage vector in the order they appear in memory (from low addresses to high).
     */
    MemoryUsage GetMemory();

    /**
     * @param diskNumber : the number of the disk to query.
     * @return : GetDisk returns an object with PID of the process served by specified disk and the name of the file read for that process.
     *           If the disk is idle, GetDisk returns the default FileReadRequest object (with PID 0 and empty string in fileName).
     */
    FileReadRequest GetDisk(int diskNumber);

    /**
     * @param diskNumber : the number of the disk to query.
     * @return : GetDiskQueue returns the I/O-queue of the specified disk starting from the “next to be served” process.
     */
    std::deque<FileReadRequest> GetDiskQueue(int diskNumber);
};

#endif // SIM_OS_H_