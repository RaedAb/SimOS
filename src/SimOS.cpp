
// Raed Abuzaid

#include "SimOS.h"
#include <unordered_set>

/**
 * @param numberOfDisks : number of hard disks in the simulated computer.
 * @param amountOfRAM : amount of memory
 * @param pagesize : page size
 * @post : Creates a SimOS Object.
 */
SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize)
    : processManager_(), diskManager_(numberOfDisks), memoryManager_(amountOfRAM, pageSize), cpu_()
{
}

/**
 * @post : Creates a new process and adds it to the ready queue, or runs it if the cpu is idle.
 *         Every process in the simulated system has a PID.
 *         The sim assigns PIDs to new processes starting from 1 and increments it by one for each new process.
 *         PIDs are never recycled
 */
void SimOS::NewProcess()
{
    int pid = processManager_.createProcess();
    cpu_.addProcess(pid);

    if (cpu_.getRunningProcess() == NO_PROCESS)
    {
        cpu_.startProcess();
    }
}

/**
 * @post : The currently running process forks a child. The child is placed in the end of the ready-queue.
 */
void SimOS::SimFork()
{
    if (cpu_.getRunningProcess() == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    int childPID = processManager_.forkProcess(cpu_.getRunningProcess());
    cpu_.addProcess(childPID);
}

/**
 * @post : The process that is currently using the CPU terminates.
 *         Memory used by this process is released
 *         If its parent is already waiting, the process terminates immediately and the parent becomes runnable (goes to the ready-queue).
 *         If its parent hasn't called wait yet, the process turns into zombie.
 *         To avoid the appearance of orphans, the system implements cascading termination.
 */
void SimOS::SimExit()
{
    if (cpu_.getRunningProcess() == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    int pid = cpu_.getRunningProcess();
    cpu_.removeRunningProcess();
    processManager_.terminateProcess(pid, cpu_, memoryManager_, diskManager_);

    cpu_.startProcess();
}

/**
 * @post : The process wants to pause and wait for any of its child processes to terminate.
 *         Once the wait is over, the process goes to the end of the ready-queue or the CPU.
 *         If the zombie-child already exists, the process proceeds right away (keeps using the CPU) and the zombie-child disappears.
 *         If more than one zombie-child exists, the system uses one of them (any!) to immediately resume the parent, while other zombies keep waiting for the next wait from the parent.
 */
void SimOS::SimWait()
{
    if (cpu_.getRunningProcess() == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    processManager_.waitProcess(cpu_.getRunningProcess(), cpu_);

    if (cpu_.getRunningProcess() == NO_PROCESS)
    {
        cpu_.startProcess();
    }
}

/**
 * @post : Interrupt arrives from the timer signaling that the time slice of the currently running process is over.
 */
void SimOS::TimerInterrupt()
{
    if (cpu_.getRunningProcess() == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    cpu_.handleTimerInterrupt();
}

/**
 * @param diskNumber : the number of the disk to read from.
 * @param fileName : the name of the file to read.
 * @post : Currently running process requests to read the specified file from the disk with a given number.
 *         The process issuing disk reading requests immediately stops using the CPU, even if the ready-queue is empty.
 */
void SimOS::DiskReadRequest(int diskNumber, std::string fileName)
{
    if (cpu_.getRunningProcess() == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }
    if (diskNumber > diskManager_.getNumberOfDisks() - 1)
    {
        throw std::logic_error("Requested disk out of range.");
    }

    diskManager_.readRequest(cpu_.getRunningProcess(), diskNumber, fileName);
    cpu_.removeRunningProcess();

    // start new
    cpu_.startProcess();
}

/**
 * @param diskNumber : the number of the disk that completed a job.
 * @post : A disk with a specified number reports that a single job is completed.
 *         The served process should return to the ready-queue.
 */
void SimOS::DiskJobCompleted(int diskNumber)
{
    if (diskNumber > diskManager_.getNumberOfDisks() - 1)
    {
        throw std::logic_error("Requested disk out of range.");
    }

    if (diskManager_.getDiskStatus(diskNumber).PID != 0)
    {
        int pid = diskManager_.completeJob(diskNumber);
        cpu_.addProcess(pid);

        if (cpu_.getRunningProcess() == NO_PROCESS)
        {
            cpu_.startProcess();
        }
    }
}

/**
 * @param address : the logical memory address to access.
 * @post : Currently running process wants to access the specified logical memory address.
 *         System makes sure the corresponding page is loaded in the RAM.
 *         If the corresponding page is already in the RAM, its “recently used” information is updated.
 */
void SimOS::AccessMemoryAddress(unsigned long long address)
{
    memoryManager_.accessAddress(cpu_.getRunningProcess(), address);
}

/**
 * @return : GetCPU returns the PID of the process currently using the CPU.
 *           If CPU is idle it returns NO_PROCESS.
 */
int SimOS::GetCPU()
{
    return cpu_.getRunningProcess();
}

/**
 * @return : GetReadyQueue returns the std::deque with PIDs of processes in the ready-queue where element in front corresponds start of the ready-queue.
 */
std::deque<int> SimOS::GetReadyQueue()
{
    return cpu_.getReadyQueue();
}

/**
 * @return : GetMemory returns MemoryUsage vector describing all currently used frames of RAM.
 *           Terminated “zombie” processes don’t use memory, so they don’t contribute to memory usage.
 *           MemoryItems appear in the MemoryUsage vector in the order they appear in memory (from low addresses to high).
 */
MemoryUsage SimOS::GetMemory()
{
    return memoryManager_.getMemoryUsage();
}

/**
 * @param diskNumber : the number of the disk to query.
 * @return : GetDisk returns an object with PID of the process served by specified disk and the name of the file read for that process.
 *           If the disk is idle, GetDisk returns the default FileReadRequest object (with PID 0 and empty string in fileName).
 */
FileReadRequest SimOS::GetDisk(int diskNumber)
{
    if (diskNumber > diskManager_.getNumberOfDisks() - 1)
    {
        throw std::logic_error("Requested disk out of range.");
    }

    FileReadRequest request(diskManager_.getDiskStatus(diskNumber));

    return request;
}

/**
 * @param diskNumber : the number of the disk to query.
 * @return : GetDiskQueue returns the I/O-queue of the specified disk starting from the “next to be served” process.
 */
std::deque<FileReadRequest> SimOS::GetDiskQueue(int diskNumber)
{
    if (diskNumber > diskManager_.getNumberOfDisks() - 1)
    {
        throw std::logic_error("Requested disk out of range.");
    }

    return diskManager_.getDiskQueue(diskNumber);
}