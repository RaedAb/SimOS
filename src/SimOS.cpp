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

    // release memory and end  child processes
    releaseMemory(pid);
    cascadingTerminate(pid);

    // check if parent waiting
    if (parent.isWaiting)
    {
        processes_.erase(process.PID);

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