#include "../include/SimOS.h"

/**
 * @param numberOfDisks : number of hard disks in the simulated computer.
 * @param amountOfRAM : amount of memory
 * @param pagesize : page size
 * @post : Creates a SimOS Object.
 */
SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize)
    : numberOfDisks_(numberOfDisks), amountOfRAM_(amountOfRAM), pageSize_(pageSize), nextPID_(1), runningProcess_(NO_PROCESS)
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
    // create new process
    Process newProcess(nextPID_);
    processes_[newProcess.PID] = newProcess;
    nextPID_++;

    readyQueue_.push_back(nextPID_);
    if (runningProcess_ == NO_PROCESS) // start process if CPU idle
    {
        runningProcess_ = newProcess.PID;
    }
    else // put in ready queue
    {
        readyQueue_.push_back(newProcess.PID);
    }
}

/**
 * @post : The currently running process forks a child. The child is placed in the end of the ready-queue.
 */
void SimOS::SimFork()
{
    if (runningProcess_ == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    Process &parent = processes_[runningProcess_];

    // create child
    Process childProcess(nextPID_, parent.PID);
    processes_[childProcess.PID] = childProcess;
    nextPID_++;

    parent.childrenPIDs.push_back(childProcess.PID);
    readyQueue_.push_back(childProcess.PID);
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
    if (runningProcess_ == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    // stop process
    int pid = runningProcess_;
    runningProcess_ = NO_PROCESS;

    Process &process = processes_[pid];
    Process &parent = processes_[process.parentPID];

    // release memory and end child processes
    releaseMemory(pid);
    cascadingTerminate(pid);

    // check if parent waiting
    if (parent.isWaiting)
    {
        // remove process from processes and from parents children vector
        processes_.erase(process.PID);
        parent.childrenPIDs.erase(std::remove(parent.childrenPIDs.begin(), parent.childrenPIDs.end(), pid), parent.childrenPIDs.end());

        // parent put in ready queue
        parent.isWaiting = false;
        readyQueue_.push_back(parent.PID);
    }
    else
    {
        process.isZombie = true;
    }

    // start new process
    if (!readyQueue_.empty())
    {
        runningProcess_ = readyQueue_.front();
        readyQueue_.pop_front();
    }
}

/**
 * @post : The process wants to pause and wait for any of its child processes to terminate.
 *         Once the wait is over, the process goes to the end of the ready-queue or the CPU.
 *         If the zombie-child already exists, the process proceeds right away (keeps using the CPU) and the zombie-child disappears.
 *         If more than one zombie-child exists, the system uses one of them (any!) to immediately resume the parent, while other zombies keep waiting for the next wait from the parent.
 */
void SimOS::SimWait()
{
    if (runningProcess_ == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    Process &process = processes_[runningProcess_];

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

        // start new process
        if (!readyQueue_.empty())
        {
            runningProcess_ = readyQueue_.front();
            readyQueue_.pop_front();
        }
    }
}

/**
 * @post : Interrupt arrives from the timer signaling that the time slice of the currently running process is over.
 */
void SimOS::TimerInterrupt()
{
    if (runningProcess_ == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    if (!readyQueue_.empty())
    {
        readyQueue_.push_back(runningProcess_); // put process to back of ready queue

        // start new process
        runningProcess_ = readyQueue_.front();
        readyQueue_.pop_front();
    }
}

/**
 * @param diskNumber : the number of the disk to read from.
 * @param fileName : the name of the file to read.
 * @post : Currently running process requests to read the specified file from the disk with a given number.
 *         The process issuing disk reading requests immediately stops using the CPU, even if the ready-queue is empty.
 */
void SimOS::DiskReadRequest(int diskNumber, std::string fileName)
{
    if (runningProcess_ == NO_PROCESS)
    {
        throw std::logic_error("No process currently using the CPU.");
    }

    // create request
    FileReadRequest request(runningProcess_, fileName);

    diskQueues_[diskNumber].push_back(request);

    // stop process, start new if available
    if (!readyQueue_.empty())
    {
        runningProcess_ = readyQueue_.front();
        readyQueue_.pop_front();
    } else
    {
        runningProcess_ = NO_PROCESS;
    }

}

/**
 * @param diskNumber : the number of the disk that completed a job.
 * @post : A disk with a specified number reports that a single job is completed.
 *         The served process should return to the ready-queue.
 */
void SimOS::DiskJobCompleted(int diskNumber)
{
    // complete job
    int servedProcess = diskQueues_[diskNumber].front().PID;
    diskQueues_[diskNumber].pop_front();

    // return to ready queue
    if (runningProcess_ == NO_PROCESS)
    {
        runningProcess_ = servedProcess;
    } else
    {
        readyQueue_.push_back(servedProcess);
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
}

/**
 * @return : GetCPU returns the PID of the process currently using the CPU.
 *           If CPU is idle it returns NO_PROCESS.
 */
int SimOS::GetCPU()
{
    return runningProcess_;
}

/**
 * @return : GetReadyQueue returns the std::deque with PIDs of processes in the ready-queue where element in front corresponds start of the ready-queue.
 */
std::deque<int> SimOS::GetReadyQueue()
{
    return readyQueue_;
}

/**
 * @return : GetMemory returns MemoryUsage vector describing all currently used frames of RAM.
 *           Terminated “zombie” processes don’t use memory, so they don’t contribute to memory usage.
 *           MemoryItems appear in the MemoryUsage vector in the order they appear in memory (from low addresses to high).
 */
MemoryUsage SimOS::GetMemory()
{
    return memory_;
}

/**
 * @param diskNumber : the number of the disk to query.
 * @return : GetDisk returns an object with PID of the process served by specified disk and the name of the file read for that process.
 *           If the disk is idle, GetDisk returns the default FileReadRequest object (with PID 0 and empty string in fileName).
 */
FileReadRequest SimOS::GetDisk(int diskNumber)
{
    FileReadRequest idle(0, "");
    
    if (diskQueues_[diskNumber].size() != 0)
    {
        return diskQueues_[diskNumber].front();
    }
    else
    {
        return idle;
    }
}

/**
 * @param diskNumber : the number of the disk to query.
 * @return : GetDiskQueue returns the I/O-queue of the specified disk starting from the “next to be served” process.
 */
std::deque<FileReadRequest> SimOS::GetDiskQueue(int diskNumber)
{
    return diskQueues_[0];
}

/**
 * Helper Functions
 */
void SimOS::releaseMemory(int pid)
{
}

void SimOS::cascadingTerminate(int pid)
{
    Process &process = processes_[pid];

    // Recursively terminate all children of the current process
    for (int childPID : process.childrenPIDs)
    {
        cascadingTerminate(childPID);
        processes_.erase(childPID);
    }
}
