// Raed Abuzaid

#include "ProcessManager.hpp"
#include <vector>
#include <algorithm>

/**
 * Creates PRocess Manager object. Sets lowest PID to 1
 */
ProcessManager::ProcessManager() : nextPID_(1) {}

/**
 * Creates a process, adds it to processes map, increments pid
 * @return : PID of new process
 */
int ProcessManager::createProcess()
{
    Process newProcess(nextPID_);
    processes_[newProcess.PID] = newProcess;
    nextPID_++;

    return newProcess.PID;
}

/**
 * Forks currently running process
 * @param parentPID : PID of child
 * @return : child pid
 */
int ProcessManager::forkProcess(int parentPID)
{
    // Track parent, create child
    Process &parent = processes_[parentPID];
    Process child(nextPID_, parentPID);

    // add child to processes, and parents child vector
    processes_[child.PID] = child;
    parent.childrenPIDs.push_back(child.PID);
    nextPID_++;

    return child.PID;
}

/**
 * Terminates process, as well as all decendent processes
 * If parent is waiting automatically terminate, else process becomes zombie
 * @param pid : PID of process
 * @param cpu : refrence to cpu
 * @param cpu : refrence to disk manager
 * @param memoryManager : refrence to memory manager
 */
void ProcessManager::terminateProcess(int pid, CPU &cpu, MemoryManager &memoryManager, DiskManager &diskManager)
{
    Process &process = processes_[pid];

    // release memory, delete disk requests, cascading terminate chilren as well
    memoryManager.deallocateMemory(pid);
    diskManager.deleteRequests(pid);
    if (process.childrenPIDs.size() > 0)
    {
        cascadingTerminate(pid, cpu, memoryManager, diskManager);
    }

    // check if process has parent
    if (process.parentPID != -1)
    {
        Process &parent = processes_[process.parentPID];

        if (parent.isWaiting) // remove process from processes and from parents children vector
        {
            processes_.erase(pid);
            parent.childrenPIDs.erase(std::remove(parent.childrenPIDs.begin(), parent.childrenPIDs.end(), pid), parent.childrenPIDs.end());
            parent.isWaiting = false;
            cpu.addProcess(parent.PID);
        }
        else
        {
            process.isZombie = true;
        }
    }
    else
    {
        processes_.erase(pid);
    }
}

/**
 * Process pauses and waits for any child process to terminate
 * If child process is already a zombie, process resumes without stopping
 * If more than one zombie-child exists, the system uses one of them (any!) to immediately resume the parent,
 * while other zombies keep waiting for the next wait from the parent.
 * @param pid : PID of Process
 * @param cpu : Refrence to cpu
 */
void ProcessManager::waitProcess(int pid, CPU &cpu)
{
    Process &process = processes_[pid];

    bool resume = false;
    for (auto it = process.childrenPIDs.begin(); it != process.childrenPIDs.end(); ++it)
    {
        if (processes_[*it].isZombie)
        {
            process.childrenPIDs.erase(it);
            processes_.erase(*it);
            resume = true;
            break;
        }
    }

    if (!resume)
    {
        process.isWaiting = true;
        cpu.removeRunningProcess();
    }
}

/**
 * Cascading terminate to prevent orphans when a process is terminated
 * @param pid : process pid
 * @param cpu : refrence to cpu
 * @param diskManager : refrence to disk manager
 * @param memoryManager : refrence to memory manager
 */
void ProcessManager::cascadingTerminate(int pid, CPU &cpu, MemoryManager &memoryManager, DiskManager &diskManager)
{
    Process &process = processes_[pid];

    for (int childPID : process.childrenPIDs)
    {
        if (processes_[childPID].childrenPIDs.size() > 0)
        {
            cascadingTerminate(childPID, cpu, memoryManager, diskManager);
        }

        // Delete requests made by the child process from the DiskManager
        diskManager.deleteRequests(childPID);

        // Remove the child process from the CPU's ready queue
        cpu.removeFromReadyQueue(childPID);

        // deallocate chils memory
        memoryManager.deallocateMemory(childPID);
    }
}