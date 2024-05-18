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
    processes_[nextPID_] = newProcess;
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
void SimFork()
{
}