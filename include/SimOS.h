#ifndef SIM_OS_H_
#define SIM_OS_H_

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

struct FileReadRequest
{
    int PID{0};
    std::string fileName{""};
};

struct MemoryItem
{
    unsigned long long pageNumber;
    unsigned long long frameNumber;
    int PID; // PID of the process using this frame of memory
};

struct Process
{
    int PID;
    int parentPID;
    std::vector<int> childrenPIDs;
    bool isZombie;

    Process(int pid, int parentPid = -1)
        : PID(pid), parentPID(parentPid), isZombie(false) {}
};

using MemoryUsage = std::vector<MemoryItem>;

constexpr int NO_PROCESS{0};

class SimOS
{
private:
    int numberOfDisks_;
    unsigned long long amountOfRAM_;
    unsigned int pageSize_;
    int nextPID_;
    int runningProcess_ = NO_PROCESS;
    std::deque<int> readyQueue_;
    std::unordered_map<int, Process> processes_;
    std::unordered_map<int, std::deque<FileReadRequest>> diskQueues_;
    std::unordered_map<int, FileReadRequest> diskStatus_;
    MemoryUsage memory_;

    void releaseMemory(int pid);
    void cascadingTerminate(int pid);

public:
    /**
     * @param numberOfDisks : number of hard disks in the simulated computer.
     * @param amountOfRAM : amount of memory
     * @param pageSize : page size
     * @post : Creates a SimOS Object.
     */
    SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize);

    /**
     * @post : Creates a new process and adds it to the ready queue. Every process in the simulated system has a PID.
     *         The sim assigns PIDs to new processes starting from 1 and increments it by one for each new process.
     *         PIDs are never recycled
     */
    void NewProcess();

    /**
     * @post : The currently running process forks a child. The child is placed in the end of the ready-queue.
     */
    void SimFork();

    /**
     * @post : The process that is currently using the CPU terminates.
     *         Make sure you release the memory used by this process immediately.
     *         If its parent is already waiting, the process terminates immediately and the parent becomes runnable (goes to the ready-queue).
     *         If its parent hasn't called wait yet, the process turns into zombie.
     *         To avoid the appearance of orphans, the system implements cascading termination.
     *         Cascading termination means that if a process terminates, all its descendants terminate with it.
     */
    void SimExit();

    /**
     * @post : The process wants to pause and wait for any of its child processes to terminate.
     *         Once the wait is over, the process goes to the end of the ready-queue or the CPU.
     *         If the zombie-child already exists, the process proceeds right away (keeps using the CPU) and the zombie-child disappears.
     *         If more than one zombie-child exists, the system uses one of them (any!) to immediately resume the parent, while other zombies keep waiting for the next wait from the parent.
     */
    void SimWait();

    /**
     * @post : Interrupt arrives from the timer signaling that the time slice of the currently running process is over.
     */
    void TimerInterrupt();

    /**
     * @param diskNumber : the number of the disk to read from.
     * @param fileName : the name of the file to read.
     * @post : Currently running process requests to read the specified file from the disk with a given number.
     *         The process issuing disk reading requests immediately stops using the CPU, even if the ready-queue is empty.
     */
    void DiskReadRequest(int diskNumber, std::string fileName);

    /**
     * @param diskNumber : the number of the disk that completed a job.
     * @post : A disk with a specified number reports that a single job is completed.
     *         The served process should return to the ready-queue.
     */
    void DiskJobCompleted(int diskNumber);

    /**
     * @param address : the logical memory address to access.
     * @post : Currently running process wants to access the specified logical memory address.
     *         System makes sure the corresponding page is loaded in the RAM.
     *         If the corresponding page is already in the RAM, its “recently used” information is updated.
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
