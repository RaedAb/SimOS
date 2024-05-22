
// Raed Abuzaid

#include "CPU.hpp"

// Default constructor
CPU::CPU() : runningProcess_(NO_PROCESS) {}

/**
 * Starts a process
 */
void CPU::startProcess()
{
    if (!readyQueue_.empty())
    {
        runningProcess_ = readyQueue_.front();
        readyQueue_.pop_front();
    }
}

/**
 * @param: process pid
 * Adds process to CPU Ready queue
 */
void CPU::addProcess(int pid)
{
    readyQueue_.push_back(pid);
}

/**
 * Sets running process to no process
 */
void CPU::removeRunningProcess()
{
    runningProcess_ = NO_PROCESS;
}

/**
 * Handles TImer interrupt from OS by pussing running process to back of ready queue
 * STarting new process
 */
void CPU::handleTimerInterrupt()
{
    if (!readyQueue_.empty())
    {
        readyQueue_.push_back(runningProcess_); // put process to back of ready queue
        startProcess();
    }
}

/**
 * @return: currently running process
 */
int CPU::getRunningProcess()
{
    return runningProcess_;
}

/**
 * @return: the ready queue
 */
std::deque<int> CPU::getReadyQueue()
{
    return readyQueue_;
}

/**
 * Removed process from cpu ready queue
 */
void CPU::removeFromReadyQueue(int pid)
{
    readyQueue_.erase(std::remove(readyQueue_.begin(), readyQueue_.end(), pid), readyQueue_.end());
}